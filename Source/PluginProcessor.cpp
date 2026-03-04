#include "PluginProcessor.h"
#include "PluginEditor.h"

juce::AudioProcessorValueTreeState::ParameterLayout LoveMasterProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // â”€â”€ The one knob that matters â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "love", "Love", juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 50.f));

    // â”€â”€ Triage Center (nerd panel) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "triage_ceiling",   "Ceiling",       juce::NormalisableRange<float>(-30.f, 0.f,  0.1f), -0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "triage_lowcut",    "Low Cut Hz",    juce::NormalisableRange<float>(10.f,  200.f, 1.f, 0.4f), 20.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "triage_midcut",    "Mid Cut dB",    juce::NormalisableRange<float>(-12.f, 0.f,  0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "triage_output",    "Output Gain",   juce::NormalisableRange<float>(-24.f, 12.f, 0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "triage_release",   "Release ms",    juce::NormalisableRange<float>(10.f,  500.f, 1.f, 0.5f), 80.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "triage_attack",    "Attack ms",     juce::NormalisableRange<float>(0.1f,  100.f, 0.1f, 0.5f), 10.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "triage_width",     "Stereo Width",  juce::NormalisableRange<float>(0.f,   200.f, 0.1f), 100.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "triage_excfreq",   "Exciter Freq",  juce::NormalisableRange<float>(1000.f,16000.f,1.f, 0.3f), 4000.f));

    // â”€â”€ Hidden internal params (Love drives these) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "eq_lowshelf_freq", "LS Freq",  juce::NormalisableRange<float>(20.f,   500.f,  1.f, 0.3f), 80.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "eq_lowshelf_gain", "LS Gain",  juce::NormalisableRange<float>(-12.f,  12.f,   0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "eq_low_mid_freq",  "LM Freq",  juce::NormalisableRange<float>(200.f,  2000.f, 1.f, 0.3f), 350.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "eq_low_mid_gain",  "LM Gain",  juce::NormalisableRange<float>(-12.f,  12.f,   0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "eq_low_mid_q",     "LM Q",     juce::NormalisableRange<float>(0.1f,   10.f,   0.01f, 0.5f), 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "eq_high_mid_freq", "HM Freq",  juce::NormalisableRange<float>(1000.f, 10000.f,1.f, 0.3f), 3000.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "eq_high_mid_gain", "HM Gain",  juce::NormalisableRange<float>(-12.f,  12.f,   0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "eq_high_mid_q",    "HM Q",     juce::NormalisableRange<float>(0.1f,   10.f,   0.01f, 0.5f), 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "eq_highshelf_freq","HS Freq",  juce::NormalisableRange<float>(2000.f, 20000.f,1.f, 0.3f), 10000.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "eq_highshelf_gain","HS Gain",  juce::NormalisableRange<float>(-12.f,  12.f,   0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("eq_bypass", "EQ Bypass", false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "comp_crossover_lo","XO Lo",    juce::NormalisableRange<float>(60.f,   500.f,  1.f, 0.4f), 150.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "comp_crossover_hi","XO Hi",    juce::NormalisableRange<float>(1000.f, 12000.f,1.f, 0.4f), 5000.f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("comp_bypass","Comp Bypass", false));

    for (auto& band : { "lo", "mid", "hi" }) {
        juce::String b(band);
        params.push_back(std::make_unique<juce::AudioParameterFloat>("comp_"+b+"_thresh", b+" Thresh", juce::NormalisableRange<float>(-60.f,0.f,0.1f), -18.f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("comp_"+b+"_ratio",  b+" Ratio",  juce::NormalisableRange<float>(1.f,20.f,0.1f,0.5f), 4.f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("comp_"+b+"_attack",  b+" Attack", juce::NormalisableRange<float>(0.1f,200.f,0.1f,0.5f), 10.f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("comp_"+b+"_release", b+" Release",juce::NormalisableRange<float>(10.f,2000.f,1.f,0.5f), 100.f));
        params.push_back(std::make_unique<juce::AudioParameterFloat>("comp_"+b+"_gain",    b+" Gain",   juce::NormalisableRange<float>(-12.f,12.f,0.1f), 0.f));
    }

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "stereo_width",    "Width",     juce::NormalisableRange<float>(0.f, 200.f, 0.1f), 100.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "stereo_mono_lo",  "Mono Lo",   juce::NormalisableRange<float>(20.f, 300.f, 1.f, 0.4f), 80.f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("stereo_bypass","Stereo Bypass", false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "exciter_drive",   "Exc Drive", juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "exciter_freq",    "Exc Freq",  juce::NormalisableRange<float>(1000.f,16000.f,1.f,0.3f), 4000.f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "exciter_mix",     "Exc Mix",   juce::NormalisableRange<float>(0.f, 100.f, 0.1f), 0.f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("exciter_bypass","Exciter Bypass", false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "limiter_ceiling", "Ceiling",   juce::NormalisableRange<float>(-30.f, 0.f, 0.1f), -0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "limiter_lookahead","Lookahead",juce::NormalisableRange<float>(0.f, 10.f, 0.1f), 1.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "limiter_release", "Lim Rel",   juce::NormalisableRange<float>(1.f, 1000.f, 0.1f, 0.5f), 50.f));
    params.push_back(std::make_unique<juce::AudioParameterBool>("limiter_bypass","Limiter Bypass", false));

    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "output_gain",     "Output",    juce::NormalisableRange<float>(-24.f, 12.f, 0.1f), 0.f));

    return { params.begin(), params.end() };
}

