#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class EQModule;
class CompressorModule;
class LimiterModule;
class StereoModule;
class ExciterModule;
class MeterModule;
class MainToolbar;
class PresetBar;
class IzoCloneLookAndFeel;

class IzoCloneEditor : public juce::AudioProcessorEditor,
                           private juce::Timer
{
public:
    explicit IzoCloneEditor(IzoCloneProcessor&);
    ~IzoCloneEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void showModule(int index);
    void handleABButton();

    enum class ABState { Idle, SavedA, ComparingA, ComparingB };
    ABState abState = ABState::Idle;

    IzoCloneProcessor& processorRef;

    std::unique_ptr<IzoCloneLookAndFeel> lookAndFeel;
    std::unique_ptr<MainToolbar>            toolbar;
    std::unique_ptr<PresetBar>              presetBar;
    std::unique_ptr<EQModule>               eqModule;
    std::unique_ptr<CompressorModule>       compModule;
    std::unique_ptr<StereoModule>           stereoModule;
    std::unique_ptr<ExciterModule>          exciterModule;
    std::unique_ptr<LimiterModule>          limiterModule;
    std::unique_ptr<MeterModule>            meterModule;

    int activeModuleIndex = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(IzoCloneEditor)
};

