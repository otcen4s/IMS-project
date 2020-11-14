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

using namespace std;

void sirModel::print_help() {
    std::cout <<
              "-i [infected]             Initial count of infected.\n"
              "-p [population]           Initial count of population.\n"
              "-b [transmission_rate]    Transmission rate of infection COVID-19.\n"
              "-a [recovery_rate]        Recovery rate from infection COVID-19.\n"
              "-s [steps]                Number of steps of simulation.\n"
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
                        if(strtol(optarg, nullptr, 10) == 1) modelSEIRD = true; break;

                    default:
                        print_help();
                    }

        }
    }
}

void sirModel::calculateSIR() {
    // Calculate new values
    long double rnd = ((double)rand() / RAND_MAX) * 2; // pseudorandom float on <0,2>
    next.S += -rates.beta * curr.S * curr.I * rnd;
    next.I += rates.beta * curr.S * curr.I * rnd - rates.alpha * curr.I;
    next.R += (rates.alpha * curr.I);

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
    next.S += (-rates.beta * curr.S * curr.I);
    next.E += ((rates.beta * curr.S * curr.I)) - (rates.sigma * curr.E);
    next.I += (rates.sigma * curr.E) - (rates.alpha * curr.I) - rates.omega * curr.I;
    next.R += rates.alpha * curr.I;
    next.D += rates.omega * curr.I;

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

    curr.S = next.S = 1000 - next.I / N * 1000; // Susceptible = Population - Infected
    for (unsigned long i = 0; i < steps; i++) { // Simulation start
        dataFile
                << round(curr.S * N / 1000) << ","
                << round(curr.I * N / 1000) << ","
                << round(curr.R * N / 1000) << endl;
        calculateSIR();
    }
    dataFile.close();
    return 0;
}

int sirModel::simulateSEIRD() {
    ofstream dataFile;

    dataFile.open(filenames.SEIRD);
    dataFile << "S,E,I,R,D\n"; // CSV header

    curr.S = next.S = N - next.I - next.E;  // Susceptible = Population - Infected - Exposed
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
    N = 35000;

    curr.I = next.I = 1;

    rates.beta = 0.001;
    rates.alpha = 0.1;

    steps = 60;

    simulateSIR();
}

void sirModel::exp2() {
    N = 10000;

    curr.I = next.I = 3;

    rates.beta = 0.0001;
    rates.alpha = 0.1;
    rates.sigma = 1;
    rates.omega = 0.01;

    steps = 200;

    simulateSEIRD();
}
