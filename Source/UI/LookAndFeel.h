#pragma once
#include <JuceHeader.h>

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  IzoCloneLookAndFeel
//  Dark industrial aesthetic with amber/gold accent colour
//  Inspired by high-end hardware mastering units
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
class IzoCloneLookAndFeel : public juce::LookAndFeel_V4
{
public:
    // â”€â”€ Colour palette â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    static constexpr juce::uint32 COL_BG_DARK    = 0xFF0D0D12;
    static constexpr juce::uint32 COL_BG_MID     = 0xFF15151E;
    static constexpr juce::uint32 COL_BG_PANEL   = 0xFF1C1C28;
    static constexpr juce::uint32 COL_BG_CONTROL = 0xFF242435;
    static constexpr juce::uint32 COL_ACCENT     = 0xFFE8A020;   // Amber gold
    static constexpr juce::uint32 COL_ACCENT2    = 0xFF4ECDC4;   // Teal highlight
    static constexpr juce::uint32 COL_TEXT       = 0xFFD8D8E0;
    static constexpr juce::uint32 COL_TEXT_DIM   = 0xFF707088;
    static constexpr juce::uint32 COL_BYPASS_ON  = 0xFF22CC55;   // Green when active
    static constexpr juce::uint32 COL_METER_GR   = 0xFFE84040;   // Red for GR

