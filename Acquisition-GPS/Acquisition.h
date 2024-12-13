#ifndef ACQUISITION_GPS_H
#define ACQUISITION_GPS_H

#include <vector>
#include <complex>
#include "SettingsGps.h"

struct AcqResults {
    double carrierFrequency;  // Frecuencia portadora estimada
    double codePhase;         // Fase de código estimada
};

AcqResults acquisitionGpsL1C(const Settings& settings, const std::vector<std::complex<double>>& inputSignal);

#endif // ACQUISITION_GPS_H
