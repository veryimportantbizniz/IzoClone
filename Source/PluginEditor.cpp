#include "PluginEditor.h"

LoveMasterEditor::LoveMasterEditor(LoveMasterProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(MAIN_WIDTH, MAIN_HEIGHT);
    setResizable(false, false);

    // â”€â”€ Love knob â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    loveKnob.setSliderStyle(juce::Slider::RotaryVerticalDrag);
    loveKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 60, 20);
    loveKnob.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffe8003a));
    loveKnob.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xffff6b8a));
    loveKnob.setColour(juce::Slider::thumbColourId, juce::Colour(0xffff0040));
    loveKnob.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xffe8003a));
    loveKnob.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
    addAndMakeVisible(loveKnob);

    loveAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.apvts, "love", loveKnob);

    loveLabel.setText("LOVE", juce::dontSendNotification);
    loveLabel.setFont(juce::Font("Georgia", 18.f, juce::Font::bold | juce::Font::italic));
    loveLabel.setColour(juce::Label::textColourId, juce::Colour(0xffe8003a));
    loveLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(loveLabel);

    // â”€â”€ Triage knobs â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    for (int i = 0; i < 8; ++i)
    {
        auto& tk = triageKnobs[i];
        tk.slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        tk.slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 58, 16);
        tk.slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xffcc0033));
        tk.slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xffff6b8a));
        tk.slider.setColour(juce::Slider::textBoxTextColourId, juce::Colour(0xff880020));
        tk.slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        tk.slider.setVisible(false);
        addChildComponent(tk.slider);

        tk.label.setText(triageLabels[i], juce::dontSendNotification);
        tk.label.setFont(juce::Font("Arial", 10.f, juce::Font::plain));
        tk.label.setColour(juce::Label::textColourId, juce::Colour(0xff880020));
        tk.label.setJustificationType(juce::Justification::centred);
        tk.label.setVisible(false);
        addChildComponent(tk.label);

        tk.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.apvts, triageParamIDs[i], tk.slider);
    }

    // â”€â”€ For Nerds button â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    nerdsButton.setButtonText("for nerds");
    nerdsButton.setColour(juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    nerdsButton.setColour(juce::TextButton::textColourOffId, juce::Colour(0xffcccccc));
    nerdsButton.setColour(juce::ComboBox::outlineColourId, juce::Colours::transparentBlack);
    nerdsButton.onClick = [this]() {
        if (!triageOpen) openTriageCenter();
        else             closeTriageCenter();
    };
    addAndMakeVisible(nerdsButton);

    startTimerHz(60);
}

LoveMasterEditor::~LoveMasterEditor()
{
    stopTimer();
}

// â”€â”€ Layout â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void LoveMasterEditor::resized()
{
    int w = getWidth();
    int h = getHeight();

    // Love knob â€” centred in main area
    int knobSize = 160;
    int knobX = (MAIN_WIDTH - knobSize) / 2;
    int knobY = 80;
    loveKnob.setBounds(knobX, knobY, knobSize, knobSize);
    loveLabel.setBounds(knobX, knobY - 28, knobSize, 24);

    // For Nerds button â€” bottom left
    nerdsButton.setBounds(8, h - 26, 80, 20);

    // Triage knobs â€” 4x2 grid in the right panel
    if (triageOpen)
    {
        int triageX = MAIN_WIDTH + 10;
        int cols = 4, rows = 2;
        int kw = 68, kh = 80;
        int padX = (TRIAGE_WIDTH - cols * kw) / (cols + 1);
        int padY = 60;

        for (int i = 0; i < 8; ++i)
        {
            int col = i % cols;
            int row = i / cols;
            int x = triageX + padX + col * (kw + padX);
            int y = padY + row * (kh + 30);
            triageKnobs[i].slider.setBounds(x, y, kw, kh - 18);
            triageKnobs[i].label.setBounds(x, y + kh - 16, kw, 16);
        }
    }
}

// â”€â”€ Paint â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void LoveMasterEditor::paint(juce::Graphics& g)
{
    // Flat white background
    g.fillAll(juce::Colours::white);

    // Triage panel background
    if (triageOpen)
    {
        g.setColour(juce::Colour(0xfff8f0f2));
        g.fillRect(MAIN_WIDTH, 0, TRIAGE_WIDTH, getHeight());
        g.setColour(juce::Colour(0xffff6b8a));
        g.drawLine((float)MAIN_WIDTH, 0.f, (float)MAIN_WIDTH, (float)getHeight(), 1.5f);

        // Triage Center title
        g.setColour(juce::Colour(0xffe8003a));
        g.setFont(juce::Font("Georgia", 14.f, juce::Font::bold | juce::Font::italic));
        g.drawText("Triage Center", MAIN_WIDTH, 20, TRIAGE_WIDTH, 28,
                   juce::Justification::centred);
    }

    // Plugin name
    g.setFont(juce::Font("Georgia", 22.f, juce::Font::bold | juce::Font::italic));
    g.setColour(juce::Colour(0xffe8003a));
    g.drawText("LoveMaster", 0, 14, MAIN_WIDTH, 30, juce::Justification::centred);

    // Waveform display
    paintWaveform(g);

    // Spinning heart
    paintSpinningHeart(g);

    // Nerd Found overlay
    if (nerdFoundVisible)
        paintNerdFoundOverlay(g);
}

