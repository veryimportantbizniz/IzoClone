#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"

class CompressorModule : public juce::Component
{
public:
    CompressorModule(juce::AudioProcessorValueTreeState& apvts) : apvts(apvts)
    {
        bypassBtn.setButtonText("ACTIVE");
        bypassBtn.setClickingTogglesState(true);
        bypassBtn.setToggleState(true, juce::dontSendNotification);
        addAndMakeVisible(bypassBtn);
        bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, "comp_bypass", bypassBtn);

        const char* bands[] = { "lo", "mid", "hi" };
        const char* bandLabels[] = { "LOW BAND", "MID BAND", "HIGH BAND" };

        for (int b = 0; b < 3; ++b) {
            juce::String pfx = juce::String("comp_") + bands[b];

            addKnob(threshKnobs[b], threshLabels[b], pfx + "_thresh", "THRESH");
            addKnob(ratioKnobs[b],  ratioLabels[b],  pfx + "_ratio",  "RATIO");
            addKnob(attackKnobs[b], attackLabels[b], pfx + "_attack", "ATTACK");
            addKnob(relKnobs[b],    relLabels[b],    pfx + "_release","RELEASE");
            addKnob(gainKnobs[b],   gainLabels[b],   pfx + "_gain",   "GAIN");

            bandTitleLabels[b].setText(bandLabels[b], juce::dontSendNotification);
            bandTitleLabels[b].setFont(IzoCloneLookAndFeel::getTitleFont());
            bandTitleLabels[b].setJustificationType(juce::Justification::centred);
            bandTitleLabels[b].setColour(juce::Label::textColourId,
                                          juce::Colour(IzoCloneLookAndFeel::COL_ACCENT));
            addAndMakeVisible(bandTitleLabels[b]);
        }

        // Crossover knobs
        addKnob(xoverLoKnob, xoverLoLabel, "comp_crossover_lo", "XOVER LO");
        addKnob(xoverHiKnob, xoverHiLabel, "comp_crossover_hi", "XOVER HI");
    }

    void paint(juce::Graphics& g) override
    {
        IzoCloneLookAndFeel::drawModuleBackground(g, getLocalBounds(), "COMPRESSOR");

        // Draw 3 band column separators
        auto bounds = getLocalBounds().reduced(12);
        bounds.removeFromTop(44);
        int bw = bounds.getWidth() / 3;
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_BG_CONTROL));
        for (int i = 1; i < 3; ++i)
            g.drawVerticalLine(bounds.getX() + i * bw, (float)bounds.getY(), (float)bounds.getBottom());

        // GR meters (simple vertical bars)
        for (int b = 0; b < 3; ++b) {
            auto meterArea = getMeterArea(b);
            drawGRMeter(g, meterArea, gainReductionValues[b]);
        }
    }

    void resized() override
    {
        bypassBtn.setBounds(getWidth() - 72, 8, 60, 22);

        auto bounds = getLocalBounds().reduced(12);
        bounds.removeFromTop(44);
        int bw = bounds.getWidth() / 3;

        for (int b = 0; b < 3; ++b) {
            auto col = bounds.withWidth(bw).withX(bounds.getX() + b * bw).reduced(8, 0);
            bandTitleLabels[b].setBounds(col.removeFromTop(20));
            col.removeFromTop(4);

            auto meterArea = col.removeFromRight(20);
            col.removeFromRight(4);

            int kw = col.getWidth() / 5;
            placeKnob(threshKnobs[b], threshLabels[b], col.removeFromLeft(kw));
            placeKnob(ratioKnobs[b],  ratioLabels[b],  col.removeFromLeft(kw));
            placeKnob(attackKnobs[b], attackLabels[b], col.removeFromLeft(kw));
            placeKnob(relKnobs[b],    relLabels[b],    col.removeFromLeft(kw));
            placeKnob(gainKnobs[b],   gainLabels[b],   col.removeFromLeft(kw));
        }

        // Crossover knobs at bottom
        auto bottom = getLocalBounds().reduced(12).removeFromBottom(80);
        int xoverW = 80;
        placeKnob(xoverLoKnob, xoverLoLabel,
                   bottom.withWidth(xoverW).withX(bottom.getCentreX() - xoverW - 8));
        placeKnob(xoverHiKnob, xoverHiLabel,
                   bottom.withWidth(xoverW).withX(bottom.getCentreX() + 8));
    }

    void refreshMeters()
    {
        // Values are set externally by PluginEditor via CompressorProcessor::getGainReductionDb()
        repaint();
    }

    float gainReductionValues[3] = { 0.f, 0.f, 0.f };

private:
    juce::Rectangle<int> getMeterArea(int band) const
    {
        auto bounds = getLocalBounds().reduced(12);
        bounds.removeFromTop(64);
        int bw = bounds.getWidth() / 3;
        auto col = bounds.withWidth(bw).withX(bounds.getX() + band * bw).reduced(8, 0);
        col.removeFromTop(24);
        return col.removeFromRight(20);
    }

    void drawGRMeter(juce::Graphics& g, juce::Rectangle<int> area, float grDb)
    {
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_BG_CONTROL));
        g.fillRoundedRectangle(area.toFloat(), 3.f);

        float maxGR = 20.f;
        float norm  = juce::jlimit(0.f, 1.f, std::abs(grDb) / maxGR);
        int   fillH = (int)(area.getHeight() * norm);

        if (fillH > 0) {
            auto fill = area.removeFromBottom(fillH);
            g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_METER_GR));
            g.fillRoundedRectangle(fill.toFloat(), 3.f);
        }

        // GR label
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        g.setFont(juce::Font(juce::FontOptions("Helvetica Neue", 8.f, juce::Font::plain)));
        g.drawText("GR", area.getX() - 2, area.getBottom() + 2, 24, 10,
                    juce::Justification::centredLeft);
    }

    void addKnob(juce::Slider& s, juce::Label& l, const juce::String& id, const char* text)
    {
        s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 54, 14);
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

    juce::Slider threshKnobs[3], ratioKnobs[3], attackKnobs[3], relKnobs[3], gainKnobs[3];
    juce::Label  threshLabels[3], ratioLabels[3], attackLabels[3], relLabels[3], gainLabels[3];
    juce::Label  bandTitleLabels[3];

    juce::Slider xoverLoKnob, xoverHiKnob;
    juce::Label  xoverLoLabel, xoverHiLabel;

    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
};

