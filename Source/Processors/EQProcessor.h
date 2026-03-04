#pragma once
#include <JuceHeader.h>

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  EQProcessor
//  5-band parametric EQ using JUCE's IIR filter cascade
//  Bands: Low Shelf | Low Mid Bell | High Mid Bell | High Shelf | (HPF optional)
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
class EQProcessor
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec,
                 juce::AudioProcessorValueTreeState& apvts);

    void process(juce::dsp::ProcessContextReplacing<float>& context,
                 juce::AudioProcessorValueTreeState& apvts);

    // Called by UI to get current magnitude response for the EQ curve display
    // Returns magnitudes in dB for frequencies in ``freqs`` (size must match ``output``)
    void getMagnitudeResponse(const std::vector<double>& freqs,
                               std::vector<double>& output) const;

private:
    void updateFilters(juce::AudioProcessorValueTreeState& apvts);
    bool filtersNeedUpdate(juce::AudioProcessorValueTreeState& apvts);

    using FilterChain = juce::dsp::ProcessorChain<
        juce::dsp::IIR::Filter<float>,   // Low shelf
        juce::dsp::IIR::Filter<float>,   // Low mid bell
        juce::dsp::IIR::Filter<float>,   // High mid bell
        juce::dsp::IIR::Filter<float>    // High shelf
    >;

    FilterChain filterChainL, filterChainR;
    double sampleRate = 44100.0;

    // Shadow values to detect changes without touching apvts in processBlock
    float prevLowShelfFreq  = -1.f, prevLowShelfGain  = -999.f;
    float prevLowMidFreq    = -1.f, prevLowMidGain    = -999.f, prevLowMidQ  = -1.f;
    float prevHighMidFreq   = -1.f, prevHighMidGain   = -999.f, prevHighMidQ = -1.f;
    float prevHighShelfFreq = -1.f, prevHighShelfGain = -999.f;
};

