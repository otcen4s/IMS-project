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
                    exit(1);
                }

                switch (opt) {
                    case 'i': // infected
                        I = dI = stod(optarg); break;

                    case 'p': // population
                        N = stod(optarg); break;

                    case 'b': // transmission
                        beta = stod(optarg); break;

                    case 'a': // recovery
                        alpha = stod(optarg); break;

                    case 'e': // exposed
                        E = dE = stod(optarg); break;

                    case 'o': // fatality
                        omega = stod(optarg); break;

                    case 's': // steps
                        steps = strtol(optarg, nullptr, 10); break;

                    case 'm': // infectiveness
                        sigma = stod(optarg); break;

                    case 'x': // SIR or SEIRD ?
                        if(strtol(optarg, nullptr, 10) == 1) modelSEIRD = true; break;

                    default:
                        print_help();
                        exit(1);
                    }

        }
    }
}

void sirModel::calculateSIR() {
    dS += (-beta * S * I) / N;
    dI += ((beta * S * I) / N) - alpha * I;
    dR += (alpha * I);

    // dI += (S * I * beta);
    // dI -= (alpha * I);
    // dR += (alpha * I);

    S = dS;
    I = dI;
}

void sirModel::calculateSEIRD() {
    dS += (- beta * S * I) / N;
    dE += ((beta * S * I) / N) - (sigma * E);
    dI += (sigma * E) - (alpha * I) - omega * I; // (- omega * I) -> berie do uvahy umrtia
    dR += alpha * I;
    dD += omega * I;

    D = dD;
    S = dS;
    E = dE;
    I = dI;
}

int sirModel::performSimulation() {
    ofstream dataFile;

    if(modelSEIRD) {
        dataFile.open(SEIRD);
        dataFile << "S,E,I,R,D\n"; // CSV header

        S = dS = N - dI - dE;  // Susceptible = Population - Infected - Exposed
        for (unsigned long i = 0; i < steps; i++) { // Simulation start
            dataFile
                << round(dS) << ","
                << round(dE) << ","
                << round(dI) << ","
                << round(dR) << ","
                << round(dD) << "\n";

            calculateSEIRD();
        }
        dataFile.close();
    }
    else {
        dataFile.open(SIR);
        dataFile << "S,I,R\n"; // CSV header

        S = dS = N - dI;  // Susceptible = Population - Infected
        for (unsigned long i = 0; i < steps; i++) { // Simulation start
            dataFile
                << round(dS) << ","
                << round(dI) << ","
                << round(dR) << "\n";

            calculateSIR();
        }

        dataFile.close();
    }

    return 0;
}
