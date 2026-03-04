#pragma once
#include <JuceHeader.h>
#include "LookAndFeel.h"
#include "../Utils/PresetManager.h"

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  PresetBar
//  A compact horizontal bar that sits above the module area:
//    [â—„] [â–¶]  [ Preset Name Combo â–¼ ]  [SAVE]  [DELETE]
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
class PresetBar : public juce::Component
{
public:
    PresetBar(PresetManager& pm) : presetManager(pm)
    {
        // Prev/Next arrows
        prevBtn.setButtonText("<");
        prevBtn.onClick = [this] { navigatePreset(-1); };
        addAndMakeVisible(prevBtn);

        nextBtn.setButtonText(">");
        nextBtn.onClick = [this] { navigatePreset(+1); };
        addAndMakeVisible(nextBtn);

        // Preset combo
        presetCombo.onChange = [this] {
            auto name = presetCombo.getText();
            if (name.isNotEmpty())
                presetManager.loadPreset(name);
        };
        addAndMakeVisible(presetCombo);

        // Save button
        saveBtn.setButtonText("SAVE");
        saveBtn.onClick = [this] { showSaveDialog(); };
        addAndMakeVisible(saveBtn);

        // Delete button
        deleteBtn.setButtonText("DEL");
        deleteBtn.onClick = [this] {
            auto name = presetCombo.getText();
            if (name.isNotEmpty() && presetManager.deletePreset(name))
                refreshList();
        };
        addAndMakeVisible(deleteBtn);

        refreshList();
    }

    void paint(juce::Graphics& g) override
    {
        g.setColour(juce::Colour(0xFF111120));
        g.fillAll();
        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_BG_CONTROL));
        g.drawLine(0, (float)getHeight() - 1, (float)getWidth(), (float)getHeight() - 1);

        g.setColour(juce::Colour(IzoCloneLookAndFeel::COL_TEXT_DIM));
        g.setFont(IzoCloneLookAndFeel::getLabelFont());
        g.drawText("PRESET", 8, 0, 50, getHeight(), juce::Justification::centredLeft);
    }

    void resized() override
    {
        int y = (getHeight() - 22) / 2;
        int x = 60;
        prevBtn.setBounds(x, y, 24, 22);   x += 28;
        nextBtn.setBounds(x, y, 24, 22);   x += 30;
        presetCombo.setBounds(x, y, 220, 22); x += 226;
        saveBtn.setBounds(x, y, 50, 22);   x += 56;
        deleteBtn.setBounds(x, y, 40, 22);
    }

    void refreshList()
    {
        auto names = presetManager.getAllPresetNames();
        presetCombo.clear(juce::dontSendNotification);
        int id = 1;
        for (auto& name : names)
            presetCombo.addItem(name, id++);

        // Select current
        auto current = presetManager.getCurrentPresetName();
        presetCombo.setText(current, juce::dontSendNotification);
    }

private:
    void navigatePreset(int direction)
    {
        auto names = presetManager.getAllPresetNames();
        if (names.isEmpty()) return;

        int current = presetCombo.getSelectedItemIndex();
        int next    = (current + direction + names.size()) % names.size();
        presetCombo.setSelectedItemIndex(next);
        presetManager.loadPreset(names[next]);
    }

    void showSaveDialog()
    {
        auto* dialog = new juce::AlertWindow("Save Preset",
                                              "Enter a name for this preset:",
                                              juce::MessageBoxIconType::NoIcon);
        dialog->addTextEditor("presetName", presetManager.getCurrentPresetName());
        dialog->addButton("Save",   1);
        dialog->addButton("Cancel", 0);

        dialog->enterModalState(true,
            juce::ModalCallbackFunction::create([this, dialog](int result) {
                if (result == 1) {
                    auto name = dialog->getTextEditorContents("presetName").trim();
                    if (name.isNotEmpty()) {
                        presetManager.savePreset(name);
                        refreshList();
                        presetCombo.setText(name, juce::dontSendNotification);
                    }
                }
                delete dialog;
            }), true);
    }

    PresetManager& presetManager;

    juce::TextButton prevBtn, nextBtn, saveBtn, deleteBtn;
    juce::ComboBox   presetCombo;
};