    IzoCloneLookAndFeel()
    {
        // Set JUCE colour scheme
        setColour(juce::ResizableWindow::backgroundColourId,      juce::Colour(COL_BG_DARK));
        setColour(juce::Slider::thumbColourId,                    juce::Colour(COL_ACCENT));
        setColour(juce::Slider::trackColourId,                    juce::Colour(COL_BG_CONTROL));
        setColour(juce::Slider::backgroundColourId,               juce::Colour(COL_BG_CONTROL));
        setColour(juce::Slider::rotarySliderFillColourId,         juce::Colour(COL_ACCENT));
        setColour(juce::Slider::rotarySliderOutlineColourId,      juce::Colour(COL_BG_CONTROL));
        setColour(juce::Label::textColourId,                      juce::Colour(COL_TEXT));
        setColour(juce::TextButton::buttonColourId,               juce::Colour(COL_BG_CONTROL));
        setColour(juce::TextButton::buttonOnColourId,             juce::Colour(COL_ACCENT));
        setColour(juce::TextButton::textColourOffId,              juce::Colour(COL_TEXT_DIM));
        setColour(juce::TextButton::textColourOnId,               juce::Colour(COL_BG_DARK));
        setColour(juce::ComboBox::backgroundColourId,             juce::Colour(COL_BG_CONTROL));
        setColour(juce::ComboBox::textColourId,                   juce::Colour(COL_TEXT));
        setColour(juce::ComboBox::arrowColourId,                  juce::Colour(COL_ACCENT));
        setColour(juce::PopupMenu::backgroundColourId,            juce::Colour(COL_BG_PANEL));
        setColour(juce::PopupMenu::textColourId,                  juce::Colour(COL_TEXT));
        setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(COL_ACCENT).withAlpha(0.3f));
    }

    // â”€â”€ Rotary knob drawing â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                           juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(4.f);
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        auto centre = bounds.getCentre();
        auto lineW  = radius * 0.08f;

        // Outer ring
        g.setColour(juce::Colour(COL_BG_CONTROL));
        g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2.f, radius * 2.f);

        // Arc track
        juce::Path arcTrack;
        arcTrack.addCentredArc(centre.x, centre.y, radius - lineW, radius - lineW,
                                0, rotaryStartAngle, rotaryEndAngle, true);
        g.setColour(juce::Colour(COL_BG_DARK));
        g.strokePath(arcTrack, juce::PathStrokeType(lineW, juce::PathStrokeType::curved,
                                                     juce::PathStrokeType::rounded));

        // Filled arc
        juce::Path arcFill;
        arcFill.addCentredArc(centre.x, centre.y, radius - lineW, radius - lineW,
                               0, rotaryStartAngle,
                               rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle), true);
        g.setColour(juce::Colour(COL_ACCENT));
        g.strokePath(arcFill, juce::PathStrokeType(lineW, juce::PathStrokeType::curved,
                                                     juce::PathStrokeType::rounded));

        // Knob body
        auto knobR = radius * 0.62f;
        juce::ColourGradient knobGrad(juce::Colour(COL_BG_PANEL).brighter(0.15f),
                                        centre.x - knobR * 0.3f, centre.y - knobR * 0.3f,
                                        juce::Colour(COL_BG_DARK),
                                        centre.x + knobR * 0.5f, centre.y + knobR * 0.5f, true);
        g.setGradientFill(knobGrad);
        g.fillEllipse(centre.x - knobR, centre.y - knobR, knobR * 2.f, knobR * 2.f);

        // Pointer line
        auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        juce::Point<float> tip(centre.x + (knobR * 0.7f) * std::sin(angle),
                                 centre.y - (knobR * 0.7f) * std::cos(angle));
        g.setColour(juce::Colour(COL_TEXT));
        g.drawLine(centre.x, centre.y, tip.x, tip.y, lineW * 0.9f);
    }

    // â”€â”€ Bypass toggle button â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(2.f);
        bool on     = button.getToggleState();

        g.setColour(on ? juce::Colour(COL_BYPASS_ON) : juce::Colour(COL_BG_CONTROL));
        g.fillRoundedRectangle(bounds, 4.f);

        g.setColour(on ? juce::Colour(COL_BG_DARK) : juce::Colour(COL_TEXT_DIM));
        g.setFont(juce::Font(juce::FontOptions("Helvetica Neue", bounds.getHeight() * 0.45f,
                                                 juce::Font::bold)));
        g.drawText(button.getButtonText(), bounds, juce::Justification::centred);
    }

    // â”€â”€ Linear slider â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                           float sliderPos, float minSliderPos, float maxSliderPos,
                           juce::Slider::SliderStyle style, juce::Slider& slider) override
    {
        if (style == juce::Slider::LinearVertical)
        {
            float trackW = 4.f;
            float cx     = x + width * 0.5f;

            // Track background
            g.setColour(juce::Colour(COL_BG_CONTROL));
            g.fillRoundedRectangle(cx - trackW * 0.5f, (float)y, trackW, (float)height, 2.f);

            // Filled portion
            g.setColour(juce::Colour(COL_ACCENT));
            g.fillRoundedRectangle(cx - trackW * 0.5f, sliderPos, trackW,
                                    (float)(y + height) - sliderPos, 2.f);

            // Thumb
            g.setColour(juce::Colour(COL_ACCENT));
            g.fillEllipse(cx - 7.f, sliderPos - 7.f, 14.f, 14.f);
        }
        else
        {
            LookAndFeel_V4::drawLinearSlider(g, x, y, width, height, sliderPos,
                                              minSliderPos, maxSliderPos, style, slider);
        }
    }

    // â”€â”€ Shared helpers â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    static void drawModuleBackground(juce::Graphics& g, juce::Rectangle<int> bounds,
                                      const juce::String& title)
    {
        // Panel background
        g.setColour(juce::Colour(COL_BG_PANEL));
        g.fillRoundedRectangle(bounds.toFloat(), 8.f);

        // Subtle border
        g.setColour(juce::Colour(COL_BG_CONTROL));
        g.drawRoundedRectangle(bounds.toFloat().reduced(0.5f), 8.f, 1.f);

        // Accent line at top of panel
        auto accentBar = bounds.removeFromTop(2).toFloat();
        g.setColour(juce::Colour(COL_ACCENT));
        g.fillRoundedRectangle(accentBar, 1.f);
    }

    static juce::Font getLabelFont()
    {
        return juce::Font(juce::FontOptions("Helvetica Neue", 10.5f, juce::Font::plain));
    }

    static juce::Font getTitleFont()
    {
        return juce::Font(juce::FontOptions("Helvetica Neue", 13.f, juce::Font::bold));
    }
};

