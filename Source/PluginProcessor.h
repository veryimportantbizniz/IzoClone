#pragma once
#include <JuceHeader.h>
#include "Processors/EQProcessor.h"
#include "Processors/AllProcessors.h"
#include "Processors/LimiterProcessor.h"

class LoveMasterEditor;

class LoveMasterProcessor : public juce::AudioProcessor,
                             public juce::AudioProcessorValueTreeState::Listener
{
public:
    LoveMasterProcessor();
    ~LoveMasterProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "LoveMaster"; }
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
    void parameterChanged(const juce::String& parameterID, float newValue) override;

    juce::AudioProcessorValueTreeState apvts;
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // â”€â”€ Public accessors for UI â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    LoudnessMeter&         getLoudnessMeter()  { return loudnessMeter; }
    FFTAnalyzer&           getFFTAnalyzer()    { return fftAnalyzer; }
    LimiterProcessor&      getLimiterProcessor(){ return limiterProcessor; }

    // Waveform data for UI (ring buffer, UI polls this)
    float getWaveformSample(int index) const { return waveformRing[index & (WAVEFORM_SIZE-1)]; }
    static constexpr int WAVEFORM_SIZE = 512;

private:
    void applyLoveMacro(float love);
    void applyAutoMudRemoval(juce::AudioBuffer<float>& buffer);

    EQProcessor           eqProcessor;
    CompressorProcessor   compProcessor;
    StereoImagerProcessor stereoProcessor;
    ExciterProcessor      exciterProcessor;
    LimiterProcessor      limiterProcessor;
    LoudnessMeter         loudnessMeter;
    FFTAnalyzer           fftAnalyzer;

    // Mud detection â€” simple one-pole IIR bandpass energy tracker
    float mudEnergy     = 0.f;
    float mudGainSmooth = 1.f;
    juce::dsp::StateVariableTPTFilter<float> mudBandpass;
    bool mudFilterReady = false;

    // Waveform ring buffer (written on audio thread, read on UI thread)
    std::array<std::atomic<float>, WAVEFORM_SIZE> waveformRing;
    std::atomic<int> waveformWritePos { 0 };

    double currentSampleRate = 44100.0;
    int    currentBlockSize  = 512;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoveMasterProcessor)
};

