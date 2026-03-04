#include "AllProcessors.h"

void ExciterProcessor::prepare(const juce::dsp::ProcessSpec& spec,
                                 juce::AudioProcessorValueTreeState& apvts)
{
    sampleRate = spec.sampleRate;

    auto monoSpec = spec;
    monoSpec.numChannels = 1;
    hpFilter.prepare(monoSpec);
    hpFilter.setType(juce::dsp::StateVariableTPTFilterType::highpass);

    sideChainBuffer.setSize(2, (int)spec.maximumBlockSize);

    prevFreq = -1.f;
    updateFilter(apvts);
}

void ExciterProcessor::updateFilter(juce::AudioProcessorValueTreeState& apvts)
{
    float freq = apvts.getRawParameterValue("exciter_freq")->load();
    if (freq != prevFreq) {
        hpFilter.setCutoffFrequency(freq);
        prevFreq = freq;
    }
}

void ExciterProcessor::process(juce::dsp::ProcessContextReplacing<float>& context,
                                 juce::AudioProcessorValueTreeState& apvts)
{
    updateFilter(apvts);

    float drive = apvts.getRawParameterValue("exciter_drive")->load() / 100.f;
    float mix   = apvts.getRawParameterValue("exciter_mix")  ->load() / 100.f;

    if (mix < 0.001f) return;  // Skip processing if mix is zero

    auto& block   = context.getOutputBlock();
    int   numSamp = (int)block.getNumSamples();

    // Copy input into sidechain buffer
    for (int ch = 0; ch < 2; ++ch)
        sideChainBuffer.copyFrom(ch, 0, block.getChannelPointer(ch), numSamp);

    // HP filter the sidechain to isolate the exciter frequency band
    for (int ch = 0; ch < 2; ++ch)
    {
        auto* data = sideChainBuffer.getWritePointer(ch);
        for (int n = 0; n < numSamp; ++n)
            data[n] = hpFilter.processSample(ch, data[n]);
    }

    // Apply soft-clip saturation to generate harmonics
    float driveGain = 1.0f + drive * 9.0f;   // 1x - 10x drive range
    for (int ch = 0; ch < 2; ++ch)
    {
        auto* data = sideChainBuffer.getWritePointer(ch);
        for (int n = 0; n < numSamp; ++n)
            data[n] = softClip(data[n], driveGain);
    }

    // Parallel mix: blend saturated harmonics into dry signal
    for (int ch = 0; ch < 2; ++ch)
    {
        auto* out  = block.getChannelPointer(ch);
        auto* side = sideChainBuffer.getReadPointer(ch);
        for (int n = 0; n < numSamp; ++n)
            out[n] = out[n] * (1.0f - mix * 0.5f) + side[n] * mix;
    }
}

