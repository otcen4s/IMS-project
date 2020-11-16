/**
 * IMS 2020/21 Project - Epidemic on macro level
 *
 * @authors Matej Otčenáš, Mário Gažo
 * @see https://en.wikipedia.org/wiki/Compartmental_models_in_epidemiology#The_SIR_model
 * @file main.cpp
 * @date 13. 11. 2020
 */

#include "headers/Model.h"

/**
 * Main simulation function
 *
 * @param argc Argument count
 * @param argv Arguments from CLI
 * @return 0 if OK
 */
int main(int argc, char** argv){
    Model model;

    if (argc == 2) {
        if (strcmp(argv[1],"scenario1") == 0) {
            model.exp1();
        }

        if (strcmp(argv[1],"scenario2") == 0) {
            model.exp2();
        }

        if (strcmp(argv[1],"scenario3") == 0) {
            model.exp3();
        }

        if (strcmp(argv[1],"scenario4") == 0) {
            model.exp4();
        }
        
    } else {
        model.parseArgs(argc, argv);

        if (model.modelSEIRD) {
            model.simulateSEIRD();
        } else {
            model.simulateSIR();
        }
    }
}
