/*
########################################################################
# Acquisition.cpp:
# Acquisition module
#
#  Project:        sw-rcvr-c++
#  File:           Acquisition.cppS
#
#   Author: Nerea Sánchez
#   Copyright 2024 Nerea Sánchez
#
########################################################################
*/

#include <iostream>
#include <vector>
#include <complex>
#include <string>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <fftw3.h>
#include <algorithm>
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;
namespace fs = std::filesystem;

class AcqResults {
public:
    std::vector<std::vector<std::vector<double>>> searchSpace; // Search space
    std::vector<double> carrFreq;    // Carrier frequencies of detected signals
    std::vector<double> codeDelay;  // Code delays of detected signals
    std::vector<double> peakMetric; // Correlation peak ratios
    std::vector<double> SNR;        // Signal-to-Noise ratio

    // Initialize the acquisition results
    void initialize(const Settings& settings) {
        size_t samplesPerCode = static_cast<size_t>(
            std::round(settings.samplingFreq * settings.codeLength / settings.codeFreqBasis));

        size_t nFrqBins = static_cast<size_t>(std::round(settings.acqFreqRangekHz * 8)) + 1;

        size_t numSatellites = settings.satMask.size() + 1;

        searchSpace.resize(numSatellites, std::vector<std::vector<double>>(nFrqBins, std::vector<double>(samplesPerCode, 0.0)));
        carrFreq.resize(numSatellites, 0.0);
        codeDelay.resize(numSatellites, 0.0);
        peakMetric.resize(numSatellites, 0.0);
        SNR.resize(numSatellites, 0.0);
    }

    // Plot the acquisition results
    void plot(const Settings& settings, size_t samplesPerCode, size_t nFrqBins) {
        std::string figspath = fs::path(settings.inputFile).parent_path().string() + "/SW-RCVR-C++/";

        if (!fs::exists(figspath)) {
            fs::create_directories(figspath);
        }

        for (size_t PRN = 1; PRN < settings.satMask.size() + 1; ++PRN) {
            std::vector<std::vector<double>> prnSearchSpace = searchSpace[PRN];

            std::vector<double> frequencies(nFrqBins);
            for (size_t i = 0; i < nFrqBins; ++i) {
                frequencies[i] = -settings.acqFreqRangekHz / 2 + 0.125 * i;
            }

            std::vector<double> delays(samplesPerCode);
            for (size_t i = 0; i < samplesPerCode; ++i) {
                delays[i] = i * settings.codeFreqBasis / settings.samplingFreq;
            }

            plt::figure();
            plt::plot_surface(frequencies, delays, prnSearchSpace, {
                {"cmap", "coolwarm"},
                {"antialiased", false}
            });

            plt::title("PRN " + std::to_string(PRN) + " Search Space");
            plt::xlabel("Doppler Frequency [kHz]");
            plt::ylabel("Code Delay");
            plt::grid(true, "--", 0.5);

            plt::save(figspath + "SEARCH_SPACE_PRN" + std::to_string(PRN) + ".png");
            plt::close();
        }

        plotBar(settings, "Acquisition Metric", peakMetric, figspath + "ACQUISITION_METRIC.png");
        plotBar(settings, "Signal to Noise Ratio [dB-Hz]", SNR, figspath + "SNR.png", true);
    }

private:
    // Helper function for bar plots
    void plotBar(const Settings& settings, const std::string& title, const std::vector<double>& data, const std::string& filepath, bool limitY = false) {
        std::vector<double> x;
        for (size_t i = 1; i <= settings.satMask.size(); ++i) {
            x.push_back(static_cast<double>(i));
        }

        plt::figure();
        plt::bar(x, std::vector<double>(data.begin() + 1, data.end()));
        plt::title(title);
        plt::xlabel("PRN number (no bar - SV is not in the acquisition list)");
        plt::ylabel(title);
        plt::grid(true, "--", 0.5);

        if (limitY) {
            plt::ylim(20, *std::max_element(data.begin() + 1, data.end()));
        }

        plt::save(filepath);
        plt::close();
    }
};

int nextPowerOf2(int n) {
    return pow(2, ceil(log2(n)));
}

