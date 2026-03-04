#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"

class LimiterModule : public juce::Component
{
public:
    LimiterModule(juce::AudioProcessorValueTreeState& apvts) : apvts(apvts)
    {
        bypassBtn.setButtonText("ACTIVE");
        bypassBtn.setClickingTogglesState(true);
        bypassBtn.setToggleState(true, juce::dontSendNotification);
        addAndMakeVisible(bypassBtn);
        bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, "limiter_bypass", bypassBtn);

        addKnob(ceilingKnob,   ceilingLabel,   "limiter_ceiling",   "CEILING");
        addKnob(lookaheadKnob, lookaheadLabel, "limiter_lookahead", "LOOKAHEAD");
        addKnob(releaseKnob,   releaseLabel,   "limiter_release",   "RELEASE");

        // Ceiling readout label
        ceilingReadout.setText("-1.0 dBTP", juce::dontSendNotification);
        ceilingReadout.setJustificationType(juce::Justification::centred);
        ceilingReadout.setFont(juce::Font(juce::FontOptions("Helvetica Neue", 22.f, juce::Font::bold)));
        ceilingReadout.setColour(juce::Label::textColourId,
                                  juce::Colour(IzoCloneLookAndFeel::COL_ACCENT));
        addAndMakeVisible(ceilingReadout);

        ceilingKnob.onValueChange = [this] {
            ceilingReadout.setText(juce::String(ceilingKnob.getValue(), 1) + " dBTP",
                                    juce::dontSendNotification);
        };
    }

    void paint(juce::Graphics& g) override
    {
        IzoCloneLookAndFeel::drawModuleBackground(g, getLocalBounds(), "LIMITER");

        // GR meter (large, centered)
        auto meterArea = getGRMeterArea();
        drawGRMeter(g, meterArea, currentGR);

        // "GAIN REDUCTION" label
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        g.setFont(IzoCloneLookAndFeel::getLabelFont());
        g.drawText("GAIN REDUCTION", meterArea.getX(), meterArea.getBottom() + 4,
                    meterArea.getWidth(), 14, juce::Justification::centred);
    }

    void resized() override
    {
        bypassBtn.setBounds(getWidth() - 72, 8, 60, 22);

        auto bounds = getLocalBounds().reduced(40);
        bounds.removeFromTop(50);

        // GR meter on the right
        auto meterSide = bounds.removeFromRight(60);
        // (drawn in paint)

        // Ceiling readout at top
        ceilingReadout.setBounds(bounds.removeFromTop(50));
        bounds.removeFromTop(20);

        // 3 knobs in a row
        int kw = bounds.getWidth() / 3;
        placeKnob(ceilingKnob,   ceilingLabel,   bounds.removeFromLeft(kw).reduced(8, 0));
        placeKnob(lookaheadKnob, lookaheadLabel, bounds.removeFromLeft(kw).reduced(8, 0));
        placeKnob(releaseKnob,   releaseLabel,   bounds.removeFromLeft(kw).reduced(8, 0));
    }

    void refreshGRMeter()
    {
        // currentGR is set by PluginEditor from LimiterProcessor::getGainReductionDb()
        repaint();
    }

    float currentGR = 0.f;  // in dB, set by editor

private:
    juce::Rectangle<int> getGRMeterArea() const
    {
        return getLocalBounds().reduced(40).removeFromRight(50)
                               .withTrimmedTop(50).withTrimmedBottom(30);
    }

    void drawGRMeter(juce::Graphics& g, juce::Rectangle<int> area, float grDb)
    {
        // Background
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_BG_CONTROL));
        g.fillRoundedRectangle(area.toFloat(), 6.f);

        float maxGR = 24.f;
        float norm  = juce::jlimit(0.f, 1.f, std::abs(grDb) / maxGR);
        int   fillH = (int)(area.getHeight() * norm);

        if (fillH > 0) {
            // Colour gradient: green â†’ amber â†’ red based on amount
            juce::Colour grColour = (norm < 0.3f)
                ? juce::Colour(IzoCloneLookAndFeel::COL_BYPASS_ON)
                : (norm < 0.7f)
                ? juce::Colour(IzoCloneLookAndFeel::COL_ACCENT)
                : juce::Colour(IzoCloneLookAndFeel::COL_METER_GR);

            auto fill = area.removeFromBottom(fillH);
            g.setColour(grColour);
            g.fillRoundedRectangle(fill.toFloat(), 6.f);
        }

        // dB value text
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_TEXT));
        g.setFont(juce::Font(juce::FontOptions("Helvetica Neue", 11.f, juce::Font::bold)));
        g.drawText(juce::String(grDb, 1) + " dB", area.getX() - 10, area.getY() - 20,
                    area.getWidth() + 20, 18, juce::Justification::centred);
    }

    void addKnob(juce::Slider& s, juce::Label& l, const juce::String& id, const char* text)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
        s.setDoubleClickReturnValue(true, (double)apvts.getParameter(id)->getDefaultValue());
        addAndMakeVisible(s);
        attachments.push_back(
            std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(apvts, id, s));

        l.setText(text, juce::dontSendNotification);
        l.setJustificationType(juce::Justification::centred);
        l.setFont(IzoCloneLookAndFeel::getLabelFont());
        l.setColour(juce::Label::textColourId, juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        addAndMakeVisible(l);
    }

    void placeKnob(juce::Slider& s, juce::Label& l, juce::Rectangle<int> area)
    {
        l.setBounds(area.removeFromTop(14));
        s.setBounds(area);
    }

    juce::AudioProcessorValueTreeState& apvts;

    juce::ToggleButton bypassBtn;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;

    juce::Slider ceilingKnob, lookaheadKnob, releaseKnob;
    juce::Label  ceilingLabel, lookaheadLabel, releaseLabel;
    juce::Label  ceilingReadout;

    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
};

