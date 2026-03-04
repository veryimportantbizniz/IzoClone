#include "AllProcessors.h"

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  LoudnessMeter â€” ITU-R BS.1770-4
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void LoudnessMeter::prepare(double sr, int blockSize)
{
    sampleRate = sr;
    squaredSum = 0.f;
    sampleCount = 0;

    juce::dsp::ProcessSpec spec{ sr, (juce::uint32)blockSize, 2 };

    // K-weighting pre-filter (high-shelf ~+4dB above 1kHz)
    preFilter.prepare(spec);
    *preFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighShelf(
        sr, 1681.97f, 0.7071f, juce::Decibels::decibelsToGain(3.999843f));

    // K-weighting RLB filter (high-pass ~40Hz)
    weightFilter.prepare(spec);
    *weightFilter.state = *juce::dsp::IIR::Coefficients<float>::makeHighPass(
        sr, 38.13547f, 0.5003270f);
}

void LoudnessMeter::process(const juce::AudioBuffer<float>& buffer)
{
    // We process a copy so we don't touch the main signal
    juce::AudioBuffer<float> kWeighted(buffer);

    {
        juce::dsp::AudioBlock<float> block(kWeighted);
        juce::dsp::ProcessContextReplacing<float> ctx(block);
        preFilter.process(ctx);
        weightFilter.process(ctx);
    }

    int numSamps = buffer.getNumSamples();
    int numCh    = juce::jmin(buffer.getNumChannels(), 2);

    // Mean square across channels
    float ms = 0.f;
    for (int ch = 0; ch < numCh; ++ch) {
        auto* data = kWeighted.getReadPointer(ch);
        for (int n = 0; n < numSamps; ++n)
            ms += data[n] * data[n];
    }
    ms /= (float)(numSamps * numCh);

    // Accumulate for integrated LUFS (simple ungated version)
    squaredSum  += ms * numSamps;
    sampleCount += numSamps;

    // Momentary (400ms window â‰ˆ use current block as approximation)
    float momentary = (ms > 0.f) ? -0.691f + 10.f * std::log10(ms) : -70.f;
    momentaryLUFS.store(momentary, std::memory_order_relaxed);

    // Short-term (3s window â€” simplified: use 3s rolling average via IIR)
    static float stAvg = 0.f;
    float stCoeff = (float)std::exp(-1.0 / (3.0 * sampleRate / numSamps));
    stAvg = stCoeff * stAvg + (1.f - stCoeff) * ms;
    float shortTerm = (stAvg > 0.f) ? -0.691f + 10.f * std::log10(stAvg) : -70.f;
    shortTermLUFS.store(shortTerm, std::memory_order_relaxed);

    // Integrated
    if (sampleCount > 0) {
        float intMs = squaredSum / sampleCount;
        float integ = (intMs > 0.f) ? -0.691f + 10.f * std::log10(intMs) : -70.f;
        integratedLUFS.store(integ, std::memory_order_relaxed);
    }

    // True Peak (simple: find abs max in buffer)
    float peak = 0.f;
    for (int ch = 0; ch < numCh; ++ch) {
        auto mag = buffer.getMagnitude(ch, 0, numSamps);
        peak = std::max(peak, mag);
    }
    truePeakDb.store(juce::Decibels::gainToDecibels(peak, -70.f), std::memory_order_relaxed);
}

// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
//  FFTAnalyzer
// â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€â”€
void FFTAnalyzer::prepare(double sr, int /*blockSize*/)
{
    sampleRate = sr;
    fifoData.fill(0.f);
    fftData.fill(0.f);
    spectrumOut.fill(0.f);
    nextFFTBlockReady.store(false, std::memory_order_relaxed);
}

void FFTAnalyzer::pushBuffer(const juce::AudioBuffer<float>& buffer)
{
    int numSamps = buffer.getNumSamples();
    int numCh    = juce::jmin(buffer.getNumChannels(), 2);

    for (int n = 0; n < numSamps; ++n)
    {
        // Average channels to mono for display
        float sample = 0.f;
        for (int ch = 0; ch < numCh; ++ch)
            sample += buffer.getReadPointer(ch)[n];
        sample /= (float)numCh;

        // Push into FIFO
        int start1, size1, start2, size2;
        fifo.prepareToWrite(1, start1, size1, start2, size2);
        if (size1 > 0) {
            fifoData[(size_t)start1] = sample;
            fifo.finishedWrite(size1);
        }

        // When we have a full FFT block, process it
        if (fifo.getNumReady() >= FFT_SIZE)
        {
            int s1, sz1, s2, sz2;
            fifo.prepareToRead(FFT_SIZE, s1, sz1, s2, sz2);

            // Copy to fftData with windowing
            for (int i = 0; i < sz1; ++i) fftData[(size_t)i]        = fifoData[(size_t)(s1 + i)];
            for (int i = 0; i < sz2; ++i) fftData[(size_t)(sz1 + i)] = fifoData[(size_t)(s2 + i)];
            fifo.finishedRead(sz1 + sz2);

            // Apply Hann window
            window.multiplyWithWindowingTable(fftData.data(), (size_t)FFT_SIZE);

            // Zero imaginary part
            std::fill(fftData.begin() + FFT_SIZE, fftData.end(), 0.f);

            // Run FFT
            fft.performFrequencyOnlyForwardTransform(fftData.data());

            // Smooth spectrum with decay
            const float decay = 0.85f;
            for (int i = 0; i < FFT_SIZE / 2; ++i)
                spectrumOut[(size_t)i] = decay * spectrumOut[(size_t)i]
                                       + (1.f - decay) * (fftData[(size_t)i] / FFT_SIZE);

            nextFFTBlockReady.store(true, std::memory_order_relaxed);
        }
    }
}

bool FFTAnalyzer::getLatestSpectrum(std::vector<float>& output)
{
    if (!nextFFTBlockReady.load(std::memory_order_relaxed))
        return false;

    output.assign(spectrumOut.begin(), spectrumOut.begin() + FFT_SIZE / 2);
    nextFFTBlockReady.store(false, std::memory_order_relaxed);
    return true;
}

