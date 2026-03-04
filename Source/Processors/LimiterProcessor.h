#pragma once
#include <JuceHeader.h>

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  LimiterProcessor
//  True-peak lookahead brick-wall limiter
//  Algorithm: Lookahead gain computer â†’ smooth gain reduction â†’ output
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
class LimiterProcessor
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec,
                 juce::AudioProcessorValueTreeState& apvts);

    void process(juce::dsp::ProcessContextReplacing<float>& context,
                 juce::AudioProcessorValueTreeState& apvts);

    // For GR meter display (UI thread reads this)
    float getGainReductionDb() const { return gainReductionDb.load(std::memory_order_relaxed); }

private:
    static constexpr int MAX_LOOKAHEAD_SAMPLES = 2048;  // ~46ms @ 44.1k

    // Delay line for lookahead
    juce::AudioBuffer<float> delayBuffer;
    int delayWritePos   = 0;
    int lookaheadSamples = 0;

    // Gain smoothing
    float currentGain    = 1.0f;
    float releaseCoeff   = 0.0f;
    double sampleRate    = 44100.0;

    std::atomic<float> gainReductionDb { 0.0f };

    void updateParams(juce::AudioProcessorValueTreeState& apvts);
    float prevCeiling   = 999.f;
    float prevLookahead = 999.f;
    float prevRelease   = 999.f;
    float ceiling       = 1.0f;
};

