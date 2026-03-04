#pragma once
#include <JuceHeader.h>

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  StereoImagerProcessor
//  Mid/Side width control with bass mono-ing below a crossover frequency
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
class StereoImagerProcessor
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec, juce::AudioProcessorValueTreeState& apvts);
    void process(juce::dsp::ProcessContextReplacing<float>& context, juce::AudioProcessorValueTreeState& apvts);

    float getCorrelation() const { return correlation.load(std::memory_order_relaxed); }

private:
    void updateParams(juce::AudioProcessorValueTreeState& apvts);

    // LP filter to extract low-end for mono bass
    juce::dsp::StateVariableTPTFilter<float> bassLPF;
    double sampleRate     = 44100.0;
    float  prevMonoLo     = -1.f;
    float  prevWidth      = -1.f;
    std::atomic<float> correlation { 1.0f };
};

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  ExciterProcessor
//  High-frequency harmonic saturation via soft-clipping on a parallel HF band
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
class ExciterProcessor
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec, juce::AudioProcessorValueTreeState& apvts);
    void process(juce::dsp::ProcessContextReplacing<float>& context, juce::AudioProcessorValueTreeState& apvts);

private:
    void updateFilter(juce::AudioProcessorValueTreeState& apvts);

    // HPF to isolate the frequency band we'll saturate
    juce::dsp::StateVariableTPTFilter<float> hpFilter;

    juce::AudioBuffer<float> sideChainBuffer;   // Holds HF band copy
    double sampleRate = 44100.0;
    float  prevFreq   = -1.f;

    // Soft clip: tanh-based waveshaper for musical harmonic generation
    static float softClip(float x, float drive)
    {
        float driven = x * (1.0f + drive * 0.1f);
        return std::tanh(driven) / std::tanh(1.0f + drive * 0.1f);
    }
};

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  CompressorProcessor (multiband stub â€” expandable)
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
class CompressorProcessor
{
public:
    void prepare(const juce::dsp::ProcessSpec& spec, juce::AudioProcessorValueTreeState& apvts);
    void process(juce::dsp::ProcessContextReplacing<float>& context, juce::AudioProcessorValueTreeState& apvts);

    float getGainReductionDb(int band) const {
        return gainReductionDb[juce::jlimit(0, 2, band)].load(std::memory_order_relaxed);
    }

private:
    static constexpr int NUM_BANDS = 3;

    // Crossover filters for 3-band split
    juce::dsp::LinkwitzRileyFilter<float> loSplitLP, loSplitHP;
    juce::dsp::LinkwitzRileyFilter<float> hiSplitLP, hiSplitHP;

    // One compressor per band
    juce::dsp::Compressor<float> compressors[NUM_BANDS];

    juce::AudioBuffer<float> bandBuffers[NUM_BANDS];
    std::atomic<float> gainReductionDb[NUM_BANDS];

    double sampleRate = 44100.0;
    float  prevCrossoverLo = -1.f, prevCrossoverHi = -1.f;

    void updateCrossovers(juce::AudioProcessorValueTreeState& apvts);
    void updateCompressorParams(juce::AudioProcessorValueTreeState& apvts);
};

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  LoudnessMeter â€” ITU-R BS.1770-4 LUFS measurement
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
class LoudnessMeter
{
public:
    void prepare(double sampleRate, int blockSize);
    void process(const juce::AudioBuffer<float>& buffer);

    float getIntegratedLUFS() const { return integratedLUFS.load(std::memory_order_relaxed); }
    float getShortTermLUFS()  const { return shortTermLUFS.load(std::memory_order_relaxed);  }
    float getMomentaryLUFS()  const { return momentaryLUFS.load(std::memory_order_relaxed);  }
    float getTruePeakDb()     const { return truePeakDb.load(std::memory_order_relaxed);     }

private:
    // K-weighting filters (pre-filter + RLB weighting)
    juce::dsp::ProcessorDuplicator<juce::dsp::IIR::Filter<float>,
                                    juce::dsp::IIR::Coefficients<float>> preFilter, weightFilter;
    double sampleRate   = 44100.0;
    float  squaredSum   = 0.0f;
    int    sampleCount  = 0;

    std::atomic<float> integratedLUFS { -70.0f };
    std::atomic<float> shortTermLUFS  { -70.0f };
    std::atomic<float> momentaryLUFS  { -70.0f };
    std::atomic<float> truePeakDb     { -70.0f };
};

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  FFTAnalyzer â€” pushes audio to an FFT and exposes magnitude spectrum for UI
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
class FFTAnalyzer
{
public:
    static constexpr int FFT_ORDER = 12;   // 4096-point FFT
    static constexpr int FFT_SIZE  = 1 << FFT_ORDER;

    void prepare(double sampleRate, int blockSize);
    void pushBuffer(const juce::AudioBuffer<float>& buffer);

    // UI thread: copy latest magnitude data (lock-free double buffer)
    bool getLatestSpectrum(std::vector<float>& output);

private:
    juce::dsp::FFT fft { FFT_ORDER };
    juce::dsp::WindowingFunction<float> window {
        (size_t)FFT_SIZE, juce::dsp::WindowingFunction<float>::hann };

    juce::AbstractFifo fifo { FFT_SIZE };
    std::array<float, FFT_SIZE>     fifoData    {};
    std::array<float, FFT_SIZE * 2> fftData     {};
    std::array<float, FFT_SIZE / 2> spectrumOut {};

    std::atomic<bool> nextFFTBlockReady { false };
    double sampleRate = 44100.0;
};

