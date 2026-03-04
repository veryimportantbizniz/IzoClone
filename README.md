# IzoClone VST3 Plugin

A professional mastering suite built with C++20 and JUCE 8.
Dark industrial aesthetic. Zero-allocation audio thread. All modules bypass-able independently.

## Signal Chain
``````
Input â†’ EQ (5-band) â†’ Multiband Compressor (3-band) â†’ Stereo Imager â†’ Exciter â†’ Limiter â†’ Output
``````

## Features
| Module | What it does |
|---|---|
| **EQ** | 5-band parametric with live FFT spectrum + real EQ curve overlay |
| **Compressor** | 3-band Linkwitz-Riley split, per-band threshold/ratio/attack/release/gain |
| **Stereo Imager** | M/S width 0â€“200%, mono bass below crossover, correlation meter |
| **Exciter** | Parallel harmonic saturation (tanh), animated harmonic display |
| **Limiter** | True-peak lookahead brick wall, colour-coded GR meter |
| **Meters** | Momentary / Short-term / Integrated LUFS + True Peak (ITU-R BS.1770-4) |
| **Presets** | 8 factory presets, save/load/delete user presets, prev/next navigation |
| **A/B Compare** | Snapshot & flip between two full plugin states instantly |
| **Auto-Gain** | Tracks integrated LUFS and nudges output gain toward your target |

## Build Instructions

### Prerequisites
- **Visual Studio 2022** with C++ Desktop workload
- **CMake 3.22+** â€” [cmake.org](https://cmake.org/download/)
- **Git**
- **NSIS** (for installer only) â€” [nsis.sourceforge.io](https://nsis.sourceforge.io)

### Build the plugin
``````bash
git clone https://github.com/yourname/IzoClone.git
cd IzoClone

# CMake fetches JUCE 8 automatically â€” no manual install needed
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
``````

The ``.vst3`` copies automatically to ``C:\Program Files\Common Files\VST3\``.
Restart FL Studio and it should appear under VST3 plugins.

### Build the installer
``````bash
# After building the plugin:
# 1. Right-click installer/IzoClone.nsi in Windows Explorer
# 2. Select "Compile NSIS Script"
# 3. Output: IzoClone_Setup_v1.0.0.exe
``````

### Code signing (free, self-signed)
``````powershell
# Run PowerShell as Administrator:
Set-ExecutionPolicy -Scope Process Bypass
.\installer\generate_selfsigned_cert.ps1
``````
This signs both the VST3 DLL and installer EXE. Windows will still show a SmartScreen
warning for other users â€” that's normal for self-signed certs. For public distribution,
use [SignPath.io](https://signpath.io) (free for open source).

## Project Structure
``````
IzoClone/
â”œâ”€â”€ CMakeLists.txt                    â† Build config, JUCE 8 auto-fetch, AVX2 flags
â”œâ”€â”€ installer/
â”‚   â”œâ”€â”€ IzoClone.nsi               â† NSIS installer script
â”‚   â””â”€â”€ generate_selfsigned_cert.ps1  â† Free self-signed code signing
â”œâ”€â”€ Source/
â”‚   â”œâ”€â”€ PluginProcessor.h/.cpp        â† Audio engine, full parameter tree, A/B, auto-gain
â”‚   â”œâ”€â”€ PluginEditor.h/.cpp           â† Main UI, 60Hz timer, meter wiring
â”‚   â”œâ”€â”€ Processors/
â”‚   â”‚   â”œâ”€â”€ EQProcessor               â† 5-band IIR + magnitude response curve
â”‚   â”‚   â”œâ”€â”€ CompressorProcessor       â† 3-band Linkwitz-Riley + per-band compression
â”‚   â”‚   â”œâ”€â”€ LimiterProcessor          â† Lookahead brick-wall limiter
â”‚   â”‚   â”œâ”€â”€ StereoImagerProcessor     â† M/S matrix + bass mono + correlation
â”‚   â”‚   â”œâ”€â”€ ExciterProcessor          â† Parallel tanh harmonic saturation
â”‚   â”‚   â””â”€â”€ MeterAndFFT               â† BS.1770-4 LUFS + FFT spectrum analyzer
â”‚   â”œâ”€â”€ UI/
â”‚   â”‚   â”œâ”€â”€ LookAndFeel               â† Dark/amber theme, custom knobs/toggles
â”‚   â”‚   â”œâ”€â”€ MainToolbar               â† Tabs, output gain, LUFS target, A/B, auto-gain
â”‚   â”‚   â”œâ”€â”€ PresetBar                 â† Preset browser with save/load/delete
â”‚   â”‚   â”œâ”€â”€ EQModule                  â† FFT spectrum + real EQ curve + 10 knobs
â”‚   â”‚   â”œâ”€â”€ CompressorModule          â† 3-band columns, GR meters
â”‚   â”‚   â”œâ”€â”€ LimiterModule             â† GR meter with colour gradient
â”‚   â”‚   â”œâ”€â”€ StereoModule              â† Stereo field arc, correlation bar
â”‚   â”‚   â”œâ”€â”€ ExciterModule             â† Animated harmonic bar display
â”‚   â”‚   â””â”€â”€ MeterModule               â† 4-column LUFS + true peak
â”‚   â””â”€â”€ Utils/
â”‚       â””â”€â”€ PresetManager             â† Save/load/delete presets, 8 factory presets
``````

## Performance Architecture
- **Zero-allocation audio thread** â€” all buffers pre-allocated in ``prepareToPlay()``
- **Lock-free UIâ†”audio bridge** â€” ``std::atomic<>`` for meters, ``AbstractFifo`` for FFT
- **Dirty-flag parameter updates** â€” filters only recalculate when params actually change  
- **AVX2 SIMD optimisation** â€” ``/arch:AVX2 /O2 /fp:fast`` on MSVC
- **Bypass fast-paths** â€” atomic bools skip entire processing chains instantly