void LoveMasterEditor::paintWaveform(juce::Graphics& g)
{
    const int wx = 20, wy = 260, ww = MAIN_WIDTH - 40, wh = 50;

    // Background track
    g.setColour(juce::Colour(0xfff5e8eb));
    g.fillRoundedRectangle((float)wx, (float)wy, (float)ww, (float)wh, 4.f);

    // Draw waveform in reds
    juce::Path wave;
    bool first = true;
    for (int x = 0; x < ww; ++x)
    {
        int idx = (x * LoveMasterProcessor::WAVEFORM_SIZE / ww) & (LoveMasterProcessor::WAVEFORM_SIZE - 1);
        float sample = waveformDisplay[x * 256 / ww];
        float y = wy + wh * 0.5f - sample * wh * 0.45f;
        y = juce::jlimit((float)wy, (float)(wy + wh), y);
        if (first) { wave.startNewSubPath((float)(wx + x), y); first = false; }
        else        wave.lineTo((float)(wx + x), y);
    }

    // Colour based on level
    float rms = 0.f;
    for (auto v : waveformDisplay) rms += v * v;
    rms = std::sqrt(rms / 256.f);
    float loud = juce::jlimit(0.f, 1.f, rms * 8.f);

    juce::Colour waveColour = juce::Colour(0xffffb3c1).interpolatedWith(juce::Colour(0xffaa0020), loud);
    g.setColour(waveColour);
    g.strokePath(wave, juce::PathStrokeType(1.8f));
}

void LoveMasterEditor::paintSpinningHeart(juce::Graphics& g)
{
    // Spinning heart lives top-right corner
    const float cx = MAIN_WIDTH - 28.f, cy = 22.f, size = 14.f;

    juce::Graphics::ScopedSaveState save(g);
    g.addTransform(juce::AffineTransform::rotation(heartAngle, cx, cy));

    // Draw a heart shape
    juce::Path heart;
    heart.startNewSubPath(cx, cy + size * 0.3f);
    heart.cubicTo(cx - size, cy - size * 0.5f,
                  cx - size * 1.5f, cy + size * 0.5f,
                  cx, cy + size);
    heart.cubicTo(cx + size * 1.5f, cy + size * 0.5f,
                  cx + size, cy - size * 0.5f,
                  cx, cy + size * 0.3f);

    float love = (float)processor.apvts.getRawParameterValue("love")->load() / 100.f;
    juce::Colour heartColour = juce::Colour(0xffffb3c1).interpolatedWith(juce::Colour(0xffe8003a), love);
    g.setColour(heartColour);
    g.fillPath(heart);
}

void LoveMasterEditor::paintNerdFoundOverlay(juce::Graphics& g)
{
    // Dark vignette
    juce::ColourGradient vignette(
        juce::Colours::black.withAlpha(0.85f * nerdFoundAlpha), (float)getWidth() * 0.5f, (float)getHeight() * 0.5f,
        juce::Colours::black.withAlpha(0.3f  * nerdFoundAlpha), 0.f, 0.f,
        true);
    g.setGradientFill(vignette);
    g.fillAll();

    // "NERD FOUND" in gothic style
    g.setFont(juce::Font("Georgia", 38.f, juce::Font::bold | juce::Font::italic));
    g.setColour(juce::Colour(0xffff6b8a).withAlpha(nerdFoundAlpha));

    // Flicker effect â€” slight offset shadow first
    g.setColour(juce::Colour(0xff880020).withAlpha(nerdFoundAlpha * 0.6f));
    g.drawText("NERD FOUND", 3, getHeight()/2 - 27, getWidth(), 54, juce::Justification::centred);

    g.setColour(juce::Colour(0xffff6b8a).withAlpha(nerdFoundAlpha));
    g.drawText("NERD FOUND", 0, getHeight()/2 - 28, getWidth(), 54, juce::Justification::centred);
}

// â”€â”€ Timer â€” 60hz animation tick â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void LoveMasterEditor::timerCallback()
{
    float love = (float)processor.apvts.getRawParameterValue("love")->load() / 100.f;

    // Spin heart â€” faster with more love
    heartAngle += 0.008f + love * 0.06f;
    if (heartAngle > juce::MathConstants<float>::twoPi)
        heartAngle -= juce::MathConstants<float>::twoPi;

    // Update waveform display with smoothing
    for (int i = 0; i < 256; ++i)
    {
        int srcIdx = (i * LoveMasterProcessor::WAVEFORM_SIZE / 256) & (LoveMasterProcessor::WAVEFORM_SIZE - 1);
        float incoming = processor.getWaveformSample(srcIdx);
        waveformDisplay[i] = waveformDisplay[i] * 0.7f + incoming * 0.3f;
    }

    // Nerd Found animation
    if (nerdFoundVisible)
    {
        nerdFoundTimer++;
        if (nerdFoundTimer < 15)
            nerdFoundAlpha = (float)nerdFoundTimer / 15.f;
        else if (nerdFoundTimer > 75)
            nerdFoundAlpha = 1.f - (float)(nerdFoundTimer - 75) / 15.f;

        if (nerdFoundTimer > 90)
        {
            nerdFoundVisible = false;
            nerdFoundAlpha   = 0.f;
        }
    }

    repaint();
}

// â”€â”€ Triage open/close â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void LoveMasterEditor::openTriageCenter()
{
    triageOpen = true;
    setSize(MAIN_WIDTH + TRIAGE_WIDTH, MAIN_HEIGHT);

    for (auto& tk : triageKnobs) {
        tk.slider.setVisible(true);
        tk.label.setVisible(true);
    }

    // Trigger NERD FOUND
    nerdFoundVisible = true;
    nerdFoundAlpha   = 0.f;
    nerdFoundTimer   = 0;

    nerdsButton.setButtonText("nevermind");
    resized();
}

void LoveMasterEditor::closeTriageCenter()
{
    triageOpen = false;
    setSize(MAIN_WIDTH, MAIN_HEIGHT);

    for (auto& tk : triageKnobs) {
        tk.slider.setVisible(false);
        tk.label.setVisible(false);
    }

    nerdsButton.setButtonText("for nerds");
    resized();
}

