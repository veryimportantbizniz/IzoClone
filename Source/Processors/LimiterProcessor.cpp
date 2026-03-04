#include "LimiterProcessor.h"

void LimiterProcessor::prepare(const juce::dsp::ProcessSpec& spec,
                                 juce::AudioProcessorValueTreeState& apvts)
{
    sampleRate = spec.sampleRate;
    delayBuffer.setSize(2, MAX_LOOKAHEAD_SAMPLES);
    delayBuffer.clear();
    delayWritePos = 0;
    currentGain   = 1.0f;

    prevCeiling = 999.f;
    updateParams(apvts);
}

void LimiterProcessor::updateParams(juce::AudioProcessorValueTreeState& apvts)
{
    float newCeiling   = apvts.getRawParameterValue("limiter_ceiling")  ->load();
    float newLookahead = apvts.getRawParameterValue("limiter_lookahead")->load();
    float newRelease   = apvts.getRawParameterValue("limiter_release")  ->load();

    if (newCeiling != prevCeiling) {
        ceiling     = juce::Decibels::decibelsToGain(newCeiling);
        prevCeiling = newCeiling;
    }
    if (newLookahead != prevLookahead) {
        lookaheadSamples = juce::roundToInt(newLookahead * 0.001f * (float)sampleRate);
        lookaheadSamples = juce::jlimit(0, MAX_LOOKAHEAD_SAMPLES - 1, lookaheadSamples);
        prevLookahead    = newLookahead;
    }
    if (newRelease != prevRelease) {
        // One-pole IIR release coefficient
        releaseCoeff = std::exp(-1.0f / (newRelease * 0.001f * (float)sampleRate));
        prevRelease  = newRelease;
    }
}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void LimiterProcessor::process(juce::dsp::ProcessContextReplacing<float>& context,
                                 juce::AudioProcessorValueTreeState& apvts)
{
    updateParams(apvts);

    auto& block   = context.getOutputBlock();
    auto numSamps = (int)block.getNumSamples();
    auto* dataL   = block.getChannelPointer(0);
    auto* dataR   = block.getChannelPointer(1);
    auto* delayL  = delayBuffer.getWritePointer(0);
    auto* delayR  = delayBuffer.getWritePointer(1);

    float maxGainReduction = 1.0f;  // track per-block GR for meter

    for (int n = 0; n < numSamps; ++n)
    {
        // Write incoming sample into the delay line
        delayL[delayWritePos] = dataL[n];
        delayR[delayWritePos] = dataR[n];

        // Read the lookahead-delayed sample (what we'll actually output)
        int readPos = (delayWritePos - lookaheadSamples + MAX_LOOKAHEAD_SAMPLES)
                      % MAX_LOOKAHEAD_SAMPLES;

        float outL = delayL[readPos];
        float outR = delayR[readPos];

        // Compute required gain to keep within ceiling â€” look at current input
        float peak = std::max(std::abs(dataL[n]), std::abs(dataR[n]));
        float targetGain = (peak > ceiling) ? ceiling / peak : 1.0f;

        // Smooth gain: attack is instant (catch transients), release is smooth
        if (targetGain < currentGain)
            currentGain = targetGain;                          // Instant attack
        else
            currentGain += (1.0f - releaseCoeff) * (targetGain - currentGain); // Smooth release

        dataL[n] = outL * currentGain;
        dataR[n] = outR * currentGain;

        if (currentGain < maxGainReduction)
            maxGainReduction = currentGain;

        delayWritePos = (delayWritePos + 1) % MAX_LOOKAHEAD_SAMPLES;
    }

    // Store GR in dB for the meter (atomic, safe for UI thread to read)
    gainReductionDb.store(juce::Decibels::gainToDecibels(maxGainReduction),
                           std::memory_order_relaxed);
}

