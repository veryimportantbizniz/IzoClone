#include "PluginEditor.h"

LoveMasterEditor::LoveMasterEditor(LoveMasterProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
{
    setSize(BASE_WIDTH, BASE_HEIGHT);
    setResizable(true, true);
    setResizeLimits(320, 220, 1200, 800);

    // ── EKG Slider ────────────────────────────────────────────────────────────
    float initLove = processor.apvts.getRawParameterValue("love")->load() / 100.f;
    ekgSlider.value = initLove;
    ekgSlider.onValueChange = [this](float v) {
        if (auto* param = processor.apvts.getParameter("love"))
            param->setValueNotifyingHost(v);
    };
    addAndMakeVisible(ekgSlider);

    loveLabel.setText("L O V E", juce::dontSendNotification);
    loveLabel.setFont(juce::Font("Georgia", 15.f, juce::Font::bold | juce::Font::italic));
    loveLabel.setColour(juce::Label::textColourId, juce::Colour(0xffe8003a));
    loveLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(loveLabel);

    // ── Triage knobs ──────────────────────────────────────────────────────────
    for (int i = 0; i < 8; ++i)
    {
        auto& tk = triageKnobs[i];
        tk.slider.setSliderStyle(juce::Slider::RotaryVerticalDrag);
        tk.slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 56, 14);
        tk.slider.setColour(juce::Slider::rotarySliderFillColourId,    juce::Colour(0xffcc0033));
        tk.slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xffff6b8a));
        tk.slider.setColour(juce::Slider::textBoxTextColourId,         juce::Colour(0xff880020));
        tk.slider.setColour(juce::Slider::textBoxOutlineColourId,      juce::Colours::transparentBlack);
        tk.slider.setVisible(false);
        addChildComponent(tk.slider);

        tk.label.setText(triageLabels[i], juce::dontSendNotification);
        tk.label.setFont(juce::Font("Arial", 9.f, juce::Font::plain));
        tk.label.setColour(juce::Label::textColourId, juce::Colour(0xff880020));
        tk.label.setJustificationType(juce::Justification::centred);
        tk.label.setVisible(false);
        addChildComponent(tk.label);

        tk.attachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            processor.apvts, triageParamIDs[i], tk.slider);
    }

    // ── For Nerds button ──────────────────────────────────────────────────────
    nerdsButton.setButtonText("for nerds");
    nerdsButton.setColour(juce::TextButton::buttonColourId,   juce::Colours::transparentBlack);
    nerdsButton.setColour(juce::TextButton::textColourOffId,  juce::Colour(0xffbbbbbb));
    nerdsButton.setColour(juce::ComboBox::outlineColourId,    juce::Colours::transparentBlack);
    nerdsButton.onClick = [this]() {
        if (!triageOpen) openTriageCenter();
        else             closeTriageCenter();
    };
    addAndMakeVisible(nerdsButton);

    startTimerHz(60);
}

LoveMasterEditor::~LoveMasterEditor() { stopTimer(); }

// ── Layout — all proportional to current size ─────────────────────────────────
void LoveMasterEditor::resized()
{
    int w = getWidth();
    int h = getHeight();
    int mainW = triageOpen ? w - TRIAGE_WIDTH : w;

    // Title area height proportional
    int titleH = juce::jmax(28, h / 10);

    // EKG slider — centre of the main area, takes up most of the width
    int sliderH  = juce::jmax(80, h / 3);
    int sliderY  = titleH + (h - titleH - sliderH - 80) / 2;
    int sliderPad = mainW / 12;
    ekgSlider.setBounds(sliderPad, sliderY, mainW - sliderPad * 2, sliderH);

    // Label above slider
    loveLabel.setBounds(sliderPad, sliderY - 24, mainW - sliderPad * 2, 20);

    // For Nerds button — bottom left
    nerdsButton.setBounds(8, h - 24, 76, 18);

    // Triage panel
    if (triageOpen)
    {
        int triageX = w - TRIAGE_WIDTH;
        int cols = 4, kw = 60, kh = 70;
        int padX = (TRIAGE_WIDTH - cols * kw) / (cols + 1);
        int padY = titleH + 20;

        for (int i = 0; i < 8; ++i)
        {
            int col = i % cols;
            int row = i / cols;
            int x = triageX + padX + col * (kw + padX);
            int y = padY + row * (kh + 28);
            triageKnobs[i].slider.setBounds(x, y, kw, kh);
            triageKnobs[i].label.setBounds(x, y + kh, kw, 14);
        }
    }
}

