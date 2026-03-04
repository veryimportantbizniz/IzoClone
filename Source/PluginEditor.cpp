#include "PluginEditor.h"
#include "UI/LookAndFeel.h"
#include "UI/MainToolbar.h"
#include "UI/EQModule.h"
#include "UI/CompressorModule.h"
#include "UI/LimiterModule.h"
#include "UI/StereoModule.h"
#include "UI/ExciterModule.h"
#include "UI/MeterModule.h"
#include "UI/PresetBar.h"

IzoCloneEditor::IzoCloneEditor(IzoCloneProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    lookAndFeel = std::make_unique<IzoCloneLookAndFeel>();
    juce::LookAndFeel::setDefaultLookAndFeel(lookAndFeel.get());

    toolbar = std::make_unique<MainToolbar>(p.apvts,
        [this](int idx) { showModule(idx); },
        [this]()        { handleABButton(); },
        [this](bool on) { processorRef.setAutoGainEnabled(on); });
    addAndMakeVisible(*toolbar);

    presetBar = std::make_unique<PresetBar>(p.getPresetManager());
    addAndMakeVisible(*presetBar);

    eqModule      = std::make_unique<EQModule>        (p.apvts, p.getFFTAnalyzer(), p.getEQProcessor());
    compModule    = std::make_unique<CompressorModule> (p.apvts);
    stereoModule  = std::make_unique<StereoModule>    (p.apvts);
    exciterModule = std::make_unique<ExciterModule>   (p.apvts);
    limiterModule = std::make_unique<LimiterModule>   (p.apvts);
    meterModule   = std::make_unique<MeterModule>     (p.getLoudnessMeter());

    addChildComponent(*eqModule);
    addChildComponent(*compModule);
    addChildComponent(*stereoModule);
    addChildComponent(*exciterModule);
    addChildComponent(*limiterModule);
    addChildComponent(*meterModule);

    showModule(0);

    setResizable(true, true);
    setResizeLimits(880, 496, 1760, 990);
    getConstrainer()->setFixedAspectRatio(1100.0 / 620.0);
    setSize(1100, 620);

    startTimerHz(60);
}

IzoCloneEditor::~IzoCloneEditor()
{
    stopTimer();
    juce::LookAndFeel::setDefaultLookAndFeel(nullptr);
}

void IzoCloneEditor::paint(juce::Graphics& g)
{
    g.setGradientFill(juce::ColourGradient(
        juce::Colour(0xFF0D0D12), 0, 0,
        juce::Colour(0xFF15151E), 0, (float)getHeight(), false));
    g.fillAll();
}

void IzoCloneEditor::resized()
{
    auto area = getLocalBounds();
    toolbar->setBounds(area.removeFromTop(56));
    presetBar->setBounds(area.removeFromTop(34));

    for (auto* m : { (juce::Component*)eqModule.get(), compModule.get(),
                      stereoModule.get(), exciterModule.get(),
                      limiterModule.get(), meterModule.get() })
        m->setBounds(area);
}

void IzoCloneEditor::showModule(int index)
{
    activeModuleIndex = index;
    eqModule     ->setVisible(index == 0);
    compModule   ->setVisible(index == 1);
    stereoModule ->setVisible(index == 2);
    exciterModule->setVisible(index == 3);
    limiterModule->setVisible(index == 4);
    meterModule  ->setVisible(index == 5);
}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  A/B Comparison
//  Idle â†’ click â†’ Saves A, label changes to "SAVE B"
//  SavedA â†’ click â†’ Saves B, recalls A, label "â†’ B"
//  ComparingA â†’ click â†’ recalls B, label "â†’ A"
//  ComparingB â†’ click â†’ recalls A, label "â†’ B"
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void IzoCloneEditor::handleABButton()
{
    switch (abState)
    {
        case ABState::Idle:
            processorRef.snapshotToSlot(0);
            abState = ABState::SavedA;
            toolbar->setABLabel("SAVE B");
            break;
        case ABState::SavedA:
            processorRef.snapshotToSlot(1);
            processorRef.recallSlot(0);
            abState = ABState::ComparingA;
            toolbar->setABLabel("TO B");
            break;
        case ABState::ComparingA:
            processorRef.recallSlot(1);
            abState = ABState::ComparingB;
            toolbar->setABLabel("TO A");
            break;
        case ABState::ComparingB:
            processorRef.recallSlot(0);
            abState = ABState::ComparingA;
            toolbar->setABLabel("TO B");
            break;
    }
}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  60Hz timer - the bridge between audio thread and UI thread
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void IzoCloneEditor::timerCallback()
{
    // EQ spectrum always ticks (cheap, FFT only runs when ready)
    eqModule->refreshSpectrum();

    if (meterModule->isVisible())
        meterModule->refresh();

    if (compModule->isVisible())
    {
        auto& comp = processorRef.getCompProcessor();
        compModule->gainReductionValues[0] = comp.getGainReductionDb(0);
        compModule->gainReductionValues[1] = comp.getGainReductionDb(1);
        compModule->gainReductionValues[2] = comp.getGainReductionDb(2);
        compModule->refreshMeters();
    }

    if (limiterModule->isVisible())
    {
        limiterModule->currentGR = processorRef.getLimiterProcessor().getGainReductionDb();
        limiterModule->refreshGRMeter();
    }

    if (stereoModule->isVisible())
    {
        stereoModule->correlationValue = processorRef.getStereoProcessor().getCorrelation();
        stereoModule->repaint();
    }

    // Toolbar LUFS always visible
    toolbar->updateLUFSReadout(
        processorRef.getLoudnessMeter().getIntegratedLUFS(),
        processorRef.getLoudnessMeter().getTruePeakDb());
}

