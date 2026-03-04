#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

// ─────────────────────────────────────────────────────────────────────────────
//  EKGSlider — custom Love control
//  Draws an EKG heartbeat line as the track.
//  A heart shape slides along it, stays upright, glows on hover.
// ─────────────────────────────────────────────────────────────────────────────
class EKGSlider : public juce::Component
{
public:
    std::function<void(float)> onValueChange;
    float value = 0.5f;  // 0-1 normalised

    EKGSlider()
    {
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }

    void paint(juce::Graphics& g) override
    {
        auto b = getLocalBounds().toFloat().reduced(20.f, 10.f);
        float trackY = b.getCentreY();
        float trackX0 = b.getX();
        float trackW  = b.getWidth();

        // ── EKG line ──────────────────────────────────────────────────────────
        juce::Path ekg;
        ekg.startNewSubPath(trackX0, trackY);

        // EKG shape: flat → spike up → spike down → bump → flat
        // Repeated 3 times across the width
        int repeats = 3;
        float segW = trackW / (float)repeats;
        float amp  = b.getHeight() * 0.38f;

        for (int r = 0; r < repeats; ++r)
        {
            float ox = trackX0 + r * segW;
            float sw = segW;
            // flat in
            ekg.lineTo(ox + sw * 0.25f, trackY);
            // sharp up
            ekg.lineTo(ox + sw * 0.35f, trackY - amp * 1.8f);
            // sharp down
            ekg.lineTo(ox + sw * 0.42f, trackY + amp * 0.9f);
            // small bump
            ekg.lineTo(ox + sw * 0.52f, trackY - amp * 0.35f);
            ekg.lineTo(ox + sw * 0.60f, trackY);
            // flat out
            ekg.lineTo(ox + sw, trackY);
        }

        // Glow when hovered
        if (hovered)
        {
            g.setColour(juce::Colour(0xffff6b8a).withAlpha(0.3f));
            g.strokePath(ekg, juce::PathStrokeType(6.f));
        }

        g.setColour(juce::Colour(0xffe8003a).withAlpha(0.7f));
        g.strokePath(ekg, juce::PathStrokeType(2.f));

        // ── Heart thumb ───────────────────────────────────────────────────────
        float heartX = trackX0 + value * trackW;
        float heartY = trackY;
        float hs = 14.f + (hovered ? 3.f : 0.f) + pulseAmount * 4.f;

        // Glow around heart
        float glowAlpha = (hovered ? 0.4f : 0.15f) + pulseAmount * 0.3f;
        juce::ColourGradient glow(
            juce::Colour(0xffff0040).withAlpha(glowAlpha), heartX, heartY,
            juce::Colours::transparentBlack, heartX + hs * 2.f, heartY,
            true);
        g.setGradientFill(glow);
        g.fillEllipse(heartX - hs * 1.5f, heartY - hs * 1.5f, hs * 3.f, hs * 3.f);

        // Heart shape — always upright
        juce::Path heart;
        float hw = hs * 0.7f;
        heart.startNewSubPath(heartX, heartY + hw * 0.5f);
        heart.cubicTo(heartX - hw * 1.4f, heartY - hw * 0.7f,
                      heartX - hw * 2.0f, heartY + hw * 0.8f,
                      heartX, heartY + hw * 1.8f);
        heart.cubicTo(heartX + hw * 2.0f, heartY + hw * 0.8f,
                      heartX + hw * 1.4f, heartY - hw * 0.7f,
                      heartX, heartY + hw * 0.5f);

        juce::Colour heartCol = juce::Colour(0xffffb3c1).interpolatedWith(
            juce::Colour(0xffe8003a), value);
        if (hovered) heartCol = heartCol.brighter(0.3f);
        g.setColour(heartCol);
        g.fillPath(heart);
        g.setColour(juce::Colour(0xff880020).withAlpha(0.5f));
        g.strokePath(heart, juce::PathStrokeType(0.8f));

        // Value label below heart
        g.setFont(juce::Font("Arial", 10.f, juce::Font::plain));
        g.setColour(juce::Colour(0xffe8003a).withAlpha(0.8f));
        g.drawText(juce::String((int)(value * 100.f)),
                   (int)(heartX - 16.f), (int)(heartY + hs * 2.2f), 32, 14,
                   juce::Justification::centred);
    }

    void mouseDown(const juce::MouseEvent& e) override
    {
        dragStartX = e.x;
        dragStartVal = value;
        updateFromX(e.x);
    }

    void mouseDrag(const juce::MouseEvent& e) override
    {
        updateFromX(e.x);
    }

    void mouseEnter(const juce::MouseEvent&) override { hovered = true;  repaint(); }
    void mouseExit (const juce::MouseEvent&) override { hovered = false; repaint(); }

    void setPulse(float p) { pulseAmount = p; repaint(); }

private:
    void updateFromX(int x)
    {
        auto b = getLocalBounds().toFloat().reduced(20.f, 10.f);
        float newVal = juce::jlimit(0.f, 1.f, (x - b.getX()) / b.getWidth());
        if (newVal != value) {
            value = newVal;
            if (onValueChange) onValueChange(value);
            repaint();
        }
    }

    bool  hovered      = false;
    float pulseAmount  = 0.f;
    int   dragStartX   = 0;
    float dragStartVal = 0.f;
};


// ─────────────────────────────────────────────────────────────────────────────
//  LoveMasterEditor
// ─────────────────────────────────────────────────────────────────────────────
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

    // ── EKG Love slider ───────────────────────────────────────────────────────
    EKGSlider ekgSlider;
    juce::Label loveLabel;

    // ── Triage Center ─────────────────────────────────────────────────────────
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

    // ── Nerd Found overlay ────────────────────────────────────────────────────
    bool  nerdFoundVisible = false;
    float nerdFoundAlpha   = 0.f;
    int   nerdFoundTimer   = 0;

    // ── Animation ─────────────────────────────────────────────────────────────
    float heartAngle     = 0.f;
    float heartPulse     = 0.f;
    std::array<float, 256> waveformDisplay {};

    static constexpr int BASE_WIDTH   = 480;
    static constexpr int BASE_HEIGHT  = 320;
    static constexpr int TRIAGE_WIDTH = 300;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LoveMasterEditor)
};