// ── Paint ─────────────────────────────────────────────────────────────────────
void LoveMasterEditor::paint(juce::Graphics& g)
{
    int w = getWidth();
    int h = getHeight();
    int mainW = triageOpen ? w - TRIAGE_WIDTH : w;

    // Flat white
    g.fillAll(juce::Colours::white);

    // Triage panel
    if (triageOpen)
    {
        g.setColour(juce::Colour(0xfff8f0f2));
        g.fillRect(w - TRIAGE_WIDTH, 0, TRIAGE_WIDTH, h);
        g.setColour(juce::Colour(0xffff6b8a));
        g.drawLine((float)(w - TRIAGE_WIDTH), 0.f, (float)(w - TRIAGE_WIDTH), (float)h, 1.5f);

        float titleSize = juce::jmap((float)w, 320.f, 1200.f, 11.f, 16.f);
        g.setFont(juce::Font("Georgia", titleSize, juce::Font::bold | juce::Font::italic));
        g.setColour(juce::Colour(0xffe8003a));
        g.drawText("Triage Center", w - TRIAGE_WIDTH, 12, TRIAGE_WIDTH, 22,
                   juce::Justification::centred);
    }

    // Plugin title — scales with window
    float titleSize = juce::jmap((float)h, 220.f, 800.f, 16.f, 28.f);
    g.setFont(juce::Font("Georgia", titleSize, juce::Font::bold | juce::Font::italic));
    g.setColour(juce::Colour(0xffe8003a));
    g.drawText("LoveMaster", 0, 8, mainW, (int)titleSize + 8, juce::Justification::centred);

    // Waveform
    paintWaveform(g);

    // Spinning heart (top right)
    paintSpinningHeart(g);

    // Nerd Found
    if (nerdFoundVisible)
        paintNerdFoundOverlay(g);
}

void LoveMasterEditor::paintWaveform(juce::Graphics& g)
{
    int w = getWidth();
    int h = getHeight();
    int mainW = triageOpen ? w - TRIAGE_WIDTH : w;

    int waveH = juce::jmax(36, h / 7);
    int waveY = h - waveH - 30;
    int wavePad = mainW / 16;
    int waveW = mainW - wavePad * 2;

    g.setColour(juce::Colour(0xfff5e8eb));
    g.fillRoundedRectangle((float)(wavePad), (float)waveY,
                            (float)waveW, (float)waveH, 4.f);

    juce::Path wave;
    bool first = true;
    for (int x = 0; x < waveW; ++x)
    {
        int idx = x * 256 / waveW;
        float sample = waveformDisplay[juce::jlimit(0, 255, idx)];
        float cy = waveY + waveH * 0.5f - sample * waveH * 0.42f;
        cy = juce::jlimit((float)waveY, (float)(waveY + waveH), cy);
        if (first) { wave.startNewSubPath((float)(wavePad + x), cy); first = false; }
        else          wave.lineTo((float)(wavePad + x), cy);
    }

    float rms = 0.f;
    for (auto v : waveformDisplay) rms += v * v;
    rms = std::sqrt(rms / 256.f);
    float loud = juce::jlimit(0.f, 1.f, rms * 8.f);

    juce::Colour waveCol = juce::Colour(0xffffb3c1).interpolatedWith(juce::Colour(0xffaa0020), loud);
    g.setColour(waveCol);
    g.strokePath(wave, juce::PathStrokeType(1.6f));
}

void LoveMasterEditor::paintSpinningHeart(juce::Graphics& g)
{
    int mainW = triageOpen ? getWidth() - TRIAGE_WIDTH : getWidth();
    float cx = (float)mainW - 22.f;
    float cy = 18.f;
    float size = juce::jmap((float)getHeight(), 220.f, 800.f, 10.f, 18.f);

    juce::Graphics::ScopedSaveState save(g);
    g.addTransform(juce::AffineTransform::rotation(heartAngle, cx, cy));

    juce::Path heart;
    heart.startNewSubPath(cx, cy + size * 0.3f);
    heart.cubicTo(cx - size, cy - size * 0.5f,
                  cx - size * 1.5f, cy + size * 0.5f,
                  cx, cy + size);
    heart.cubicTo(cx + size * 1.5f, cy + size * 0.5f,
                  cx + size, cy - size * 0.5f,
                  cx, cy + size * 0.3f);

    float love = ekgSlider.value;
    g.setColour(juce::Colour(0xffffb3c1).interpolatedWith(juce::Colour(0xffe8003a), love));
    g.fillPath(heart);
}

