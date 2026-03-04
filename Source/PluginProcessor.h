#pragma once
#include <JuceHeader.h>
#include "Processors/EQProcessor.h"
#include "Processors/AllProcessors.h"
#include "Utils/PresetManager.h"

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  IzoCloneProcessor
//  Signal chain (in order):
//    Input â†’ EQ â†’ Multiband Compressor â†’ Stereo Imager â†’ Exciter â†’ Limiter â†’ Output
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
class IzoCloneProcessor : public juce::AudioProcessor,
                              public juce::AudioProcessorValueTreeState::Listener
{
public:
    IzoCloneProcessor();
    ~IzoCloneProcessor() override;

    // â”€â”€ AudioProcessor â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "IzoClone"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return "Default"; }
    void changeProgramName(int, const juce::String&) override {}

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    // â”€â”€ Parameter listener â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    // â”€â”€ Parameter Tree â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // â”€â”€ Public DSP access (for UI metering) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    LoudnessMeter&      getLoudnessMeter()   { return loudnessMeter; }
    FFTAnalyzer&        getFFTAnalyzer()     { return fftAnalyzer; }
    EQProcessor&        getEQProcessor()     { return eqProcessor; }
    CompressorProcessor& getCompProcessor()  { return compProcessor; }
    LimiterProcessor&   getLimiterProcessor(){ return limiterProcessor; }
    StereoImagerProcessor& getStereoProcessor() { return stereoProcessor; }

    PresetManager& getPresetManager() { return presetManager; }

    // â”€â”€ A/B Comparison â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void snapshotToSlot(int slot);   // slot 0 = A, slot 1 = B
    void recallSlot(int slot);
    bool hasSlot(int slot) const { return !abSlots[slot].isEmpty(); }

    // â”€â”€ LUFS Auto-gain â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void setAutoGainEnabled(bool enabled) { autoGainEnabled.store(enabled, std::memory_order_relaxed); }
    bool isAutoGainEnabled() const        { return autoGainEnabled.load(std::memory_order_relaxed); }

    // â”€â”€ Module bypass states (thread-safe atomic) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    std::atomic<bool> eqBypassed      { false };
    std::atomic<bool> compBypassed    { false };
    std::atomic<bool> stereoBypassed  { false };
    std::atomic<bool> exciterBypassed { false };
    std::atomic<bool> limiterBypassed { false };

private:
    // â”€â”€ Processing chain â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    EQProcessor           eqProcessor;
    CompressorProcessor   compProcessor;
    StereoImagerProcessor stereoProcessor;
    ExciterProcessor      exciterProcessor;
    LimiterProcessor      limiterProcessor;
    LoudnessMeter         loudnessMeter;
    FFTAnalyzer           fftAnalyzer;

    double currentSampleRate = 44100.0;
    int    currentBlockSize  = 512;

    PresetManager presetManager { apvts };

    // A/B state slots (serialised APVTS XML)
    juce::MemoryBlock abSlots[2];

    // LUFS auto-gain
    std::atomic<bool> autoGainEnabled { false };
    float             autoGainSmoothDb = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IzoCloneProcessor)
};

