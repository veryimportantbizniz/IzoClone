#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"
#include "../Processors/AllProcessors.h"
#include "../Processors/EQProcessor.h"

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  EQModule
//  Displays: spectrum analyzer background + EQ curve overlay + 5 knob controls
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
class EQModule : public juce::Component
{
public:
    EQModule(juce::AudioProcessorValueTreeState& apvts,
             FFTAnalyzer& fftAnalyzer,
             EQProcessor& eqProcessor)
        : apvts(apvts), fftAnalyzer(fftAnalyzer), eqProcessor(eqProcessor)
    {
        // Bypass toggle
        bypassBtn.setButtonText("ACTIVE");
        bypassBtn.setClickingTogglesState(true);
        bypassBtn.setToggleState(true, juce::dontSendNotification);
        addAndMakeVisible(bypassBtn);
        bypassAttach = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            apvts, "eq_bypass", bypassBtn);

        // â”€â”€ Knob factory â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
        auto makeKnob = [&](juce::Slider& s, juce::Label& l,
                             const juce::String& paramId, const juce::String& labelText)
        {
            s.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
            s.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 16);
            s.setDoubleClickReturnValue(true, (double)apvts.getParameter(paramId)->getDefaultValue());
            addAndMakeVisible(s);
            knobAttachments.push_back(
                std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
                    apvts, paramId, s));

            l.setText(labelText, juce::dontSendNotification);
            l.setJustificationType(juce::Justification::centred);
            l.setFont(IzoCloneLookAndFeel::getLabelFont());
            l.setColour(juce::Label::textColourId,
                         juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
            addAndMakeVisible(l);
        };

        makeKnob(lsFreqKnob,  lsFreqLabel,  "eq_lowshelf_freq",  "LS FREQ");
        makeKnob(lsGainKnob,  lsGainLabel,  "eq_lowshelf_gain",  "LS GAIN");
        makeKnob(lmFreqKnob,  lmFreqLabel,  "eq_low_mid_freq",   "LM FREQ");
        makeKnob(lmGainKnob,  lmGainLabel,  "eq_low_mid_gain",   "LM GAIN");
        makeKnob(lmQKnob,     lmQLabel,     "eq_low_mid_q",      "LM Q");
        makeKnob(hmFreqKnob,  hmFreqLabel,  "eq_high_mid_freq",  "HM FREQ");
        makeKnob(hmGainKnob,  hmGainLabel,  "eq_high_mid_gain",  "HM GAIN");
        makeKnob(hmQKnob,     hmQLabel,     "eq_high_mid_q",     "HM Q");
        makeKnob(hsFreqKnob,  hsFreqLabel,  "eq_highshelf_freq", "HS FREQ");
        makeKnob(hsGainKnob,  hsGainLabel,  "eq_highshelf_gain", "HS GAIN");

        // Build frequency axis for response curve (256 points, log scale)
        const int numPoints = 256;
        freqs.resize(numPoints);
        magnitudes.resize(numPoints);
        for (int i = 0; i < numPoints; ++i)
            freqs[i] = 20.0 * std::pow(1000.0, (double)i / (numPoints - 1));
    }

    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds();
        IzoCloneLookAndFeel::drawModuleBackground(g, bounds, "EQ");

        auto displayArea = bounds.reduced(12).removeFromTop(bounds.getHeight() - 130);

        // â”€â”€ Spectrum background â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
        drawSpectrumBackground(g, displayArea);
        drawFFTSpectrum(g, displayArea);
        drawEQCurve(g, displayArea);
        drawFrequencyLabels(g, displayArea);
    }

    void resized() override
    {
        auto bounds = getLocalBounds().reduced(12);
        bounds.removeFromTop(bounds.getHeight() - 118);   // Knobs at bottom

        bypassBtn.setBounds(getWidth() - 72, 8, 60, 22);

        // Layout: LS | LM (3 knobs) | HM (3 knobs) | HS
        int kW = 70, kH = 80, padding = 8;
        int totalW = 2 * kW + 3 * kW + 3 * kW + 2 * kW + 9 * padding;
        int startX = (getWidth() - totalW) / 2;
        int y = bounds.getY() + 10;

        auto placeKnob = [&](juce::Slider& s, juce::Label& l, int x) {
            l.setBounds(x, y, kW, 14);
            s.setBounds(x, y + 14, kW, kH);
        };

        placeKnob(lsFreqKnob,  lsFreqLabel,  startX);
        placeKnob(lsGainKnob,  lsGainLabel,  startX + kW + padding);
        startX += 2 * kW + 2 * padding + 16;

        placeKnob(lmFreqKnob,  lmFreqLabel,  startX);
        placeKnob(lmGainKnob,  lmGainLabel,  startX + kW + padding);
        placeKnob(lmQKnob,     lmQLabel,     startX + 2 * (kW + padding));
        startX += 3 * kW + 3 * padding + 16;

        placeKnob(hmFreqKnob,  hmFreqLabel,  startX);
        placeKnob(hmGainKnob,  hmGainLabel,  startX + kW + padding);
        placeKnob(hmQKnob,     hmQLabel,     startX + 2 * (kW + padding));
        startX += 3 * kW + 3 * padding + 16;

        placeKnob(hsFreqKnob,  hsFreqLabel,  startX);
        placeKnob(hsGainKnob,  hsGainLabel,  startX + kW + padding);
    }

    void refreshSpectrum()
    {
        if (fftAnalyzer.getLatestSpectrum(spectrumData))
            repaint();
    }