void LoveMasterEditor::paintNerdFoundOverlay(juce::Graphics& g)
{
    int w = getWidth(), h = getHeight();
    juce::ColourGradient vignette(
        juce::Colours::black.withAlpha(0.85f * nerdFoundAlpha), w * 0.5f, h * 0.5f,
        juce::Colours::black.withAlpha(0.25f * nerdFoundAlpha), 0.f, 0.f,
        true);
    g.setGradientFill(vignette);
    g.fillAll();

    float fontSize = juce::jmap((float)w, 320.f, 1200.f, 28.f, 52.f);
    g.setFont(juce::Font("Georgia", fontSize, juce::Font::bold | juce::Font::italic));

    g.setColour(juce::Colour(0xff880020).withAlpha(nerdFoundAlpha * 0.5f));
    g.drawText("NERD FOUND", 3, h/2 - (int)(fontSize/2) - 2, w, (int)fontSize + 8,
               juce::Justification::centred);

    g.setColour(juce::Colour(0xffff6b8a).withAlpha(nerdFoundAlpha));
    g.drawText("NERD FOUND", 0, h/2 - (int)(fontSize/2) - 4, w, (int)fontSize + 8,
               juce::Justification::centred);
}

// ── Timer ─────────────────────────────────────────────────────────────────────
void LoveMasterEditor::timerCallback()
{
    // Sync EKG slider value from APVTS (in case host automation changes it)
    float love = processor.apvts.getRawParameterValue("love")->load() / 100.f;
    if (std::abs(love - ekgSlider.value) > 0.005f)
        ekgSlider.value = love;

    // Pulse amount drives heart glow
    heartPulse = heartPulse * 0.85f + love * 0.15f;
    ekgSlider.setPulse(heartPulse);

    // Spinning heart — speed proportional to love
    heartAngle += 0.006f + love * 0.055f;
    if (heartAngle > juce::MathConstants<float>::twoPi)
        heartAngle -= juce::MathConstants<float>::twoPi;

    // Waveform smoothing
    for (int i = 0; i < 256; ++i)
    {
        int srcIdx = (i * LoveMasterProcessor::WAVEFORM_SIZE / 256)
                     & (LoveMasterProcessor::WAVEFORM_SIZE - 1);
        float incoming = processor.getWaveformSample(srcIdx);
        waveformDisplay[i] = waveformDisplay[i] * 0.72f + incoming * 0.28f;
    }

    // Nerd Found fade
    if (nerdFoundVisible)
    {
        nerdFoundTimer++;
        if      (nerdFoundTimer < 15)  nerdFoundAlpha = (float)nerdFoundTimer / 15.f;
        else if (nerdFoundTimer > 75)  nerdFoundAlpha = 1.f - (float)(nerdFoundTimer - 75) / 15.f;
        else                           nerdFoundAlpha = 1.f;

        if (nerdFoundTimer > 90) { nerdFoundVisible = false; nerdFoundAlpha = 0.f; }
    }

    repaint();
}

// ── Triage open / close ───────────────────────────────────────────────────────
void LoveMasterEditor::openTriageCenter()
{
    triageOpen = true;
    setSize(getWidth() + TRIAGE_WIDTH, getHeight());
    for (auto& tk : triageKnobs) { tk.slider.setVisible(true); tk.label.setVisible(true); }

    nerdFoundVisible = true;
    nerdFoundAlpha   = 0.f;
    nerdFoundTimer   = 0;

    nerdsButton.setButtonText("nevermind");
    resized();
}

void LoveMasterEditor::closeTriageCenter()
{
    triageOpen = false;
    setSize(getWidth() - TRIAGE_WIDTH, getHeight());
    for (auto& tk : triageKnobs) { tk.slider.setVisible(false); tk.label.setVisible(false); }

    nerdsButton.setButtonText("for nerds");
    resized();
}
