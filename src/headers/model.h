/**
 * IMS 2020/21 Project - Epidemic on macro level
 *
 * Simulation model interface
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
    long double N = 0.0; // Whole population: N = S + I + R (+ E + D)

    // Current values
    struct curr {
        long double
            S = 0.0/* Susceptible */,
            I = 0.0/* Infected*/,
            R = 0.0/* Removed */,
            E = 0.0/* Exposed */,
            D = 0.0/* Dead */;
    } curr;

    // Next values
    struct next {
        long double
            S = 0.0/* Susceptible */,
            I = 0.0/* Infected*/,
            R = 0.0/* Removed */,
            E = 0.0/* Exposed */,
            D = 0.0/* Dead */;
    } next;

    // Derivatives = next - curr
    struct derr {
        long double
            S = 0.0/* Susceptible */,
            I = 0.0/* Infected*/,
            R = 0.0/* Removed */,
            E = 0.0/* Exposed */,
            D = 0.0/* Dead */;
    } derr;

    // Coefficients for calculations
    struct rates {
        long double
            beta  = 0.0/* Transmission: how often a susceptible-infected contact results in a new infection */,
            alpha = 0.0/* Recovery: infected recovers and moves into the resistant phase */,
            sigma = 0.0/* Infectivity: exposed person becomes infective */,
            omega = 0.0/* Fatality: infected person dies */;
    } rates;

    // Output files
    struct filenames {
        std::string SIR = "data_SIR.csv";
        std::string SEIRD = "data_SEIRD.csv";
    } filenames;

    unsigned long steps = 0; /* number of steps */

public:
    bool modelSEIRD = false;

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
     *  dS = - S * I * alpha
     *  dI = S * I * alpha - alpha * I
     *  dR = alpha * I
     *
     *  dS + dI + dR = 0 (Changes have to match)
     *  S + I + R = N (Entire population)
     */
    void calculateSIR();

    /**
     * S + E + I + R + D = N (Entire population)
     * Results of ordinary differential equations for S, E, I, R, D values at every time point of the simulation
     */
    void calculateSEIRD();

    /**
     * Simulation of SIR model
     *
     * @return 0 if OK
     */
    int simulateSIR();

    /**
     * Simulation of SEIRD model
     *
     * @return 0 if OK
     */
    int simulateSEIRD();

    /**
     * First experiment, demonstrates epidemic with no measures (SIR)
     */
    void exp1();

    /**
     * Second experiment, demonstrates epidemic with no measures (SIERD)
     */
    void exp2();
};

#endif //_MAIN_H_
