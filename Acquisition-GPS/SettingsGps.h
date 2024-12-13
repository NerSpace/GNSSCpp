#ifndef SETTINGSGPS_H
#define SETTINGSGPS_H

#include <string>
#include <vector>

class Settings {
public:
    // Atributos de configuración
    std::string inputFile;         // Archivo de entrada
    std::string signal;            // Tipo de señal

    double IF;                     // Frecuencia intermedia [Hz]
    double samplingFreq;           // Frecuencia de muestreo [Hz]
    double codeFreqBasis;          // Frecuencia de código [Hz]
    int codeLength;                // Longitud del código C/A [chips]

    std::vector<int> satMask(32, 0);      // Máscara de satélites

    int acqFreqRangekHz;           // Número de bandas de frecuencia [kHz]
    double acqTh;                  // Umbral de adquisición

    int numberOfChannels;          // Número de canales del receptor
    int msToProcess;               // Milisegundos a procesar [ms]

    // Constructor
    Settings();
};

#endif // SETTINGSGPS_H
