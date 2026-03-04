cmake_minimum_required(VERSION 3.22)
project(IzoClone VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ─── Fetch JUCE ───────────────────────────────────────────────────────────────
include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG        8.0.4
)
FetchContent_MakeAvailable(JUCE)

# ─── Plugin Target ────────────────────────────────────────────────────────────
juce_add_plugin(IzoClone
    COMPANY_NAME            "YourStudio"
    PLUGIN_MANUFACTURER_CODE Ystu
    PLUGIN_CODE             Mfrg
    FORMATS                 VST3 Standalone
    PRODUCT_NAME            "IzoClone"
    IS_SYNTH                FALSE
    NEEDS_MIDI_INPUT        FALSE
    NEEDS_MIDI_OUTPUT       FALSE
    IS_MIDI_EFFECT          FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD TRUE
    VST3_CATEGORIES         "Mastering" "Dynamics" "EQ"
)

# ─── Sources ──────────────────────────────────────────────────────────────────
target_sources(IzoClone PRIVATE
    Source/PluginProcessor.cpp
    Source/PluginEditor.cpp

    # DSP Processors
    Source/Processors/EQProcessor.cpp
    Source/Processors/CompressorProcessor.cpp
    Source/Processors/LimiterProcessor.cpp
    Source/Processors/StereoImagerProcessor.cpp
    Source/Processors/ExciterProcessor.cpp
    Source/Processors/MeterAndFFT.cpp

    # UI Modules (header-only components)
    Source/UI/LookAndFeel.cpp

    # Utils
    Source/Utils/PresetManager.cpp
)

# ─── Compile Definitions ──────────────────────────────────────────────────────
target_compile_definitions(IzoClone PUBLIC
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
    JUCE_DISPLAY_SPLASH_SCREEN=0
    JUCE_REPORT_APP_USAGE=0
)

# ─── JUCE Modules ─────────────────────────────────────────────────────────────
target_link_libraries(IzoClone PRIVATE
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

# ─── Optimisation Flags ───────────────────────────────────────────────────────
if(MSVC)
    target_compile_options(IzoClone PRIVATE /O2 /arch:AVX2 /fp:fast)
else()
    target_compile_options(IzoClone PRIVATE -O3 -mavx2 -ffast-math)
endif()

