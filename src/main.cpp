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
    // Wrong param count
    if (argc != 2) { return 1; }

    // Model init, start & end time set and simulations
    Model model;
    if (strcmp(argv[1],"scenario1") == 0) { return model.performExp(1); }
    if (strcmp(argv[1],"scenario2") == 0) { return model.performExp(2); }
    if (strcmp(argv[1],"scenario3") == 0) { return model.performExp(3); }
    if (strcmp(argv[1],"scenario4") == 0) { return model.performExp(4); }

    // Wrong experiment name
    return 1;
}
