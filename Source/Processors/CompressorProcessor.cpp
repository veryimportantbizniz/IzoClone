#include "AllProcessors.h"

void CompressorProcessor::prepare(const juce::dsp::ProcessSpec& spec,
                                    juce::AudioProcessorValueTreeState& apvts)
{
    sampleRate = spec.sampleRate;

    // Prepare crossover filters (stereo)
    loSplitLP.prepare(spec);  loSplitLP.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    loSplitHP.prepare(spec);  loSplitHP.setType(juce::dsp::LinkwitzRileyFilterType::highpass);
    hiSplitLP.prepare(spec);  hiSplitLP.setType(juce::dsp::LinkwitzRileyFilterType::lowpass);
    hiSplitHP.prepare(spec);  hiSplitHP.setType(juce::dsp::LinkwitzRileyFilterType::highpass);

    // Prepare per-band compressors (stereo)
    for (auto& c : compressors)
        c.prepare(spec);

    // Pre-allocate band buffers (no allocation in processBlock)
    for (auto& buf : bandBuffers)
        buf.setSize(2, (int)spec.maximumBlockSize);

    for (auto& gr : gainReductionDb)
        gr.store(0.f, std::memory_order_relaxed);

    prevCrossoverLo = -1.f;  // Force initial update
    updateCrossovers(apvts);
    updateCompressorParams(apvts);
}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void CompressorProcessor::updateCrossovers(juce::AudioProcessorValueTreeState& apvts)
{
    float lo = apvts.getRawParameterValue("comp_crossover_lo")->load();
    float hi = apvts.getRawParameterValue("comp_crossover_hi")->load();

    if (lo != prevCrossoverLo) {
        loSplitLP.setCutoffFrequency(lo);
        loSplitHP.setCutoffFrequency(lo);
        prevCrossoverLo = lo;
    }
    if (hi != prevCrossoverHi) {
        hiSplitLP.setCutoffFrequency(hi);
        hiSplitHP.setCutoffFrequency(hi);
        prevCrossoverHi = hi;
    }
}

void CompressorProcessor::updateCompressorParams(juce::AudioProcessorValueTreeState& apvts)
{
    const char* bands[] = { "lo", "mid", "hi" };
    for (int i = 0; i < NUM_BANDS; ++i)
    {
        juce::String b(bands[i]);
        float thresh  = apvts.getRawParameterValue("comp_" + b + "_thresh") ->load();
        float ratio   = apvts.getRawParameterValue("comp_" + b + "_ratio")  ->load();
        float attack  = apvts.getRawParameterValue("comp_" + b + "_attack") ->load();
        float release = apvts.getRawParameterValue("comp_" + b + "_release")->load();

        compressors[i].setThreshold(thresh);
        compressors[i].setRatio(ratio);
        compressors[i].setAttack(attack);
        compressors[i].setRelease(release);
    }
}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void CompressorProcessor::process(juce::dsp::ProcessContextReplacing<float>& context,
                                    juce::AudioProcessorValueTreeState& apvts)
{
    updateCrossovers(apvts);
    updateCompressorParams(apvts);

    auto& block    = context.getOutputBlock();
    int   numSamps = (int)block.getNumSamples();

    // Copy main block into each band buffer for in-place processing
    for (int b = 0; b < NUM_BANDS; ++b)
    {
        bandBuffers[b].setSize(2, numSamps, false, false, true);
        for (int ch = 0; ch < 2; ++ch)
            bandBuffers[b].copyFrom(ch, 0, block.getChannelPointer(ch), numSamps);
    }

    // â”€â”€ Band 0: Low (loSplitLP) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    {
        juce::dsp::AudioBlock<float> loBlock(bandBuffers[0]);
        juce::dsp::ProcessContextReplacing<float> loCtx(loBlock);
        loSplitLP.process(loCtx);
    }

    // â”€â”€ Band 2: High (hiSplitHP) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    {
        juce::dsp::AudioBlock<float> hiBlock(bandBuffers[2]);
        juce::dsp::ProcessContextReplacing<float> hiCtx(hiBlock);
        hiSplitHP.process(hiCtx);
    }

    // â”€â”€ Band 1: Mid (loSplitHP â†’ hiSplitLP) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    {
        juce::dsp::AudioBlock<float> midBlock(bandBuffers[1]);
        juce::dsp::ProcessContextReplacing<float> midCtx(midBlock);
        loSplitHP.process(midCtx);
        hiSplitLP.process(midCtx);
    }

    // â”€â”€ Apply per-band compression â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    for (int b = 0; b < NUM_BANDS; ++b)
    {
        // Apply makeup gain before compressor
        const char* bands[] = { "lo", "mid", "hi" };
        float makeupDb = apvts.getRawParameterValue(juce::String("comp_") + bands[b] + "_gain")->load();
        bandBuffers[b].applyGain(juce::Decibels::decibelsToGain(makeupDb));

        juce::dsp::AudioBlock<float> bandBlock(bandBuffers[b]);
        juce::dsp::ProcessContextReplacing<float> bandCtx(bandBlock);
        compressors[b].process(bandCtx);

        // Approximate GR: compare input RMS vs output RMS
        float inputPow  = 0.f, outputPow = 0.f;
        auto* raw = bandBuffers[b].getReadPointer(0);
        for (int n = 0; n < numSamps; ++n) outputPow += raw[n] * raw[n];
        gainReductionDb[b].store(juce::Decibels::gainToDecibels(
            outputPow > 0.f ? std::sqrt(outputPow / numSamps) : 1.f),
            std::memory_order_relaxed);
    }

    // â”€â”€ Sum bands back into main block â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    for (int ch = 0; ch < 2; ++ch)
    {
        auto* out  = block.getChannelPointer(ch);
        auto* lo   = bandBuffers[0].getReadPointer(ch);
        auto* mid  = bandBuffers[1].getReadPointer(ch);
        auto* hi   = bandBuffers[2].getReadPointer(ch);
        for (int n = 0; n < numSamps; ++n)
            out[n] = lo[n] + mid[n] + hi[n];
    }
}

