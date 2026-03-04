#include "AllProcessors.h"

void StereoImagerProcessor::prepare(const juce::dsp::ProcessSpec& spec,
                                      juce::AudioProcessorValueTreeState& apvts)
{
    sampleRate = spec.sampleRate;

    auto monoSpec = spec;
    monoSpec.numChannels = 1;
    bassLPF.prepare(monoSpec);
    bassLPF.setType(juce::dsp::StateVariableTPTFilterType::lowpass);

    prevMonoLo = -1.f;
    prevWidth  = -1.f;
    updateParams(apvts);
}

void StereoImagerProcessor::updateParams(juce::AudioProcessorValueTreeState& apvts)
{
    float monoLo = apvts.getRawParameterValue("stereo_mono_lo")->load();
    if (monoLo != prevMonoLo) {
        bassLPF.setCutoffFrequency(monoLo);
        prevMonoLo = monoLo;
    }
}

void StereoImagerProcessor::process(juce::dsp::ProcessContextReplacing<float>& context,
                                      juce::AudioProcessorValueTreeState& apvts)
{
    updateParams(apvts);

    float width = apvts.getRawParameterValue("stereo_width")->load() / 100.f;

    auto& block   = context.getOutputBlock();
    int   numSamp = (int)block.getNumSamples();
    auto* L = block.getChannelPointer(0);
    auto* R = block.getChannelPointer(1);

    // M/S coefficients
    float mid  = (width <= 1.0f) ? 1.0f : (2.0f - width);
    float side = width;

    float corrSum = 0.f;

    for (int n = 0; n < numSamp; ++n)
    {
        float m = (L[n] + R[n]) * 0.5f;
        float s = (L[n] - R[n]) * 0.5f;

        // Widen/narrow by scaling side channel
        s *= side;

        // Bass mono: replace low-freq side content with zero
        // We process the mid channel through a LP to get the bass-only mid,
        // then enforce mono below the crossover by zeroing bass-side content
        float bassM = bassLPF.processSample(0, m);
        float bassS = 0.f;  // Bass side = 0 (mono bass)

        // High-pass remainder of side stays as-is
        float hiS = s - bassLPF.processSample(1, s);

        s = bassS + hiS;
        m *= mid;

        L[n] = m + s;
        R[n] = m - s;

        // Accumulate correlation (cos of angle between L and R)
        corrSum += L[n] * R[n];
    }

    // Update correlation meter (clamped -1 to 1)
    float corr = (numSamp > 0) ? juce::jlimit(-1.f, 1.f, corrSum / numSamp) : 1.f;
    correlation.store(corr, std::memory_order_relaxed);
}

