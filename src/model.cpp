/**
 * IMS 2020/21 Project - Epidemic on macro level
 *
 * Simulation model implementation
 *
 * @authors Matej Otčenáš, Mário Gažo
 * @see https://en.wikipedia.org/wiki/Compartmental_models_in_epidemiology#The_SIR_model
 * @file model.cpp
 * @date 13. 11. 2020
 */
#include "headers/model.h"
#include <cstdlib>

using namespace std;

void sirModel::print_help() {
    std::cout <<
              "-i [infected]             Initial count of infective.\n"
              "-p [population]           Initial count of population.\n"
              "-e [exposed]              Initial count of exposed.\n"
              "-b [transmission_rate]    Transmission rate of infection COVID-19.\n"  
              "-a [recovery_rate]        Recovery rate from infection COVID-19.\n"
              "-o [fatality_rate]        Infection fatality rate (IFR) of COVID-19\n"
              "-m [exposed_rate]         The rate at which an exposed person becomes infective of COVID-19\n"         
              "-s [steps]                Number of steps of simulation.\n"
              "-x [model]                1 = SEIRD, 0 = SIR\n"
              "-h [help]                 Help guide for program usage.\n";
    exit(0);
}

void sirModel::parseArgs(int argc, char **argv) {
    int opt = 0;

    while (true) {
        opt = getopt(argc, argv, "i:p:b:a:s:e:o:m:x:h");

        if(opt == -1) {
            return;
        }

        // Non-argument options
        switch (opt) {
            case 'h':
                print_help();
                break;

            case ':':
                cerr << "option needs a value" << endl;
                exit(1);

            case '?':
                cerr << "unknown option: " << optopt << endl;
                exit(1);

            default:
                // Single-argument options
                if(!optarg || !optarg[0]) {
                    this->print_help();
                }

                switch (opt) {
                    case 'i': // infected
                        curr.I = next.I = stod(optarg); break;

                    case 'p': // population
                        N = stod(optarg); break;

                    case 'b': // transmission
                        rates.beta = stod(optarg); break;

                    case 'a': // recovery
                        rates.alpha = stod(optarg); break;

                    case 'e': // exposed
                        curr.E = next.E = stod(optarg); break;

                    case 'o': // fatality
                        rates.omega = stod(optarg); break;

                    case 'm': // infectivity
                        rates.sigma = stod(optarg); break;

                    case 's': // steps
                        steps = strtol(optarg, nullptr, 10); break;

                    case 'x': // SIR or SEIRD ?
                        if(strtol(optarg, nullptr, 10) == 1) {modelSEIRD = true;} break;

                    default:
                        print_help();
                    }
        }
    }
}

void sirModel::calculateSIR() {
    // Calculate new values
    long double rnd = ((double)rand() / RAND_MAX) * 2; // pseudorandom float on <0,2>
    long double newInfected = (rates.beta * curr.S * curr.I * rnd) / N;

    if (newInfected > next.S) {
        next.S = 0;
        next.E += curr.S - (rnd * rates.sigma * curr.E);
    } else {
        next.S += -newInfected;
        next.I += newInfected - rnd * rates.alpha * curr.I;
    }
    next.R += rnd * rates.alpha * curr.I;

    // Calculate change in current step
    derr.S = next.S - curr.S;
    derr.I = next.I - curr.I;
    derr.R = next.R - curr.R;

    // Update values
    curr.S = next.S;
    curr.I = next.I;
    curr.R = next.R;
}

void sirModel::calculateSEIRD() {
    // Calculate new values
    long double rnd = ((double)rand() / RAND_MAX) * 2; // pseudorandom float on <0,2>
    long double newInfected = (rates.beta * curr.S * curr.I * rnd) / N;

    if (newInfected > next.S) {
        next.S = 0;
        next.E += curr.S - (rnd * rates.sigma * curr.E);
    } else {
        next.S += -newInfected;
        next.E += newInfected - (rnd * rates.sigma * curr.E);
    }
    next.I += rnd * (rates.sigma * curr.E - rates.alpha * curr.I - rates.omega * curr.I);
    next.R += rnd * rates.alpha * curr.I;
    next.D += rnd * rates.omega * curr.I;

    // Calculate change in current step
    derr.S = next.S - curr.S;
    derr.E = next.E - curr.E;
    derr.I = next.I - curr.I;
    derr.R = next.R - curr.R;
    derr.D = next.D - curr.D;

    // Update values
    curr.S = next.S;
    curr.E = next.E;
    curr.I = next.I;
    curr.R = next.R;
    curr.D = next.D;
}

int sirModel::simulateSIR() {
    ofstream dataFile;

    dataFile.open(filenames.SIR);
    dataFile << "S,I,R\n"; // CSV header
    
    curr.S = next.S = (N - next.I); // / N;
    //curr.I = next.I /= N;
    //curr.R = next.R /= N;

    for (unsigned long i = 0; i < steps; i++) { // Simulation start
        dataFile
                << round(curr.S) << ","
                << round(curr.I) << ","
                << round(curr.R) << endl;
        calculateSIR();
    }
    dataFile.close();
    return 0;
}

int sirModel::simulateSEIRD() {
    ofstream dataFile;

    dataFile.open(filenames.SEIRD);
    dataFile << "S,E,I,R,D\n"; // CSV header

    // S + I + E + R + D = 1
    curr.S = next.S = (N - next.I - next.E); // / N;  // Susceptible = Population - Infected - Exposed
    // curr.I = next.I /= N;
    // curr.E = next.E /= N;
    // curr.R = next.R /= N;
    // curr.D = next.D /= N;
    
    for (unsigned long i = 0; i < steps; i++) { // Simulation start
        dataFile
            << round(curr.S) << ","
            << round(curr.E) << ","
            << round(curr.I) << ","
            << round(curr.R) << ","
            << round(curr.D) << endl;
        calculateSEIRD();
    }
    dataFile.close();
    return 0;
}

void sirModel::exp1() {
    N = 5000000;

    curr.I = next.I = 10;
    rates.beta = 0.5;
    rates.alpha = 0.1;

    steps = 100;

    simulateSIR();
}

void sirModel::exp2() {
    N = 3600;

    curr.I = next.I = 0;
    curr.E = next.E = 1;
    rates.beta = 0.1 * 10; // probability of meeting a new person * number of people that infected person met
    rates.alpha = 0.095;
    rates.sigma = 0.143;
    rates.omega = 0.0034;

    steps = 100;

    simulateSEIRD();
}
