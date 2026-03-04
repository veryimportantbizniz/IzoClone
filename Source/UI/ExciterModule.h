#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"

class ExciterModule : public juce::Component
{
public:
    ExciterModule(juce::AudioProcessorValueTreeState& apvts) : apvts(apvts)
    {
        bypassBtn.setButtonText("ACTIVE");
        bypassBtn.setClickingTogglesState(true);
        bypassBtn.setToggleState(true, juce::dontSendNotification);
        addAndMakeVisible(bypassBtn);
        bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, "exciter_bypass", bypassBtn);

        addKnob(driveKnob, driveLabel, "exciter_drive", "DRIVE");
        addKnob(freqKnob,  freqLabel,  "exciter_freq",  "FREQ");
        addKnob(mixKnob,   mixLabel,   "exciter_mix",   "MIX");
    }

    void paint(juce::Graphics& g) override
    {
        IzoCloneLookAndFeel::drawModuleBackground(g, getLocalBounds(), "EXCITER");

        // Harmonic spectrum visualiser (decorative waveform)
        drawHarmonicDisplay(g);

        // "HARMONIC SATURATION" subtitle
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        g.setFont(IzoCloneLookAndFeel::getLabelFont());
        g.drawText("HARMONIC SATURATION",
                    getLocalBounds().withHeight(30).withY(getHeight() / 2 - 60),
                    juce::Justification::centred);
    }

    void resized() override
    {
        bypassBtn.setBounds(getWidth() - 72, 8, 60, 22);

        int kw = 90, kh = 90;
        int totalW = 3 * kw + 2 * 20;
        int startX = (getWidth() - totalW) / 2;
        int y = getHeight() - kh - 30;

        placeKnob(driveKnob, driveLabel, {startX,           y, kw, kh});
        placeKnob(freqKnob,  freqLabel,  {startX + kw + 20, y, kw, kh});
        placeKnob(mixKnob,   mixLabel,   {startX + 2*(kw+20), y, kw, kh});
    }

private:
    void drawHarmonicDisplay(juce::Graphics& g)
    {
        // Draw a stylised harmonic series bar chart based on drive value
        float drive = (float)driveKnob.getValue() / 100.f;
        float freq  = (float)freqKnob.getValue();
        float mix   = (float)mixKnob.getValue() / 100.f;

        int cx = getWidth() / 2;
        int cy = getHeight() / 2 - 20;
        int displayW = 300, displayH = 120;
        int dx = cx - displayW / 2;

        // Background box
        g.setColour(juce::Colour(0xFF0A0A14));
        g.fillRoundedRectangle((float)dx, (float)(cy - displayH / 2),
                                (float)displayW, (float)displayH, 6.f);

        // Draw harmonic bars (fundamental + harmonics)
        int numHarmonics = 8;
        float barW = (float)displayW / (numHarmonics * 2.f);
        for (int h = 1; h <= numHarmonics; ++h)
        {
            // Amplitude falls off with harmonic number, boosted by drive
            float amp = (1.f / h) * drive * mix;
            amp = juce::jmin(amp, 1.0f);
            int   barH   = (int)(amp * displayH * 0.85f);
            int   barX   = dx + (int)((h - 1) * 2 * barW + barW * 0.5f);
            int   barY   = cy + displayH / 2 - barH;

            // Even harmonics = warm amber, odd = teal
            juce::Colour col = (h % 2 == 0)
                ? juce::Colour(IzoCloneLookAndFeel::COL_ACCENT)
                : juce::Colour(IzoCloneLookAndFeel::COL_ACCENT2);
            g.setColour(col.withAlpha(0.7f + amp * 0.3f));
            g.fillRoundedRectangle((float)barX, (float)barY,
                                    barW * 1.4f, (float)barH, 2.f);
        }

        // Freq marker line
        float freqNorm = (std::log10(freq / 1000.f) + 1.2f) / 1.5f;
        freqNorm = juce::jlimit(0.f, 1.f, freqNorm);
        int   markerX = dx + (int)(freqNorm * displayW);
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_ACCENT).withAlpha(0.4f));
        g.drawVerticalLine(markerX, (float)(cy - displayH / 2), (float)(cy + displayH / 2));
        g.setFont(juce::Font(juce::FontOptions("Helvetica Neue", 9.f, juce::Font::plain)));
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_ACCENT));
        g.drawText(freq >= 1000.f ? juce::String(freq / 1000.f, 1) + "k" : juce::String((int)freq),
                    markerX - 15, cy - displayH / 2 - 14, 30, 12, juce::Justification::centred);
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

        // Repaint on change so harmonic display updates
        s.onValueChange = [this] { repaint(); };
    }

    void placeKnob(juce::Slider& s, juce::Label& l, juce::Rectangle<int> area)
    {
        l.setBounds(area.withHeight(14));
        s.setBounds(area.withTrimmedTop(14));
    }

    juce::AudioProcessorValueTreeState& apvts;

    juce::ToggleButton bypassBtn;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;

    juce::Slider driveKnob, freqKnob, mixKnob;
    juce::Label  driveLabel, freqLabel, mixLabel;

    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> attachments;
};

