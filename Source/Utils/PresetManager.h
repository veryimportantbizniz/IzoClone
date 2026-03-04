#pragma once
#include <JuceHeader.h>

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  PresetManager
//  Saves/loads named presets as XML files in:
//    Windows: Documents/IzoClone/Presets/
//  Ships with factory presets baked in as XML strings.
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
class PresetManager
{
public:
    explicit PresetManager(juce::AudioProcessorValueTreeState& apvts)
        : apvts(apvts)
    {
        userPresetsDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                             .getChildFile("IzoClone").getChildFile("Presets");
        userPresetsDir.createDirectory();
    }

    // â”€â”€ Save current state as a named preset â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void savePreset(const juce::String& name)
    {
        auto state  = apvts.copyState();
        auto xml    = state.createXml();
        xml->setAttribute("presetName", name);

        auto file = userPresetsDir.getChildFile(name + ".mfpreset");
        xml->writeTo(file);
    }

    // â”€â”€ Load preset from file â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    bool loadPreset(const juce::String& name)
    {
        // First look in user presets
        auto userFile = userPresetsDir.getChildFile(name + ".mfpreset");
        if (userFile.existsAsFile())
        {
            auto xml = juce::XmlDocument::parse(userFile);
            if (xml && xml->hasTagName(apvts.state.getType()))
            {
                apvts.replaceState(juce::ValueTree::fromXml(*xml));
                currentPresetName = name;
                return true;
            }
        }

        // Fall back to factory presets
        for (auto& fp : getFactoryPresets())
        {
            if (fp.name == name)
            {
                auto xml = juce::XmlDocument::parse(fp.xmlData);
                if (xml) apvts.replaceState(juce::ValueTree::fromXml(*xml));
                currentPresetName = name;
                return true;
            }
        }
        return false;
    }

    // â”€â”€ Get all preset names (factory first, then user) â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    juce::StringArray getAllPresetNames() const
    {
        juce::StringArray names;
        for (auto& fp : getFactoryPresets())
            names.add(fp.name);

        for (auto& f : userPresetsDir.findChildFiles(juce::File::findFiles, false, "*.mfpreset"))
            names.addIfNotAlreadyThere(f.getFileNameWithoutExtension());

        return names;
    }

    const juce::String& getCurrentPresetName() const { return currentPresetName; }

    // â”€â”€ Delete a user preset â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    bool deletePreset(const juce::String& name)
    {
        auto file = userPresetsDir.getChildFile(name + ".mfpreset");
        return file.deleteFile();
    }

    // â”€â”€ Export current state as a shareable .mfpreset file â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
    void exportPreset(const juce::String& name, const juce::File& destination)
    {
        auto state = apvts.copyState();
        auto xml   = state.createXml();
        xml->setAttribute("presetName", name);
        xml->writeTo(destination);
    }

private:
    struct FactoryPreset { juce::String name, xmlData; };

    // Factory presets â€” minimal XML stubs that set key parameters
    // In production these would be full APVTS state XMLs from tuned sessions
    static std::vector<FactoryPreset> getFactoryPresets()
    {
        return {
            { "Default (Flat)",        buildPreset("Default (Flat)",        0,0, 0,0,0, 0,0,0, 0,0, -18,-4,10,100, 80,100,0,0,4000,0, -1,1.5,50) },
            { "Streaming Master",      buildPreset("Streaming Master",      0,0, 0,0,0, 0,0,0, 0,0, -20,-4,10,100, 80,110,0,0,4000,10,-1,1.5,40) },
            { "Warm Vinyl",            buildPreset("Warm Vinyl",            80,2, 250,1.5f,0.8f, 0,-1,1.2f, 12000,-2, -22,-3,8,110, 80,95,0,20,3000,18,-2,2,60) },
            { "Podcast/Voice",         buildPreset("Podcast/Voice",         100,1, 300,2,1.2f, 3000,-2,2, 0,0, -16,-6,5,105, 80,100,0,0,5000,5,-1,1,30) },
            { "EDM Club Master",       buildPreset("EDM Club Master",       60,2.5f, 0,0,0, 0,1,0.8f, 10000,1, -16,-5,12,100, 80,100,0,10,5000,12,-0.3f,1,25) },
            { "Classical/Orchestral",  buildPreset("Classical/Orchestral",  0,0, 0,0,0, 0,0,0, 0,0, -23,-2,6,100, 80,100,0,0,4000,0,-1,2,80) },
            { "Hip-Hop/Trap",          buildPreset("Hip-Hop/Trap",          50,3, 0,0,0, 0,0,0, 10000,1.5f,-14,-6,15,105, 60,105,0,15,4000,18,-0.5f,1,20) },
            { "Loud & Punchy",         buildPreset("Loud & Punchy",         0,1, 0,0,0, 0,1,0.7f, 8000,2, -10,-8,15,100, 80,100,0,5,5000,15,-0.1f,0.5f,15) },
        };
    }

