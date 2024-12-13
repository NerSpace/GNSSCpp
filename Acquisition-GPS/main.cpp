#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <complex>
#include "SettingsGps.h"      // Encabezado para la clase Settings
#include "Acquisition.h"  // Encabezado para la función de adquisición

namespace fs = std::filesystem;

int main() {
    try {
        // Inicializar configuración
        Settings settings;

        // Leer archivo de entrada
        std::string npyFile = settings.inputFile + "." + std::to_string(settings.msToProcess);
        std::string npyFileWithExtension = npyFile + ".npy";
        std::vector<std::complex<double>> inputSignal;

        if (fs::exists(npyFileWithExtension)) {
            // Cargar datos del archivo .npy
            inputSignal = loadNpyFile(npyFileWithExtension);
        } else {
            // Cargar datos del archivo original
            inputSignal = loadTxtFile(settings.inputFile, 
                                      static_cast<size_t>(round(settings.msToProcess * 1e-3 * settings.samplingFreq)));

            // Guardar como .npy para uso futuro
            saveNpyFile(npyFileWithExtension, inputSignal);
        }

        // Ejecutar adquisición
        if (settings.signal.find("gpsl1c") != std::string::npos) {
            AcqResults acqResultsGpsL1C = acquisitionGpsL1C(settings, inputSignal);
            std::cout << "Acquisition complete!" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Done!" << std::endl;
    return EXIT_SUCCESS;
}
