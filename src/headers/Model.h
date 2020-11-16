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
#include <iostream>
#include <getopt.h>
#include <string>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <cmath>

/**
 * Simulation model interface
 */
class Model {
private:
    long double N = 0.0;/* Whole population: N = S + I + R (+ E + D) */

    // Current values
    struct curr {
        long double
            S = 0.0/* Susceptible */,
            E = 0.0/* Exposed */,
            I = 0.0/* Infected*/,
            R = 0.0/* Removed */,
            D = 0.0/* Dead */;
    } curr;

    // Next values
    struct next {
        long double
            S = 0.0/* Susceptible */,
            E = 0.0/* Exposed */,
            I = 0.0/* Infected*/,
            R = 0.0/* Removed */,
            D = 0.0/* Dead */;
    } next;

    // Derivatives = next - curr
    struct derr {
        long double
            S = 0.0/* Susceptible */,
            E = 0.0/* Exposed */,
            I = 0.0/* Infected*/,
            R = 0.0/* Removed */,
            D = 0.0/* Dead */;
    } derr;

    // Coefficients for calculations
    struct rates {
        long double
            beta  = 0.0/* Transmission: how often a susceptible-infected contact results in a new infection */,
            alpha = 0.0/* Recovery: infected recovers and moves into the resistant phase */,
            sigma = 0.0/* Infectivity: exposed person becomes infective */,
            omega = 0.0/* Fatality: infected person dies */,
            betaNative = 0.0;/* Initial value of beta */
    } rates;

    // Output files
    struct filenames {
        std::string SIR = "statistics/data_SIR.csv";
        std::string SEIRD = "statistics/data_SEIRD.csv";
    } filenames;

    // Statistical values we are going to track
    struct stats {
        long double maxInfected = 0.0, maxIncrement = 0.0, sumInfected = 0.0;
        unsigned long dayMaxInfected = 0, dayMaxIncrement = 0;
    } stats;

    unsigned long days = 0;/* number of days of simulation */
    bool restrictions = false;/* government measures */
    std::ofstream output;

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
     * Demonstrates epidemic with no measures (SIR)
     *
     * Population = 5 000 000
     * Infected = 1
     * Transmission rate = 0.6
     * Recovery rate = 0.095
     * Restrictions = YES
     *
     * Days = 100
     */
    void exp1();

    /**
     * Demonstrates epidemic with government measures (SIR)
     *
     * Population = 5 000 000
     * Infected = 1
     * Transmission rate = 0.6
     * Recovery rate = 0.095
     * Restrictions = YES
     *
     * Days = 400
     */
    void exp2();

    /**
     * Third experiment, demonstrates epidemic with no measures (SEIRD)
     */
    void exp3();

    /**
     * Fourth experiment, demonstrates epidemic with measures (SEIRD)
     */
    void exp4();

    /**
     * Depending on current amount of infected people there are multiple measures that can be taken, each of them
     * reduces transmission rate
     *
     * M1 = closed schools, cinemas (25% reduction)
     * M2 = masks in interior (50% reduction)
     * M3 = masks in exterior (70% reduction)
     * M4 = only trips to work and for groceries are allowed (almost 100% reduction)
     */
    void setRestriction();

    /**
     * Output information about simulation to stdout
     *
     * @param which "header" => input info, "footer" => output info
     * @param SEIRD true => model is of type SEIRD, false => model is of type SIR
     */
    void printInfo(const std::string& which, bool SEIRD);
};

#endif //_MAIN_H_