AcqResults acquisitionGpsL1C(Settings& settings, std::vector<double>& inputSignal) {
    int samplesPerCode = round((settings.samplingFreq * settings.codeLength) / settings.codeFreqBasis);
    int samplesPerCodeChip = round(settings.samplingFreq / settings.codeFreqBasis);
    std::vector<double> signal1(inputSignal.begin(), inputSignal.begin() + samplesPerCode);
    std::vector<double> signal2(inputSignal.begin() + samplesPerCode, inputSignal.begin() + 2 * samplesPerCode);
    std::vector<double> signal0DC(inputSignal.size());
    double ts = 1 / settings.samplingFreq;
    double tc = 1 / settings.codeFreqBasis;

    // Create phase points
    std::vector<double> phasePoints(samplesPerCode);
    for (int i = 0; i < samplesPerCode; ++i) {
        phasePoints[i] = 2 * i * M_PI * ts;
    }

    int nFrqBins = round(settings.acqFreqRangekHz * 8) + 1;

    std::vector<std::vector<double>> results(nFrqBins, std::vector<double>(samplesPerCode));
    std::vector<double> frqBins(nFrqBins);
    std::vector<int> codeOversampIdx(samplesPerCode);

    // Initialize the index for oversampling
    for (int i = 0; i < samplesPerCode; ++i) {
        codeOversampIdx[i] = ceil(ts * (i + 1) / tc) - 1;
    }
    codeOversampIdx[samplesPerCode - 1] = settings.codeLength - 1;

    // Initialize AcqResults object
    AcqResults acqResults;
    acqResults.initialize(settings);

    std::cout << "Acquiring GPS L1C ...\n(";

    for (int PRN : settings.satMask) {
        std::vector<int> caCodeReplica = generateGoldCode(PRN);

        // Digitizing the C/A code
        std::vector<int> caCodeReplicaOversamp(samplesPerCode);
        for (int i = 0; i < samplesPerCode; ++i) {
            caCodeReplicaOversamp[i] = caCodeReplica[codeOversampIdx[i]];
        }

        // Perform DFT of C/A code
        std::vector<std::complex<double>> caCodeReplicaFreqDom(samplesPerCode);
        fftw_complex* caCodeReplicaFreqDomArr = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * samplesPerCode);
        fftw_plan plan = fftw_plan_dft_1d(samplesPerCode, (fftw_complex*)caCodeReplicaOversamp.data(),
                                          caCodeReplicaFreqDomArr, FFTW_BACKWARD, FFTW_ESTIMATE);
        fftw_execute(plan);
        for (int i = 0; i < samplesPerCode; ++i) {
            caCodeReplicaFreqDom[i] = std::complex<double>(caCodeReplicaFreqDomArr[i][0], caCodeReplicaFreqDomArr[i][1]);
        }

        // Correlate signals for all frequency bins
        for (int frqBinIndex = 0; frqBinIndex < nFrqBins; ++frqBinIndex) {
            frqBins[frqBinIndex] = settings.IF - (settings.acqFreqRangekHz / 2) * 1000 + 125 * frqBinIndex;
            std::vector<double> sinCarrReplica(samplesPerCode);
            std::vector<double> cosCarrReplica(samplesPerCode);

            // Generate local sine and cosine
            for (int i = 0; i < samplesPerCode; ++i) {
                sinCarrReplica[i] = sin(frqBins[frqBinIndex] * phasePoints[i]);
                cosCarrReplica[i] = cos(frqBins[frqBinIndex] * phasePoints[i]);
            }

            // Remove carrier from signal (demodulation)
            std::vector<double> I1(samplesPerCode), Q1(samplesPerCode), I2(samplesPerCode), Q2(samplesPerCode);
            for (int i = 0; i < samplesPerCode; ++i) {
                I1[i] = sinCarrReplica[i] * signal1[i];
                Q1[i] = cosCarrReplica[i] * signal1[i];
                I2[i] = sinCarrReplica[i] * signal2[i];
                Q2[i] = cosCarrReplica[i] * signal2[i];
            }

            // Convert to frequency domain
            fftw_complex* IQfreqDom1Arr = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * samplesPerCode);
            fftw_complex* IQfreqDom2Arr = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * samplesPerCode);
            fftw_plan IQplan1 = fftw_plan_dft_1d(samplesPerCode, (fftw_complex*)I1.data(),
                                                 IQfreqDom1Arr, FFTW_FORWARD, FFTW_ESTIMATE);
            fftw_plan IQplan2 = fftw_plan_dft_1d(samplesPerCode, (fftw_complex*)I2.data(),
                                                 IQfreqDom2Arr, FFTW_FORWARD, FFTW_ESTIMATE);
            fftw_execute(IQplan1);
            fftw_execute(IQplan2);

            // Frequency domain multiplication (correlation in time domain)
            std::vector<std::complex<double>> convCodeIQ1(samplesPerCode), convCodeIQ2(samplesPerCode);
            for (int i = 0; i < samplesPerCode; ++i) {
                convCodeIQ1[i] = IQfreqDom1Arr[i] * caCodeReplicaFreqDom[i];
                convCodeIQ2[i] = IQfreqDom2Arr[i] * caCodeReplicaFreqDom[i];
            }

            // Inverse DFT and store correlation results
            std::vector<double> acqRes1(samplesPerCode), acqRes2(samplesPerCode);
            fftw_complex* acqRes1Arr = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * samplesPerCode);
            fftw_complex* acqRes2Arr = (fftw_complex*)fftw_malloc(sizeof(fftw_complex) * samplesPerCode);
            fftw_plan ifftPlan1 = fftw_plan_dft_1d(samplesPerCode, (fftw_complex*)convCodeIQ1.data(),
                                                    acqRes1Arr, FFTW_BACKWARD, FFTW_ESTIMATE);
            fftw_plan ifftPlan2 = fftw_plan_dft_1d(samplesPerCode, (fftw_complex*)convCodeIQ2.data(),
                                                    acqRes2Arr, FFTW_BACKWARD, FFTW_ESTIMATE);
            fftw_execute(ifftPlan1);
            fftw_execute(ifftPlan2);

            // Calculate magnitude squared
            for (int i = 0; i < samplesPerCode; ++i) {
                acqRes1[i] = std::norm(acqRes1Arr[i]);
                acqRes2[i] = std::norm(acqRes2Arr[i]);
            }

            // Store results for the frequency bin
            if (max(acqRes1) > max(acqRes2)) {
                results[frqBinIndex] = acqRes1;
            } else {
                results[frqBinIndex] = acqRes2;
            }
        }

        // Further processing for peak finding, SNR calculation, and signal acquisition continues...

    }

    std::cout << ")\n";

    // Plot results
    acqResults.plot(settings, samplesPerCode, nFrqBins);

    return acqResults;
}

