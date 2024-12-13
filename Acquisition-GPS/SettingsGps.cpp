#include "SettingsGps.h"

Settings::Settings() {
    // Archivo de entrada con señal sin procesar
    inputFile = "../DATA/GPS_RECORDED_RAW_SIGNAL_37000ms.dat";

    // Tipo de señal
    signal = "gpsl1c";

    // Frecuencia intermedia
    IF = 9.548e6; // [Hz]

    // Frecuencia de muestreo
    samplingFreq = 38.192e6; // [Hz]

    // Frecuencia base del código C/A
    codeFreqBasis = 1.023e6; // [Hz]

    // Longitud del código C/A
    codeLength = 1023;

    // Máscara de satélites
    for (int i = 1; i <= 32; ++i) {
        satMask.push_back(i);
    }

    // Rango de frecuencia en adquisición
    acqFreqRangekHz = 14; // [kHz]
    acqTh = 2.5;          // Umbral

    // Número de canales del receptor
    numberOfChannels = 10;

    // Milisegundos a procesar
    msToProcess = 3000; // [ms]
}
