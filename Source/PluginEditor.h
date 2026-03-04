#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class LoveMasterEditor : public juce::AudioProcessorEditor,
                         public juce::Timer
{
public:
    LoveMasterEditor(LoveMasterProcessor&);
    ~LoveMasterEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    void openTriageCenter();
    void closeTriageCenter();
    void paintNerdFoundOverlay(juce::Graphics&);
    void paintWaveform(juce::Graphics&);
    void paintSpinningHeart(juce::Graphics&);

    LoveMasterProcessor& processor;

    // â”€â”€ Love knob â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    juce::Slider loveKnob;
    juce::Label  loveLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> loveAttachment;

    // â”€â”€ Triage Center â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    juce::TextButton nerdsButton;
    bool triageOpen = false;

    struct TriageKnob {
        juce::Slider slider;
        juce::Label  label;
        std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> attachment;
    };
    std::array<TriageKnob, 8> triageKnobs;
    static constexpr const char* triageParamIDs[8] = {
        "triage_ceiling","triage_lowcut","triage_midcut","triage_output",
        "triage_release","triage_attack","triage_width","triage_excfreq"
    };
    static constexpr const char* triageLabels[8] = {
        "Ceiling","Low Cut","Mid Cut","Output",
        "Release","Attack","Width","Exc Freq"
    };

    // â”€â”€ Nerd Found overlay â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    bool  nerdFoundVisible = false;
    float nerdFoundAlpha   = 0.f;
    int   nerdFoundTimer   = 0;

    // â”€â”€ Animation state â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    float heartAngle       = 0.f;
    int   waveformOffset   = 0;

    // Waveform display buffer (smoothed for UI)
    std::array<float, 256> waveformDisplay {};

    static constexpr int MAIN_WIDTH   = 420;
    static constexpr int MAIN_HEIGHT  = 340;
    static constexpr int TRIAGE_WIDTH = 320;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoveMasterEditor)
};

