/**
 * IMS 2020/21 Project - Epidemic on macro level
 *
 * @authors Matej Otčenáš, Mário Gažo
 * @see https://en.wikipedia.org/wiki/Compartmental_models_in_epidemiology#The_SIR_model
 * @file main.cpp
 * @date 13. 11. 2020
 */

#include "headers/model.h"

/**
 * Main simulation function
 *
 * @param argc Argument count
 * @param argv Arguments from CLI
 * @return 0 if OK
 */
int main(int argc, char** argv){
    sirModel model;
    model.parseArgs(argc, argv);
    return model.performSimulation();
}
