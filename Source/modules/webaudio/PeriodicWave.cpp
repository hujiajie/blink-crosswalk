/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"

#if ENABLE(WEB_AUDIO)

#include "modules/webaudio/PeriodicWave.h"

#include "platform/audio/FFTFrame.h"
#include "platform/audio/VectorMath.h"
#include "modules/webaudio/OscillatorNode.h"
#include <algorithm>

namespace blink {

const unsigned PeriodicWaveSize = 4096; // This must be a power of two.
const unsigned NumberOfRanges = 36; // There should be 3 * log2(PeriodicWaveSize) 1/3 octave ranges.
const float CentsPerRange = 1200 / 3; // 1/3 Octave.

const unsigned PeriodicWave::kMaxPeriodicWaveArraySize = PeriodicWaveSize / 2;

using namespace VectorMath;

PeriodicWave* PeriodicWave::create(float sampleRate, DOMFloat32Array* real, DOMFloat32Array* imag)
{
    bool isGood = real && imag && real->length() == imag->length();
    ASSERT(isGood);
    if (isGood) {
        PeriodicWave* periodicWave = new PeriodicWave(sampleRate);
        size_t numberOfComponents = real->length();
        periodicWave->createBandLimitedTables(real->data(), imag->data(), numberOfComponents);
        return periodicWave;
    }
    return 0;
}

PeriodicWave* PeriodicWave::createSine(float sampleRate)
{
    PeriodicWave* periodicWave = new PeriodicWave(sampleRate);
    periodicWave->generateBasicWaveform(OscillatorNode::SINE);
    return periodicWave;
}

PeriodicWave* PeriodicWave::createSquare(float sampleRate)
{
    PeriodicWave* periodicWave = new PeriodicWave(sampleRate);
    periodicWave->generateBasicWaveform(OscillatorNode::SQUARE);
    return periodicWave;
}

PeriodicWave* PeriodicWave::createSawtooth(float sampleRate)
{
    PeriodicWave* periodicWave = new PeriodicWave(sampleRate);
    periodicWave->generateBasicWaveform(OscillatorNode::SAWTOOTH);
    return periodicWave;
}

PeriodicWave* PeriodicWave::createTriangle(float sampleRate)
{
    PeriodicWave* periodicWave = new PeriodicWave(sampleRate);
    periodicWave->generateBasicWaveform(OscillatorNode::TRIANGLE);
    return periodicWave;
}

PeriodicWave::PeriodicWave(float sampleRate)
    : m_sampleRate(sampleRate)
    , m_periodicWaveSize(PeriodicWaveSize)
    , m_numberOfRanges(NumberOfRanges)
    , m_centsPerRange(CentsPerRange)
{
    float nyquist = 0.5 * m_sampleRate;
    m_lowestFundamentalFrequency = nyquist / maxNumberOfPartials();
    m_rateScale = m_periodicWaveSize / m_sampleRate;
}

void PeriodicWave::waveDataForFundamentalFrequency(float fundamentalFrequency, float* &lowerWaveData, float* &higherWaveData, float& tableInterpolationFactor)
{
    // Negative frequencies are allowed, in which case we alias to the positive frequency.
    fundamentalFrequency = fabsf(fundamentalFrequency);

    // Calculate the pitch range.
    float ratio = fundamentalFrequency > 0 ? fundamentalFrequency / m_lowestFundamentalFrequency : 0.5;
    float centsAboveLowestFrequency = log2f(ratio) * 1200;

    // Add one to round-up to the next range just in time to truncate partials before aliasing occurs.
    float pitchRange = 1 + centsAboveLowestFrequency / m_centsPerRange;

    pitchRange = std::max(pitchRange, 0.0f);
    pitchRange = std::min(pitchRange, static_cast<float>(m_numberOfRanges - 1));

    // The words "lower" and "higher" refer to the table data having the lower and higher numbers of partials.
    // It's a little confusing since the range index gets larger the more partials we cull out.
    // So the lower table data will have a larger range index.
    unsigned rangeIndex1 = static_cast<unsigned>(pitchRange);
    unsigned rangeIndex2 = rangeIndex1 < m_numberOfRanges - 1 ? rangeIndex1 + 1 : rangeIndex1;

    lowerWaveData = m_bandLimitedTables[rangeIndex2]->data();
    higherWaveData = m_bandLimitedTables[rangeIndex1]->data();

    // Ranges from 0 -> 1 to interpolate between lower -> higher.
    tableInterpolationFactor = pitchRange - rangeIndex1;
}

unsigned PeriodicWave::maxNumberOfPartials() const
{
    return m_periodicWaveSize / 2;
}

unsigned PeriodicWave::numberOfPartialsForRange(unsigned rangeIndex) const
{
    // Number of cents below nyquist where we cull partials.
    float centsToCull = rangeIndex * m_centsPerRange;

    // A value from 0 -> 1 representing what fraction of the partials to keep.
    float cullingScale = pow(2, -centsToCull / 1200);

    // The very top range will have all the partials culled.
    unsigned numberOfPartials = cullingScale * maxNumberOfPartials();

    return numberOfPartials;
}

// Convert into time-domain wave buffers.
// One table is created for each range for non-aliasing playback at different playback rates.
// Thus, higher ranges have more high-frequency partials culled out.
void PeriodicWave::createBandLimitedTables(const float* realData, const float* imagData, unsigned numberOfComponents)
{
    float normalizationScale = 1;

    unsigned fftSize = m_periodicWaveSize;
    unsigned halfSize = fftSize / 2;
    unsigned i;

    numberOfComponents = std::min(numberOfComponents, halfSize);

    m_bandLimitedTables.reserveCapacity(m_numberOfRanges);

    for (unsigned rangeIndex = 0; rangeIndex < m_numberOfRanges; ++rangeIndex) {
        // This FFTFrame is used to cull partials (represented by frequency bins).
        FFTFrame frame(fftSize);
        float* realP = frame.realData();
        float* imagP = frame.imagData();

        // Copy from loaded frequency data and scale.
        float scale = fftSize;
        vsmul(realData, 1, &scale, realP, 1, numberOfComponents);
        vsmul(imagData, 1, &scale, imagP, 1, numberOfComponents);

        // If fewer components were provided than 1/2 FFT size, then clear the remaining bins.
        for (i = numberOfComponents; i < halfSize; ++i) {
            realP[i] = 0;
            imagP[i] = 0;
        }

        // Generate complex conjugate because of the way the inverse FFT is defined.
        float minusOne = -1;
        vsmul(imagP, 1, &minusOne, imagP, 1, halfSize);

        // Find the starting bin where we should start culling.
        // We need to clear out the highest frequencies to band-limit the waveform.
        unsigned numberOfPartials = numberOfPartialsForRange(rangeIndex);

        // Cull the aliasing partials for this pitch range.
        for (i = numberOfPartials + 1; i < halfSize; ++i) {
            realP[i] = 0;
            imagP[i] = 0;
        }
        // Clear packed-nyquist if necessary.
        if (numberOfPartials < halfSize)
            imagP[0] = 0;

        // Clear any DC-offset.
        realP[0] = 0;

        // Create the band-limited table.
        OwnPtr<AudioFloatArray> table = adoptPtr(new AudioFloatArray(m_periodicWaveSize));
        m_bandLimitedTables.append(table.release());

        // Apply an inverse FFT to generate the time-domain table data.
        float* data = m_bandLimitedTables[rangeIndex]->data();
        frame.doInverseFFT(data);

        // For the first range (which has the highest power), calculate its peak value then compute normalization scale.
        if (!rangeIndex) {
            float maxValue;
            vmaxmgv(data, 1, &maxValue, m_periodicWaveSize);

            if (maxValue)
                normalizationScale = 1.0f / maxValue;
        }

        // Apply normalization scale.
        vsmul(data, 1, &normalizationScale, data, 1, m_periodicWaveSize);
    }
}

void PeriodicWave::generateBasicWaveform(int shape)
{
    unsigned fftSize = periodicWaveSize();
    unsigned halfSize = fftSize / 2;

    AudioFloatArray real(halfSize);
    AudioFloatArray imag(halfSize);
    float* realP = real.data();
    float* imagP = imag.data();

    // Clear DC and Nyquist.
    realP[0] = 0;
    imagP[0] = 0;

    for (unsigned n = 1; n < halfSize; ++n) {
        float piFactor = 2 / (n * piFloat);

        // All waveforms are odd functions with a positive slope at time 0. Hence the coefficients
        // for cos() are always 0.

        // Fourier coefficients according to standard definition:
        // b = 1/pi*integrate(f(x)*sin(n*x), x, -pi, pi)
        //   = 2/pi*integrate(f(x)*sin(n*x), x, 0, pi)
        // since f(x) is an odd function.

        float b; // Coefficient for sin().

        // Calculate Fourier coefficients depending on the shape. Note that the overall scaling
        // (magnitude) of the waveforms is normalized in createBandLimitedTables().
        switch (shape) {
        case OscillatorNode::SINE:
            // Standard sine wave function.
            b = (n == 1) ? 1 : 0;
            break;
        case OscillatorNode::SQUARE:
            // Square-shaped waveform with the first half its maximum value and the second half its
            // minimum value.
            //
            // See http://mathworld.wolfram.com/FourierSeriesSquareWave.html
            //
            // b[n] = 2/n/pi*(1-(-1)^n)
            //      = 4/n/pi for n odd and 0 otherwise.
            //      = 2*(2/(n*pi)) for n odd
            b = (n & 1) ? 2 * piFactor : 0;
            break;
        case OscillatorNode::SAWTOOTH:
            // Sawtooth-shaped waveform with the first half ramping from zero to maximum and the
            // second half from minimum to zero.
            //
            // b[n] = -2*(-1)^n/pi/n
            //      = (2/(n*pi))*(-1)^(n+1)
            b = piFactor * ((n & 1) ? 1 : -1);
            break;
        case OscillatorNode::TRIANGLE:
            // Triangle-shaped waveform going from 0 at time 0 to 1 at time pi/2 and back to 0 at
            // time pi.
            //
            // See http://mathworld.wolfram.com/FourierSeriesTriangleWave.html
            //
            // b[n] = 8*sin(pi*k/2)/(pi*k)^2
            //      = 8/pi^2/n^2*(-1)^((n-1)/2) for n odd and 0 otherwise
            //      = 2*(2/(n*pi))^2 * (-1)^((n-1)/2)
            if (n & 1) {
                b = 2 * (piFactor * piFactor) * ((((n - 1) >> 1) & 1) ? -1 : 1);
            } else {
                b = 0;
            }
            break;
        default:
            ASSERT_NOT_REACHED();
            b = 0;
            break;
        }

        realP[n] = 0;
        imagP[n] = b;
    }

    createBandLimitedTables(realP, imagP, halfSize);
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