    static juce::String buildPreset(const char* name,
        float lsFreq, float lsGain,
        float lmFreq, float lmGain, float lmQ,
        float hmGain, float hmQ, float hmFreq,
        float hsFreq, float hsGain,
        float compThresh, float compRatio, float compAttack, float compRelease,
        float monoLo, float width,
        float excDrive, float excMix, float excFreq, float excColor,
        float limCeil, float limLook, float limRel)
    {
        // Build a minimal APVTS-compatible XML manually
        juce::XmlElement root("IzoClone");
        root.setAttribute("presetName", name);

        auto addParam = [&](const char* id, float val) {
            auto* p = root.createNewChildElement("PARAM");
            p->setAttribute("id", id);
            p->setAttribute("value", val);
        };

        addParam("eq_lowshelf_freq",  lsFreq > 0 ? lsFreq : 80.f);
        addParam("eq_lowshelf_gain",  lsGain);
        addParam("eq_low_mid_freq",   lmFreq > 0 ? lmFreq : 400.f);
        addParam("eq_low_mid_gain",   lmGain);
        addParam("eq_low_mid_q",      lmQ > 0 ? lmQ : 0.7f);
        addParam("eq_high_mid_freq",  hmFreq > 0 ? hmFreq : 3000.f);
        addParam("eq_high_mid_gain",  hmGain);
        addParam("eq_high_mid_q",     hmQ > 0 ? hmQ : 0.7f);
        addParam("eq_highshelf_freq", hsFreq > 0 ? hsFreq : 10000.f);
        addParam("eq_highshelf_gain", hsGain);
        addParam("comp_lo_thresh",    compThresh);
        addParam("comp_lo_ratio",     compRatio);
        addParam("comp_lo_attack",    compAttack);
        addParam("comp_lo_release",   compRelease);
        addParam("comp_mid_thresh",   compThresh + 2.f);
        addParam("comp_mid_ratio",    compRatio * 0.8f);
        addParam("comp_mid_attack",   compAttack * 1.2f);
        addParam("comp_mid_release",  compRelease);
        addParam("comp_hi_thresh",    compThresh + 4.f);
        addParam("comp_hi_ratio",     compRatio * 0.6f);
        addParam("comp_hi_attack",    compAttack * 0.5f);
        addParam("comp_hi_release",   compRelease * 0.7f);
        addParam("stereo_width",      width);
        addParam("stereo_mono_lo",    monoLo);
        addParam("exciter_drive",     excDrive);
        addParam("exciter_mix",       excMix);
        addParam("exciter_freq",      excFreq);
        addParam("exciter_color",     excColor);
        addParam("limiter_ceiling",   limCeil);
        addParam("limiter_lookahead", limLook);
        addParam("limiter_release",   limRel);

        return root.toString();
    }

    juce::AudioProcessorValueTreeState& apvts;
    juce::File userPresetsDir;
    juce::String currentPresetName = "Default (Flat)";
};

