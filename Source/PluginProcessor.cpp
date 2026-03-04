#include "PluginProcessor.h"
#include "PluginEditor.h"

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  Parameter Layout
//  All plugin parameters are declared here and automatically synced to UI
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
juce::AudioProcessorValueTreeState::ParameterLayout IzoCloneProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // â”€â”€ EQ â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    params.push_back(std::make_unique<juce::AudioParameterBool>  ("eq_bypass",       "EQ Bypass",          false));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("eq_lowshelf_freq", "Low Shelf Freq",     juce::NormalisableRange<float>(20.f,   500.f,  1.f, 0.3f), 80.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("eq_lowshelf_gain", "Low Shelf Gain",     juce::NormalisableRange<float>(-12.f,  12.f,   0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("eq_low_mid_freq",  "Low Mid Freq",       juce::NormalisableRange<float>(200.f,  2000.f, 1.f, 0.3f), 400.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("eq_low_mid_gain",  "Low Mid Gain",       juce::NormalisableRange<float>(-12.f,  12.f,   0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("eq_low_mid_q",     "Low Mid Q",          juce::NormalisableRange<float>(0.1f,   10.f,   0.01f, 0.5f), 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("eq_high_mid_freq", "High Mid Freq",      juce::NormalisableRange<float>(1000.f, 10000.f,1.f, 0.3f), 3000.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("eq_high_mid_gain", "High Mid Gain",      juce::NormalisableRange<float>(-12.f,  12.f,   0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("eq_high_mid_q",    "High Mid Q",         juce::NormalisableRange<float>(0.1f,   10.f,   0.01f, 0.5f), 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("eq_highshelf_freq","High Shelf Freq",    juce::NormalisableRange<float>(2000.f, 20000.f,1.f, 0.3f), 10000.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("eq_highshelf_gain","High Shelf Gain",    juce::NormalisableRange<float>(-12.f,  12.f,   0.1f), 0.f));

    // â”€â”€ Multiband Compressor â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    params.push_back(std::make_unique<juce::AudioParameterBool>  ("comp_bypass",      "Comp Bypass",        false));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("comp_crossover_lo","Crossover Low",      juce::NormalisableRange<float>(60.f,   500.f,  1.f, 0.4f), 150.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("comp_crossover_hi","Crossover High",     juce::NormalisableRange<float>(1000.f, 12000.f,1.f, 0.4f), 5000.f));
    for (auto& band : { "lo", "mid", "hi" }) {
        juce::String b(band);
        params.push_back(std::make_unique<juce::AudioParameterFloat>("comp_" + b + "_thresh", b + " Thresh", juce::NormalisableRange<float>(-60.f, 0.f, 0.1f), -18.f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("comp_" + b + "_ratio",  b + " Ratio",  juce::NormalisableRange<float>(1.f,   20.f, 0.1f, 0.5f), 4.f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("comp_" + b + "_attack",  b + " Attack", juce::NormalisableRange<float>(0.1f,  200.f, 0.1f, 0.5f), 10.f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("comp_" + b + "_release", b + " Release",juce::NormalisableRange<float>(10.f,  2000.f,1.f,  0.5f), 100.f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("comp_" + b + "_gain",    b + " Gain",   juce::NormalisableRange<float>(-12.f,  12.f, 0.1f), 0.f));
    }

    // â”€â”€ Stereo Imager â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    params.push_back(std::make_unique<juce::AudioParameterBool>  ("stereo_bypass",   "Stereo Bypass",      false));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("stereo_width",    "Stereo Width",       juce::NormalisableRange<float>(0.f,   200.f,  0.1f), 100.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("stereo_mono_lo",  "Mono Below (Hz)",    juce::NormalisableRange<float>(20.f,  300.f,  1.f, 0.4f), 80.f));

    // â”€â”€ Exciter â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    params.push_back(std::make_unique<juce::AudioParameterBool>  ("exciter_bypass",  "Exciter Bypass",     false));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("exciter_drive",   "Exciter Drive",      juce::NormalisableRange<float>(0.f,   100.f,  0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("exciter_freq",    "Exciter Freq",       juce::NormalisableRange<float>(1000.f,16000.f,1.f, 0.3f), 4000.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("exciter_mix",     "Exciter Mix",        juce::NormalisableRange<float>(0.f,   100.f,  0.1f), 0.f));

    // â”€â”€ Limiter â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    params.push_back(std::make_unique<juce::AudioParameterBool>  ("limiter_bypass",  "Limiter Bypass",     false));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("limiter_ceiling", "Ceiling",            juce::NormalisableRange<float>(-6.f,   0.f,   0.1f), -1.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("limiter_lookahead","Lookahead (ms)",    juce::NormalisableRange<float>(0.f,    10.f,  0.1f), 1.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("limiter_release", "Limiter Release",    juce::NormalisableRange<float>(1.f,    1000.f,0.1f, 0.5f), 50.f));

    // â”€â”€ Output â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    params.push_back(std::make_unique<juce::AudioParameterFloat> ("output_gain",     "Output Gain",        juce::NormalisableRange<float>(-24.f,  12.f,  0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterChoice>("target_lufs",     "Target LUFS",        juce::StringArray{"-6","-8","-10","-14","-16","-18","-23","Off"}, 3));

    return { params.begin(), params.end() };
}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
IzoCloneProcessor::IzoCloneProcessor()
    : AudioProcessor(BusesProperties()
        .withInput ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "IzoClone", createParameterLayout())
{
    // Register as listener for bypass params so we can fast-path processBlock
    apvts.addParameterListener("eq_bypass",      this);
    apvts.addParameterListener("comp_bypass",    this);
    apvts.addParameterListener("stereo_bypass",  this);
    apvts.addParameterListener("exciter_bypass", this);
    apvts.addParameterListener("limiter_bypass", this);
}

IzoCloneProcessor::~IzoCloneProcessor()
{
    apvts.removeParameterListener("eq_bypass",      this);
    apvts.removeParameterListener("comp_bypass",    this);
    apvts.removeParameterListener("stereo_bypass",  this);
    apvts.removeParameterListener("exciter_bypass", this);
    apvts.removeParameterListener("limiter_bypass", this);
}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void IzoCloneProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    currentBlockSize  = samplesPerBlock;

    juce::dsp::ProcessSpec spec{ sampleRate, (juce::uint32)samplesPerBlock, 2 };

    eqProcessor.prepare(spec, apvts);
    compProcessor.prepare(spec, apvts);
    stereoProcessor.prepare(spec, apvts);
    exciterProcessor.prepare(spec, apvts);
    limiterProcessor.prepare(spec, apvts);
    loudnessMeter.prepare(sampleRate, samplesPerBlock);
    fftAnalyzer.prepare(sampleRate, samplesPerBlock);
}

void IzoCloneProcessor::releaseResources() {}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  processBlock â€” THE HOT PATH
//  Rules: zero allocations, no locks, no file I/O
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void IzoCloneProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                         juce::MidiBuffer& /*midi*/)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Clear any output channels that aren't receiving input data
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Feed pre-processing signal to FFT analyzer (for display)
    fftAnalyzer.pushBuffer(buffer);

    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    if (!eqBypassed.load(std::memory_order_relaxed))
        eqProcessor.process(context, apvts);

    if (!compBypassed.load(std::memory_order_relaxed))
        compProcessor.process(context, apvts);

    if (!stereoBypassed.load(std::memory_order_relaxed))
        stereoProcessor.process(context, apvts);

    if (!exciterBypassed.load(std::memory_order_relaxed))
        exciterProcessor.process(context, apvts);

    if (!limiterBypassed.load(std::memory_order_relaxed))
        limiterProcessor.process(context, apvts);

    // Apply output gain
    auto outputGainDb = apvts.getRawParameterValue("output_gain")->load(std::memory_order_relaxed);

    // LUFS auto-gain: nudge output gain toward target LUFS
    if (autoGainEnabled.load(std::memory_order_relaxed))
    {
        static const float targetTable[] = { -6,-8,-10,-14,-16,-18,-23, -999 };
        int targetIdx = (int)apvts.getRawParameterValue("target_lufs")->load();
        float targetLUFS = targetTable[juce::jlimit(0, 7, targetIdx)];

        if (targetLUFS > -100.f)
        {
            float currentLUFS = loudnessMeter.getShortTermLUFS();
            if (currentLUFS > -69.f)  // Only adjust if we have signal
            {
                float errorDb  = targetLUFS - currentLUFS;
                // Slow attack (don't react to single loud transients)
                float coeff    = std::exp(-1.0f / (2.0f * (float)currentSampleRate / buffer.getNumSamples()));
                autoGainSmoothDb = coeff * autoGainSmoothDb + (1.f - coeff) * errorDb;
                float clampedAdjust = juce::jlimit(-12.f, 12.f, autoGainSmoothDb);
                outputGainDb += clampedAdjust;
            }
        }
    }

    buffer.applyGain(juce::Decibels::decibelsToGain(outputGainDb));

    // LUFS metering (lock-free write to circular buffer, UI reads asynchronously)
    loudnessMeter.process(buffer);
}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void IzoCloneProcessor::parameterChanged(const juce::String& id, float newValue)
{
    if      (id == "eq_bypass")      eqBypassed     .store(newValue > 0.5f, std::memory_order_relaxed);
    else if (id == "comp_bypass")    compBypassed   .store(newValue > 0.5f, std::memory_order_relaxed);
    else if (id == "stereo_bypass")  stereoBypassed .store(newValue > 0.5f, std::memory_order_relaxed);
    else if (id == "exciter_bypass") exciterBypassed.store(newValue > 0.5f, std::memory_order_relaxed);
    else if (id == "limiter_bypass") limiterBypassed.store(newValue > 0.5f, std::memory_order_relaxed);
}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void IzoCloneProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void IzoCloneProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorEditor* IzoCloneProcessor::createEditor()
{
    return new IzoCloneEditor(*this);
}

juce::AudioProcessorEditor* IzoCloneProcessor::createEditor()
{
    return new IzoCloneEditor(*this);
}

// â”€â”€ A/B Comparison â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void IzoCloneProcessor::snapshotToSlot(int slot)
{
    jassert(slot == 0 || slot == 1);
    getStateInformation(abSlots[slot]);
}

void IzoCloneProcessor::recallSlot(int slot)
{
    jassert(slot == 0 || slot == 1);
    if (!abSlots[slot].isEmpty())
        setStateInformation(abSlots[slot].getData(), (int)abSlots[slot].getSize());
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new IzoCloneProcessor();
}

