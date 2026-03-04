#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"

class MainToolbar : public juce::Component
{
public:
    using ModuleSelectCB = std::function<void(int)>;
    using ABClickCB      = std::function<void()>;
    using AutoGainCB     = std::function<void(bool)>;

    MainToolbar(juce::AudioProcessorValueTreeState& apvts,
                ModuleSelectCB onSelect,
                ABClickCB      onAB,
                AutoGainCB     onAutoGain)
        : apvts(apvts),
          onModuleSelect(std::move(onSelect)),
          onABClick(std::move(onAB)),
          onAutoGainToggle(std::move(onAutoGain))
    {
        // Module tabs
        const char* labels[] = { "EQ", "COMP", "STEREO", "EXCITER", "LIMITER", "METERS" };
        for (int i = 0; i < 6; ++i)
        {
            auto& btn = moduleTabs.emplace_back(std::make_unique<juce::TextButton>(labels[i]));
            btn->setClickingTogglesState(false);
            btn->onClick = [this, i] { activeTab = i; highlightTab(i); onModuleSelect(i); };
            addAndMakeVisible(*btn);
        }
        highlightTab(0);

        // Output gain
        outputGainSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        outputGainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 14);
        outputGainSlider.setDoubleClickReturnValue(true, 0.0);
        addAndMakeVisible(outputGainSlider);
        outputGainAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, "output_gain", outputGainSlider);

        outputLabel.setText("OUTPUT", juce::dontSendNotification);
        outputLabel.setJustificationType(juce::Justification::centred);
        outputLabel.setFont(IzoCloneLookAndFeel::getLabelFont());
        outputLabel.setColour(juce::Label::textColourId, juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        addAndMakeVisible(outputLabel);

        // LUFS target
        targetLUFSCombo.addItemList({"-6 LUFS","-8 LUFS","-10 LUFS","-14 LUFS",
                                      "-16 LUFS","-18 LUFS","-23 LUFS","Off"}, 1);
        targetLUFSCombo.setSelectedItemIndex(3, juce::dontSendNotification);
        addAndMakeVisible(targetLUFSCombo);
        targetLUFSAttach = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
            apvts, "target_lufs", targetLUFSCombo);

        lufsLabel.setText("TARGET", juce::dontSendNotification);
        lufsLabel.setJustificationType(juce::Justification::centred);
        lufsLabel.setFont(IzoCloneLookAndFeel::getLabelFont());
        lufsLabel.setColour(juce::Label::textColourId, juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        addAndMakeVisible(lufsLabel);

        // Auto-gain toggle
        autoGainBtn.setButtonText("AUTO");
        autoGainBtn.setClickingTogglesState(true);
        autoGainBtn.onClick = [this] { onAutoGainToggle(autoGainBtn.getToggleState()); };
        addAndMakeVisible(autoGainBtn);

        // A/B button
        abButton.setButtonText("A/B");
        abButton.onClick = [this] { onABClick(); };
        addAndMakeVisible(abButton);

        // LUFS readout label (always visible in toolbar)
        lufsReadout.setText("--- LUFS", juce::dontSendNotification);
        lufsReadout.setJustificationType(juce::Justification::centredRight);
        lufsReadout.setFont(juce::Font(juce::FontOptions("Helvetica Neue", 11.f, juce::Font::bold)));
        lufsReadout.setColour(juce::Label::textColourId, juce::Colour(IzoCloneLookAndFeel::COL_ACCENT));
        addAndMakeVisible(lufsReadout);

        truePeakReadout.setText("--- dBTP", juce::dontSendNotification);
        truePeakReadout.setJustificationType(juce::Justification::centredRight);
        truePeakReadout.setFont(juce::Font(juce::FontOptions("Helvetica Neue", 10.f, juce::Font::plain)));
        truePeakReadout.setColour(juce::Label::textColourId, juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        addAndMakeVisible(truePeakReadout);
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_BG_PANEL));
        g.fillAll();
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_BG_CONTROL));
        g.drawLine(0, (float)getHeight() - 1, (float)getWidth(), (float)getHeight() - 1);

        // Logo
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_ACCENT));
        g.setFont(juce::Font(juce::FontOptions("Helvetica Neue", 17.f, juce::Font::bold)));
        g.drawText("MASTERFORGE", 14, 4, 155, 26, juce::Justification::centredLeft);
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        g.setFont(juce::Font(juce::FontOptions("Helvetica Neue", 9.f, juce::Font::plain)));
        g.drawText("MASTERING SUITE  v1.0", 14, 28, 155, 14, juce::Justification::centredLeft);
    }

    void resized() override
    {
        int tabW = 76, tabH = 26, tabY = (getHeight() - tabH) / 2;
        int startX = 178;
        for (int i = 0; i < (int)moduleTabs.size(); ++i)
            moduleTabs[i]->setBounds(startX + i * (tabW + 3), tabY, tabW, tabH);

        int rx = getWidth() - 8;

        abButton.setBounds(rx - 52, tabY, 52, tabH);      rx -= 60;
        autoGainBtn.setBounds(rx - 52, tabY, 52, tabH);   rx -= 62;

        truePeakReadout.setBounds(rx - 80, tabY + 14, 80, 13); 
        lufsReadout.setBounds(rx - 80, tabY, 80, 16);          rx -= 90;

        outputLabel.setBounds(rx - 46, tabY - 2, 46, 12);
        outputGainSlider.setBounds(rx - 46, tabY + 8, 46, getHeight() - tabY - 10);
        rx -= 56;

        lufsLabel.setBounds(rx - 88, tabY - 2, 88, 12);
        targetLUFSCombo.setBounds(rx - 88, tabY + 10, 88, tabH - 4);
    }

    // Called by editor to update live LUFS display in toolbar
    void updateLUFSReadout(float integratedLUFS, float truePeakDb)
    {
        juce::String lufsStr = (integratedLUFS <= -69.f) ? "--- LUFS"
            : juce::String(integratedLUFS, 1) + " LUFS";
        lufsReadout.setText(lufsStr, juce::dontSendNotification);

        juce::String tpStr = (truePeakDb <= -69.f) ? "--- dBTP"
            : juce::String(truePeakDb, 1) + " dBTP";
        truePeakReadout.setText(tpStr, juce::dontSendNotification);

        // Colour-code true peak: red if near 0
        truePeakReadout.setColour(juce::Label::textColourId,
            truePeakDb > -1.f
                ? juce::Colour(IzoCloneLookAndFeel::COL_METER_GR)
                : juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
    }

    // Called by editor A/B handler to update button label
    void setABLabel(const juce::String& label)
    {
        abButton.setButtonText(label);
        abButton.setColour(juce::TextButton::buttonColourId,
            label == "A/B" ? juce::Colour(IzoCloneLookAndFeel::COL_BG_CONTROL)
                           : juce::Colour(IzoCloneLookAndFeel::COL_ACCENT).withAlpha(0.25f));
    }

private:
    void highlightTab(int idx)
    {
        for (int i = 0; i < (int)moduleTabs.size(); ++i)
        {
            bool active = (i == idx);
            moduleTabs[i]->setColour(juce::TextButton::buttonColourId,
                active ? juce::Colour(IzoCloneLookAndFeel::COL_ACCENT).withAlpha(0.18f)
                       : juce::Colour(IzoCloneLookAndFeel::COL_BG_CONTROL));
            moduleTabs[i]->setColour(juce::TextButton::textColourOffId,
                active ? juce::Colour(IzoCloneLookAndFeel::COL_ACCENT)
                       : juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        }
    }

    juce::AudioProcessorValueTreeState& apvts;
    ModuleSelectCB onModuleSelect;
    ABClickCB      onABClick;
    AutoGainCB     onAutoGainToggle;
    int            activeTab = 0;

    std::vector<std::unique_ptr<juce::TextButton>> moduleTabs;

    juce::Slider outputGainSlider;
    juce::Label  outputLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> outputGainAttach;

    juce::ComboBox targetLUFSCombo;
    juce::Label    lufsLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> targetLUFSAttach;

    juce::TextButton autoGainBtn, abButton;
    juce::Label      lufsReadout, truePeakReadout;
};