LoveMasterProcessor::LoveMasterProcessor()
    : AudioProcessor(BusesProperties()
        .withInput ("Input",  juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "LoveMaster", createParameterLayout())
{
    for (auto& s : waveformRing) s.store(0.f, std::memory_order_relaxed);
}

LoveMasterProcessor::~LoveMasterProcessor() {}

void LoveMasterProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
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

    // Mud bandpass: centre ~350hz, Q 1.2
    juce::dsp::ProcessSpec monoSpec{ sampleRate, (juce::uint32)samplesPerBlock, 1 };
    mudBandpass.prepare(monoSpec);
    mudBandpass.setType(juce::dsp::StateVariableTPTFilterType::bandpass);
    mudBandpass.setCutoffFrequency(350.f);
    mudBandpass.setResonance(1.2f);
    mudFilterReady = true;
}

void LoveMasterProcessor::releaseResources() {}

// â”€â”€ Love macro â€” maps 0-100 to all underlying DSP params â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void LoveMasterProcessor::applyLoveMacro(float love)
{
    const float t = love / 100.f;  // normalise to 0-1

    auto set = [&](const char* id, float val) {
        if (auto* p = apvts.getRawParameterValue(id)) p->store(val, std::memory_order_relaxed);
    };

    // Triage overrides for ceiling / width / exciter freq
    float ceiling  = apvts.getRawParameterValue("triage_ceiling") ->load();
    float width    = apvts.getRawParameterValue("triage_width")   ->load();
    float excFreq  = apvts.getRawParameterValue("triage_excfreq") ->load();
    float attack   = apvts.getRawParameterValue("triage_attack")  ->load();
    float release  = apvts.getRawParameterValue("triage_release") ->load();
    float midcut   = apvts.getRawParameterValue("triage_midcut")  ->load();
    float lowcut   = apvts.getRawParameterValue("triage_lowcut")  ->load();

    // â”€â”€ EQ â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    set("eq_lowshelf_gain",  juce::jmap(t, 0.f, 2.5f));           // warm low shelf
    set("eq_highshelf_gain", juce::jmap(t, 0.f, 1.8f));           // gentle air
    set("eq_high_mid_gain",  juce::jmap(t, 0.f, -1.5f));          // de-harsh mids
    set("eq_low_mid_freq",   350.f);                               // mud zone
    set("eq_low_mid_gain",   midcut);                              // triage override
    set("eq_low_mid_q",      juce::jmap(t, 0.5f, 1.8f));

    // Low cut from triage
    set("eq_lowshelf_freq",  lowcut);

    // â”€â”€ Compressor â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    float compAmount = juce::jmap(t, -30.f, -16.f);               // thresh scales with love
    set("comp_lo_thresh",  compAmount - 4.f);
    set("comp_mid_thresh", compAmount);
    set("comp_hi_thresh",  compAmount + 2.f);
    set("comp_lo_ratio",   juce::jmap(t, 1.5f, 4.f));
    set("comp_mid_ratio",  juce::jmap(t, 1.5f, 3.5f));
    set("comp_hi_ratio",   juce::jmap(t, 1.2f, 2.5f));
    set("comp_lo_attack",  attack);
    set("comp_mid_attack", attack);
    set("comp_hi_attack",  attack);
    set("comp_lo_release", release);
    set("comp_mid_release",release);
    set("comp_hi_release", release);

    // â”€â”€ Stereo â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    // Love adds a touch of width, triage can override
    float loveWidth = juce::jmap(t, 100.f, 125.f);
    set("stereo_width", width > 100.f ? width : loveWidth);

    // â”€â”€ Exciter â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    set("exciter_drive", juce::jmap(t, 0.f, 35.f));
    set("exciter_mix",   juce::jmap(t, 0.f, 30.f));
    set("exciter_freq",  excFreq);

    // â”€â”€ Limiter â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    set("limiter_ceiling", ceiling);
    set("limiter_bypass",  0.f);   // always on

    // â”€â”€ Output â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    float outputGain = apvts.getRawParameterValue("triage_output")->load();
    set("output_gain", outputGain);
}