private:
    void drawSpectrumBackground(juce::Graphics& g, juce::Rectangle<int> area)
    {
        g.setColour(juce::Colour(0xFF0A0A14));
        g.fillRoundedRectangle(area.toFloat(), 6.f);

        // Grid lines
        g.setColour(juce::Colour(0xFF222230));
        for (float db : { -48.f, -36.f, -24.f, -12.f, 0.f })
        {
            float y = dbToY(db, area);
            g.drawHorizontalLine((int)y, (float)area.getX(), (float)area.getRight());
        }

        static const float gridFreqs[] = { 50, 100, 200, 500, 1000, 2000, 5000, 10000, 20000 };
        for (float f : gridFreqs)
        {
            float x = freqToX(f, area);
            g.drawVerticalLine((int)x, (float)area.getY(), (float)area.getBottom());
        }
    }

    void drawFFTSpectrum(juce::Graphics& g, juce::Rectangle<int> area)
    {
        if (spectrumData.empty()) return;

        juce::Path specPath;
        bool first = true;
        for (int i = 0; i < (int)spectrumData.size(); ++i)
        {
            float freq  = 20.f * std::pow(1000.f, (float)i / (spectrumData.size() - 1));
            float x     = freqToX(freq, area);
            float db    = juce::Decibels::gainToDecibels(spectrumData[i], -80.f);
            float y     = dbToY(db, area);

            if (first) { specPath.startNewSubPath(x, y); first = false; }
            else specPath.lineTo(x, y);
        }

        // Fill under the spectrum
        specPath.lineTo((float)area.getRight(), (float)area.getBottom());
        specPath.lineTo((float)area.getX(),     (float)area.getBottom());
        specPath.closeSubPath();

        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_ACCENT2).withAlpha(0.12f));
        g.fillPath(specPath);

        // Line
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_ACCENT2).withAlpha(0.5f));
        g.strokePath(specPath, juce::PathStrokeType(1.2f));
    }

    void drawEQCurve(juce::Graphics& g, juce::Rectangle<int> area)
    {
        // Pull real magnitude response from the live EQ processor
        eqProcessor.getMagnitudeResponse(freqs, magnitudes);

        juce::Path curvePath;
        bool first = true;
        for (size_t i = 0; i < freqs.size(); ++i)
        {
            float x  = freqToX((float)freqs[i], area);
            float y  = dbToY((float)magnitudes[i], area);
            if (first) { curvePath.startNewSubPath(x, y); first = false; }
            else curvePath.lineTo(x, y);
        }

        // Glow fill under curve
        juce::Path fillPath = curvePath;
        fillPath.lineTo((float)area.getRight(), dbToY(0.f, area));
        fillPath.lineTo((float)area.getX(),     dbToY(0.f, area));
        fillPath.closeSubPath();
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_ACCENT).withAlpha(0.08f));
        g.fillPath(fillPath);

        // Main curve line
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_ACCENT));
        g.strokePath(curvePath, juce::PathStrokeType(2.f, juce::PathStrokeType::curved,
                                                       juce::PathStrokeType::rounded));

        // 0dB reference line
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM).withAlpha(0.3f));
        g.drawHorizontalLine((int)dbToY(0.f, area), (float)area.getX(), (float)area.getRight());
    }

    void drawFrequencyLabels(juce::Graphics& g, juce::Rectangle<int> area)
    {
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        g.setFont(IzoCloneLookAndFeel::getLabelFont());

        const std::pair<float, const char*> labels[] = {
            {50,"50"},{100,"100"},{200,"200"},{500,"500"},
            {1000,"1k"},{2000,"2k"},{5000,"5k"},{10000,"10k"},{20000,"20k"}
        };
        for (auto& [f, t] : labels) {
            float x = freqToX(f, area);
            g.drawText(t, (int)x - 16, area.getBottom() - 16, 32, 14,
                        juce::Justification::centred);
        }
    }

    // â”€â”€ Coordinate helpers â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    float freqToX(float freq, juce::Rectangle<int> area) const
    {
        return area.getX() + (float)area.getWidth()
               * (std::log10(freq / 20.f) / std::log10(20000.f / 20.f));
    }
    float dbToY(float db, juce::Rectangle<int> area) const
    {
        float minDb = -60.f, maxDb = 6.f;
        return area.getBottom() - (db - minDb) / (maxDb - minDb) * area.getHeight();
    }

    // â”€â”€ Members â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    juce::AudioProcessorValueTreeState& apvts;
    FFTAnalyzer&                         fftAnalyzer;
    EQProcessor&                         eqProcessor;

    juce::ToggleButton bypassBtn;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> bypassAttach;

    juce::Slider lsFreqKnob, lsGainKnob;
    juce::Slider lmFreqKnob, lmGainKnob, lmQKnob;
    juce::Slider hmFreqKnob, hmGainKnob, hmQKnob;
    juce::Slider hsFreqKnob, hsGainKnob;

    juce::Label lsFreqLabel, lsGainLabel;
    juce::Label lmFreqLabel, lmGainLabel, lmQLabel;
    juce::Label hmFreqLabel, hmGainLabel, hmQLabel;
    juce::Label hsFreqLabel, hsGainLabel;

    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> knobAttachments;

    std::vector<double> freqs, magnitudes;
    std::vector<float>  spectrumData;
};

