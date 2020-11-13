/**
 * IMS 2020/21 Project - Epidemic on macro level
 *
 * Simulation model implementation
 *
 * @authors Matej Otčenáš, Mário Gažo
 * @see https://en.wikipedia.org/wiki/Compartmental_models_in_epidemiology#The_SIR_model
 * @file model.h
 * @date 13. 11. 2020
 */

#ifndef _MAIN_H_
#define _MAIN_H_

/**
 * Include of libraries (C/C++)
 */
#include "../../simlib/src/simlib.h" // TODO change to "simlib.h"
#include <iostream>
#include <getopt.h>
#include <string> 
#include <fstream>

/**
 * Simulation model interface
 */
class sirModel {
private:
    long double
        S/* susceptible */, I/* infected*/, R = 0.0/* removed */, E/* Exposed */, D /* Dead */, N/* whole population */,
        dS = 0.0, dE = 0.0, dI = 0.0, dR = 0.0, dD = 0.0,/* new values */

    // beta:  The rate of how often a susceptible-infected contact results in a new infection.(transmission rate)
    // alpha: The rate an infected recovers and moves into the resistant phase.(recovery rate)
    // sigma: The rate at which an exposed person becomes infective.
    // omega: The infection fatality rate (IFR).
    // rates
    beta = 0.0/* transmission */, alpha = 0.0/* recovery */,
    sigma = 0.0/* infectiveness */, omega = 0.0 /* fatality */;

    unsigned long steps = 0; /* number of steps */

    bool modelSEIRD = false;
    std::string SEIRD = "data_SEIRD.csv";
    std::string SIR = "data_SIR.csv";

public:
    /**
     * Function prints help guide to stdout and terminates the program successfully
     */
    void print_help();

    /**
     * Function parses the given arguments from stdin using getopt
     */
    void parseArgs(int argc, char **argv);

    /**
     * Simulation of an epidemic based on SIR model:
     *  S = Sold - Sold * Iold * alpha
     *  I = Iold + Sold * Iold * alpha - alpha * Iold
     *  R = alpha * Iold
     *
     *  S + I + R = N (Entire population)
     */
    void calculateSIR();

    /**
     * S + E + I + R + D = N (Entire population)
     * Results of ordinary differential equations for S, E, I, R, D values at every time point of the simulation
     */
    void calculateSEIRD();

    /**
     * Simulation itself
     *
     * @return 0 if OK
     */
    int performSimulation();
};

#endif //_MAIN_H_
