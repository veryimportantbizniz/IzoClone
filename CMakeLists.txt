cmake_minimum_required(VERSION 3.22)
project(LoveMaster VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
)
FetchContent_MakeAvailable(JUCE)

juce_add_plugin(LoveMaster
    COMPANY_NAME                "LoveMaster"
    PLUGIN_MANUFACTURER_CODE    Lvms
    PLUGIN_CODE                 Lvm1
    FORMATS                     VST3 Standalone
    PRODUCT_NAME                "LoveMaster"
    IS_SYNTH                    FALSE
    NEEDS_MIDI_INPUT            FALSE
    NEEDS_MIDI_OUTPUT           FALSE
    IS_MIDI_EFFECT              FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD     FALSE
    VST3_CATEGORIES             "Mastering" "Dynamics" "EQ"
)

target_sources(LoveMaster PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp
    Source/Processors/EQProcessor.cpp
    Source/Processors/CompressorProcessor.cpp
    Source/Processors/LimiterProcessor.cpp
    Source/Processors/StereoImagerProcessor.cpp
    Source/Processors/ExciterProcessor.cpp
    Source/Processors/MeterAndFFT.cpp
    Source/UI/LookAndFeel.cpp
    Source/Utils/PresetManager.cpp
)

target_compile_definitions(LoveMaster PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
)

target_link_libraries(LoveMaster PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_dsp
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
)

juce_generate_juce_header(LoveMaster)

if(MSVC)
    target_compile_options(LoveMaster PRIVATE /O2 /arch:AVX2 /fp:fast)
else()
    target_compile_options(LoveMaster PRIVATE -O3 -mavx2 -ffast-math)
endif()

