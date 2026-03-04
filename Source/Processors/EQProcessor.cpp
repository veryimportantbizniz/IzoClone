#include "EQProcessor.h"

void EQProcessor::prepare(const juce::dsp::ProcessSpec& spec,
                            juce::AudioProcessorValueTreeState& apvts)
{
    sampleRate = spec.sampleRate;

    // Prepare each channel's filter chain
    auto monoSpec = spec;
    monoSpec.numChannels = 1;
    filterChainL.prepare(monoSpec);
    filterChainR.prepare(monoSpec);

    // Force initial coefficient update
    prevLowShelfFreq = -1.f;
    updateFilters(apvts);
}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void EQProcessor::updateFilters(juce::AudioProcessorValueTreeState& apvts)
{
    auto lsFreq  = apvts.getRawParameterValue("eq_lowshelf_freq") ->load();
    auto lsGain  = apvts.getRawParameterValue("eq_lowshelf_gain") ->load();
    auto lmFreq  = apvts.getRawParameterValue("eq_low_mid_freq")  ->load();
    auto lmGain  = apvts.getRawParameterValue("eq_low_mid_gain")  ->load();
    auto lmQ     = apvts.getRawParameterValue("eq_low_mid_q")     ->load();
    auto hmFreq  = apvts.getRawParameterValue("eq_high_mid_freq") ->load();
    auto hmGain  = apvts.getRawParameterValue("eq_high_mid_gain") ->load();
    auto hmQ     = apvts.getRawParameterValue("eq_high_mid_q")    ->load();
    auto hsFreq  = apvts.getRawParameterValue("eq_highshelf_freq")->load();
    auto hsGain  = apvts.getRawParameterValue("eq_highshelf_gain")->load();

    // Low shelf
    auto lsCoeffs = juce::dsp::IIR::Coefficients<float>::makeLowShelf(
        sampleRate, lsFreq, 0.707f, juce::Decibels::decibelsToGain(lsGain));
    *filterChainL.get<0>().coefficients = *lsCoeffs;
    *filterChainR.get<0>().coefficients = *lsCoeffs;

    // Low mid bell
    auto lmCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, lmFreq, lmQ, juce::Decibels::decibelsToGain(lmGain));
    *filterChainL.get<1>().coefficients = *lmCoeffs;
    *filterChainR.get<1>().coefficients = *lmCoeffs;

    // High mid bell
    auto hmCoeffs = juce::dsp::IIR::Coefficients<float>::makePeakFilter(
        sampleRate, hmFreq, hmQ, juce::Decibels::decibelsToGain(hmGain));
    *filterChainL.get<2>().coefficients = *hmCoeffs;
    *filterChainR.get<2>().coefficients = *hmCoeffs;

    // High shelf
    auto hsCoeffs = juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sampleRate, hsFreq, 0.707f, juce::Decibels::decibelsToGain(hsGain));
    *filterChainL.get<3>().coefficients = *hsCoeffs;
    *filterChainR.get<3>().coefficients = *hsCoeffs;

    // Update shadow values
    prevLowShelfFreq  = lsFreq;  prevLowShelfGain  = lsGain;
    prevLowMidFreq    = lmFreq;  prevLowMidGain    = lmGain;   prevLowMidQ  = lmQ;
    prevHighMidFreq   = hmFreq;  prevHighMidGain   = hmGain;   prevHighMidQ = hmQ;
    prevHighShelfFreq = hsFreq;  prevHighShelfGain = hsGain;
}

bool EQProcessor::filtersNeedUpdate(juce::AudioProcessorValueTreeState& apvts)
{
    // Quick check without touching every param â€” only verify one sentinel
    return apvts.getRawParameterValue("eq_lowshelf_freq")->load() != prevLowShelfFreq
        || apvts.getRawParameterValue("eq_lowshelf_gain")->load() != prevLowShelfGain
        || apvts.getRawParameterValue("eq_low_mid_freq") ->load() != prevLowMidFreq
        || apvts.getRawParameterValue("eq_low_mid_gain") ->load() != prevLowMidGain
        || apvts.getRawParameterValue("eq_high_mid_freq")->load() != prevHighMidFreq
        || apvts.getRawParameterValue("eq_highshelf_freq")->load()!= prevHighShelfFreq;
}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void EQProcessor::process(juce::dsp::ProcessContextReplacing<float>& context,
                            juce::AudioProcessorValueTreeState& apvts)
{
    // Update coefficients only when params have changed (cheap dirty-flag check)
    if (filtersNeedUpdate(apvts))
        updateFilters(apvts);

    // Process L and R independently through their own filter state
    auto& block = context.getOutputBlock();

    auto leftBlock  = block.getSingleChannelBlock(0);
    auto rightBlock = block.getSingleChannelBlock(1);

    juce::dsp::ProcessContextReplacing<float> leftCtx(leftBlock);
    juce::dsp::ProcessContextReplacing<float> rightCtx(rightBlock);

    filterChainL.process(leftCtx);
    filterChainR.process(rightCtx);
}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  getMagnitudeResponse â€” used by UI to draw the EQ frequency curve
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void EQProcessor::getMagnitudeResponse(const std::vector<double>& freqs,
                                         std::vector<double>& output) const
{
    jassert(freqs.size() == output.size());
    std::fill(output.begin(), output.end(), 1.0);

    // Multiply magnitude response of each filter stage together
    auto applyStage = [&](const auto& filter) {
        for (size_t i = 0; i < freqs.size(); ++i)
            output[i] *= filter.coefficients->getMagnitudeForFrequency(freqs[i], sampleRate);
    };

    applyStage(filterChainL.get<0>());
    applyStage(filterChainL.get<1>());
    applyStage(filterChainL.get<2>());
    applyStage(filterChainL.get<3>());

    // Convert to dB
    for (auto& v : output)
        v = juce::Decibels::gainToDecibels(v);
}