// â”€â”€ Auto mud removal â€” always running, invisible â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void LoveMasterProcessor::applyAutoMudRemoval(juce::AudioBuffer<float>& buffer)
{
    if (!mudFilterReady) return;

    // Measure energy in the mud band (200-500hz proxy via bandpass)
    // Mix a small amount of gain reduction in that band if mud detected
    const int numSamples = buffer.getNumSamples();
    float bandEnergy = 0.f, totalEnergy = 0.f;

    for (int ch = 0; ch < buffer.getNumChannels(); ++ch) {
        const float* data = buffer.getReadPointer(ch);
        for (int i = 0; i < numSamples; ++i) {
            float s = data[i] * data[i];
            totalEnergy += s;
        }
    }

    if (totalEnergy < 0.00001f) return;

    // Smooth mud energy estimate
    float newMud = juce::jlimit(0.f, 1.f, (totalEnergy > 0.f) ? mudEnergy / (totalEnergy + 0.0001f) : 0.f);
    mudEnergy = mudEnergy * 0.95f + newMud * 0.05f;

    // If mud is high, apply gentle gain reduction in mid band via EQ param
    // (the EQ processor reads this on the next block)
    float mudCut = juce::jmap(juce::jlimit(0.f, 1.f, mudEnergy * 3.f), 0.f, -3.f);
    if (auto* p = apvts.getRawParameterValue("eq_low_mid_gain"))
    {
        float current = p->load();
        float triageMidCut = apvts.getRawParameterValue("triage_midcut")->load();
        // Only auto-cut beyond what triage already set
        float autoCut = std::min(current, triageMidCut + mudCut);
        p->store(autoCut, std::memory_order_relaxed);
    }
}

void LoveMasterProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                        juce::MidiBuffer& /*midi*/)
{
    juce::ScopedNoDenormals noDenormals;

    auto totalIn  = getTotalNumInputChannels();
    auto totalOut = getTotalNumOutputChannels();
    for (auto i = totalIn; i < totalOut; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Apply Love macro â€” translates knob to all DSP params
    float love = apvts.getRawParameterValue("love")->load();
    applyLoveMacro(love);

    // Auto mud removal
    applyAutoMudRemoval(buffer);

    // Write waveform to ring buffer for UI
    {
        const int numSamples = buffer.getNumSamples();
        int writePos = waveformWritePos.load(std::memory_order_relaxed);
        for (int i = 0; i < numSamples; ++i)
        {
            float sample = 0.f;
            for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
                sample += buffer.getSample(ch, i);
            sample /= (float)buffer.getNumChannels();
            waveformRing[writePos & (WAVEFORM_SIZE-1)].store(sample, std::memory_order_relaxed);
            writePos = (writePos + 1) & (WAVEFORM_SIZE - 1);
        }
        waveformWritePos.store(writePos, std::memory_order_relaxed);
    }

    // Signal chain
    fftAnalyzer.pushBuffer(buffer);
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    eqProcessor.process(context, apvts);
    compProcessor.process(context, apvts);
    stereoProcessor.process(context, apvts);
    exciterProcessor.process(context, apvts);
    limiterProcessor.process(context, apvts);

    float outGain = apvts.getRawParameterValue("output_gain")->load();
    buffer.applyGain(juce::Decibels::decibelsToGain(outGain));

    loudnessMeter.process(buffer);
}

void LoveMasterProcessor::parameterChanged(const juce::String&, float) {}

void LoveMasterProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void LoveMasterProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

juce::AudioProcessorEditor* LoveMasterProcessor::createEditor()
{
    return new LoveMasterEditor(*this);
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new LoveMasterProcessor();
}

