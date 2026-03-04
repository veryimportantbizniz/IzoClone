#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"
#include "../Processors/AllProcessors.h"

class MeterModule : public juce::Component
{
public:
    MeterModule(LoudnessMeter& meter) : loudnessMeter(meter) {}

    void paint(juce::Graphics& g) override
    {
        IzoCloneLookAndFeel::drawModuleBackground(g, getLocalBounds(), "METERS");

        auto bounds = getLocalBounds().reduced(20);
        bounds.removeFromTop(40);

        int mw = bounds.getWidth() / 4;

        // Draw 4 meters side by side
        drawLUFSMeter(g, bounds.removeFromLeft(mw), "MOMENTARY", momentary, -60.f, 0.f);
        drawLUFSMeter(g, bounds.removeFromLeft(mw), "SHORT TERM", shortTerm, -60.f, 0.f);
        drawLUFSMeter(g, bounds.removeFromLeft(mw), "INTEGRATED", integrated, -60.f, 0.f);
        drawTruePeakMeter(g, bounds.removeFromLeft(mw), "TRUE PEAK", truePeak);
    }

    void resized() override {}

    void refresh()
    {
        momentary  = loudnessMeter.getMomentaryLUFS();
        shortTerm  = loudnessMeter.getShortTermLUFS();
        integrated = loudnessMeter.getIntegratedLUFS();
        truePeak   = loudnessMeter.getTruePeakDb();
        repaint();
    }

private:
    void drawLUFSMeter(juce::Graphics& g, juce::Rectangle<int> area,
                        const char* title, float valueLUFS,
                        float minVal, float maxVal)
    {
        area.reduce(6, 0);

        // Title
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        g.setFont(IzoCloneLookAndFeel::getLabelFont());
        g.drawText(title, area.withHeight(14), juce::Justification::centred);
        area.removeFromTop(18);

        // Numeric value
        g.setColour(valueLUFS > -9.f
            ? juce::Colour(IzoCloneLookAndFeel::COL_METER_GR)
            : juce::Colour(IzoCloneLookAndFeel::COL_ACCENT));
        g.setFont(juce::Font(juce::FontOptions("Helvetica Neue", 16.f, juce::Font::bold)));
        juce::String valStr = (valueLUFS <= -70.f) ? "---" : juce::String(valueLUFS, 1);
        g.drawText(valStr + " LU", area.withHeight(22), juce::Justification::centred);
        area.removeFromTop(26);

        // Vertical bar
        drawVerticalBar(g, area, valueLUFS, minVal, maxVal, false);
    }

    void drawTruePeakMeter(juce::Graphics& g, juce::Rectangle<int> area,
                             const char* title, float valueDb)
    {
        area.reduce(6, 0);

        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        g.setFont(IzoCloneLookAndFeel::getLabelFont());
        g.drawText(title, area.withHeight(14), juce::Justification::centred);
        area.removeFromTop(18);

        bool isClipping = valueDb >= 0.f;
        g.setColour(isClipping
            ? juce::Colour(IzoCloneLookAndFeel::COL_METER_GR)
            : juce::Colour(IzoCloneLookAndFeel::COL_TEXT));
        g.setFont(juce::Font(juce::FontOptions("Helvetica Neue", 16.f, juce::Font::bold)));
        juce::String valStr = (valueDb <= -70.f) ? "---" : juce::String(valueDb, 1);
        g.drawText(valStr + " dBTP", area.withHeight(22), juce::Justification::centred);
        area.removeFromTop(26);

        drawVerticalBar(g, area, valueDb, -60.f, 3.f, true);
    }

    void drawVerticalBar(juce::Graphics& g, juce::Rectangle<int> area,
                          float value, float minVal, float maxVal, bool isTruePeak)
    {
        // Background
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_BG_CONTROL));
        g.fillRoundedRectangle(area.toFloat(), 4.f);

        float norm = juce::jlimit(0.f, 1.f, (value - minVal) / (maxVal - minVal));
        int   fillH = (int)(area.getHeight() * norm);

        if (fillH > 0) {
            auto fill = area.removeFromBottom(fillH);

            // Colour: green below -6, amber -6 to -1, red above -1
            juce::Colour col;
            if (!isTruePeak) {
                col = (value > -9.f)  ? juce::Colour(IzoCloneLookAndFeel::COL_METER_GR)
                    : (value > -14.f) ? juce::Colour(IzoCloneLookAndFeel::COL_ACCENT)
                    :                   juce::Colour(IzoCloneLookAndFeel::COL_BYPASS_ON);
            } else {
                col = (value >= 0.f)  ? juce::Colour(IzoCloneLookAndFeel::COL_METER_GR)
                    : (value > -3.f)  ? juce::Colour(IzoCloneLookAndFeel::COL_ACCENT)
                    :                   juce::Colour(IzoCloneLookAndFeel::COL_BYPASS_ON);
            }

            g.setColour(col);
            g.fillRoundedRectangle(fill.toFloat(), 4.f);
        }

        // Scale markings
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        g.setFont(juce::Font(juce::FontOptions("Helvetica Neue", 8.f, juce::Font::plain)));
        for (float mark : { -6.f, -14.f, -23.f, -40.f }) {
            if (mark < minVal || mark > maxVal) continue;
            float yn = (mark - minVal) / (maxVal - minVal);
            int   y  = area.getBottom() - (int)(yn * area.getHeight());
            g.drawHorizontalLine(y, (float)area.getX() - 6, (float)area.getX());
            g.drawText(juce::String((int)mark), area.getX() - 26, y - 5, 22, 10,
                        juce::Justification::centredRight);
        }
    }

    LoudnessMeter& loudnessMeter;
    float momentary  = -70.f;
    float shortTerm  = -70.f;
    float integrated = -70.f;
    float truePeak   = -70.f;
};

