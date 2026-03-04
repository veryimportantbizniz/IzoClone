#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"

class StereoModule : public juce::Component
{
public:
    StereoModule(juce::AudioProcessorValueTreeState& apvts) : apvts(apvts)
    {
        bypassBtn.setButtonText("ACTIVE");
        bypassBtn.setClickingTogglesState(true);
        bypassBtn.setToggleState(true, juce::dontSendNotification);
        addAndMakeVisible(bypassBtn);
        bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, "stereo_bypass", bypassBtn);

        widthSlider.setSliderStyle(juce::Slider::LinearVertical);
        widthSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
        widthSlider.setDoubleClickReturnValue(true, 100.0);
        addAndMakeVisible(widthSlider);
        widthAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, "stereo_width", widthSlider);

        widthLabel.setText("WIDTH", juce::dontSendNotification);
        widthLabel.setJustificationType(juce::Justification::centred);
        widthLabel.setFont(IzoCloneLookAndFeel::getLabelFont());
        widthLabel.setColour(juce::Label::textColourId,
                              juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        addAndMakeVisible(widthLabel);

        monoLoKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        monoLoKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
        monoLoKnob.setDoubleClickReturnValue(true, 80.0);
        addAndMakeVisible(monoLoKnob);
        monoLoAttach = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            apvts, "stereo_mono_lo", monoLoKnob);

        monoLoLabel.setText("MONO BELOW", juce::dontSendNotification);
        monoLoLabel.setJustificationType(juce::Justification::centred);
        monoLoLabel.setFont(IzoCloneLookAndFeel::getLabelFont());
        monoLoLabel.setColour(juce::Label::textColourId,
                               juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        addAndMakeVisible(monoLoLabel);
    }

    void paint(juce::Graphics& g) override
    {
        IzoCloneLookAndFeel::drawModuleBackground(g, getLocalBounds(), "STEREO IMAGER");

        // Width percentage display
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_ACCENT));
        g.setFont(juce::Font(juce::FontOptions("Helvetica Neue", 48.f, juce::Font::bold)));
        float width = (float)widthSlider.getValue();
        g.drawText(juce::String((int)width) + "%",
                    getLocalBounds().withHeight(getHeight() / 2),
                    juce::Justification::centred);

        // Stereo field visualiser (simple circle with L/R width indicator)
        drawStereoField(g);

        // Correlation meter
        drawCorrelationMeter(g);
    }

    void resized() override
    {
        bypassBtn.setBounds(getWidth() - 72, 8, 60, 22);

        int cx = getWidth() / 2;
        widthLabel.setBounds(cx - 40, getHeight() - 120, 80, 14);
        widthSlider.setBounds(cx - 30, getHeight() - 106, 60, 80);

        monoLoLabel.setBounds(cx + 80, getHeight() - 120, 80, 14);
        monoLoKnob.setBounds(cx + 70, getHeight() - 106, 80, 80);
    }

    float correlationValue = 1.f;  // set by editor

private:
    void drawStereoField(juce::Graphics& g)
    {
        auto centre = juce::Point<float>(getWidth() * 0.5f, getHeight() * 0.42f);
        float radius = 80.f;

        // Outer ring
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_BG_CONTROL));
        g.drawEllipse(centre.x - radius, centre.y - radius, radius * 2, radius * 2, 1.5f);

        // Width arc
        float width = (float)widthSlider.getValue() / 100.f;
        float arcAngle = juce::MathConstants<float>::pi * 0.5f * juce::jmin(width, 2.0f);

        juce::Path arc;
        arc.addCentredArc(centre.x, centre.y, radius * 0.7f, radius * 0.7f,
                           0, -arcAngle, arcAngle, true);
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_ACCENT).withAlpha(0.7f));
        g.strokePath(arc, juce::PathStrokeType(3.f, juce::PathStrokeType::curved,
                                                 juce::PathStrokeType::rounded));

        // L / R labels
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        g.setFont(IzoCloneLookAndFeel::getLabelFont());
        g.drawText("L", (int)(centre.x - radius - 14), (int)(centre.y - 8), 14, 16,
                    juce::Justification::centred);
        g.drawText("R", (int)(centre.x + radius + 2),  (int)(centre.y - 8), 14, 16,
                    juce::Justification::centred);
        g.drawText("M", (int)(centre.x - 7), (int)(centre.y - radius - 18), 14, 16,
                    juce::Justification::centred);
    }

    void drawCorrelationMeter(juce::Graphics& g)
    {
        // Horizontal bar: -1 (bad) â†’ 0 â†’ +1 (perfect)
        int barW = 200, barH = 12;
        int bx   = (getWidth() - barW) / 2;
        int by   = getHeight() - 145;

        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_BG_CONTROL));
        g.fillRoundedRectangle((float)bx, (float)by, (float)barW, (float)barH, 4.f);

        // Needle position
        float norm  = (correlationValue + 1.f) * 0.5f;  // remap -1..1 â†’ 0..1
        int   needleX = bx + (int)(norm * barW);

        juce::Colour needleColour = (correlationValue >= 0.f)
            ? juce::Colour(IzoCloneLookAndFeel::COL_BYPASS_ON)
            : juce::Colour(IzoCloneLookAndFeel::COL_METER_GR);

        g.setColour(needleColour);
        g.fillRoundedRectangle((float)needleX - 2, (float)by - 2, 4.f, (float)barH + 4, 2.f);

        // Labels
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        g.setFont(IzoCloneLookAndFeel::getLabelFont());
        g.drawText("-1", bx - 18, by, 18, barH, juce::Justification::centredRight);
        g.drawText("+1", bx + barW + 2, by, 18, barH, juce::Justification::centredLeft);
        g.drawText("CORRELATION", bx, by + barH + 2, barW, 12, juce::Justification::centred);
    }

    juce::AudioProcessorValueTreeState& apvts;

    juce::ToggleButton bypassBtn;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;

    juce::Slider widthSlider;
    juce::Label  widthLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> widthAttach;

    juce::Slider monoLoKnob;
    juce::Label  monoLoLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> monoLoAttach;
};

