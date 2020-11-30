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
    // Population of chinese province Hubei
    long double N = 58500000.0;/* Whole population: N = S + I + R (+ E + D) */

    // Current values
    struct curr {
        long double
            S = 0.0/* Susceptible */,
            // estimated by scientific paper of Mr.Wang the initial
            // number of exposed is 20 times greater than the number infected
            E = 20.0/* Exposed */,
            I = 1.0/* Infected, case 0 */,
            R = 0.0/* Removed */,
            D = 0.0/* Dead */;
    } curr;

    // Next values
    struct next {
        long double
            S = 0.0/* Susceptible */,
            // Estimated by scientific paper of Mr.Wang the initial
            // number of exposed is 20 times greater than the number infected
            E = 20.0/* Exposed */,
            I = 1.0/* Infected, case 0 */,
            R = 0.0/* Removed */,
            D = 0.0/* Dead */;
    } next;

    // Constants typical for COVID19
    struct rates {
        long double
            R0 = 6.0,/* Basic reproduction number of disease */

            // hospitalization period^(-1) -> hospitalization period was estimated as 18 days
            alpha = 0.0556/* Recovery: infected recovers and moves into the resistant phase */,

            // R0 * alpha -> R0 is initial basic infection-reproduction number and alpha is recovery rate
            beta  = 0.0556 * 6.0/* Transmission: how often a susceptible-infected contact results in a new infection */,

            // mean incubation period^(-1) -> mean incubation period is set to 5.2 days
            sigma = 0.1923/* Infectivity: exposed person becomes infective */,

            omega = 0.0034/* Fatality: infected person dies */;
    } rates;

    // Statistical values we are going to track
    struct stats {
        long double maxInfected = 0.0, maxIncrement = 0.0, sumInfected = 0.0, sumRecovered = 0.0;
        unsigned long dayMaxInfected = 0, dayMaxIncrement = 0;
    } stats;

    // Daily change of infected people
    long double derrI = 0.0/* Infected*/;

    bool SIERD = false/* Simulation model */;
    unsigned long days = 0;/* Number of days of simulation */
    bool restrictions = false;/* Government measures */
    std::ofstream output/* Data file */;

    /**
     * Simulation
     *
     * @return 0 if OK
     */
    int simulate();

    /**
     * Calculates nest step of a simulation based on differential equations
     */
    void nextStep();

    /**
     * Counts the date difference between 31.12.2019 until 1.12.2020
     */
    void simulationDays();

    /**
     * Outputs initial conditions to stdout
     */
    void printHeader();

    /**
     * Outputs simulations results to stdout
     */
    void printFooter();

    /**
     * Simulates epidemic with no measures (SIR)
     *
     * @return 0 if OK
     */
    int exp1();

    /**
     * Simulates epidemic with government measures (SIR)
     *
     * @return 0 if OK
     */
    int exp2();

    /**
     * Simulates epidemic with no measures (SIERD)
     *
     * @return 0 if OK
     */
    int exp3();

    /**
     * Simulates epidemic with measures (SIERD)
     *
     * @return 0 if OK
     */
    int exp4();

public:
    /**
     * Wraps days calculation and experiment
     *
     * @param num experiment number
     * @return 0 if OK
     */
    int performExp(int num);
};

#endif //_MAIN_H_
