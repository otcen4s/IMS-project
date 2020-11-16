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
#include "headers/Model.h"

using namespace std;

void Model::print_help() {
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

void Model::parseArgs(int argc, char **argv) {
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
                        rates.beta = rates.betaNative = stod(optarg); break;

                    case 'a': // recovery
                        rates.alpha = stod(optarg); break;

                    case 'e': // exposed
                        curr.E = next.E = stod(optarg); break;

                    case 'o': // fatality
                        rates.omega = stod(optarg); break;

                    case 'm': // infectivity
                        rates.sigma = stod(optarg); break;

                    case 's': // steps
                        days = strtol(optarg, nullptr, 10); break;

                    case 'x': // SIR or SEIRD ?
                        if(strtol(optarg, nullptr, 10) == 1) {modelSEIRD = true;} break;

                    default:
                        print_help();
                    }
        }
    }
}

void Model::calculateSIR() {
    // Calculate new values
    long double rnd = ((double)rand() / RAND_MAX) * 2; // pseudorandom float on <0,2>
    long double newInfected = (rates.beta * curr.S * curr.I * rnd) / N;

    if (newInfected > next.S) {
        next.S = 0;
        next.I += curr.S - (rnd * rates.alpha * curr.I);
    } else {
        next.S += -newInfected;
        next.I += newInfected - rnd * rates.alpha * curr.I;
        stats.sumInfected += newInfected;
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

void Model::calculateSEIRD() {
    // Calculate new values
    long double rnd = ((double)rand() / RAND_MAX) * 2; // pseudorandom float on <0,2>
    long double newInfected = (rates.beta * curr.S * curr.I * rnd) / N;

    if (newInfected > next.S) {
        next.S = 0;
        next.E += curr.S - (rnd * rates.sigma * curr.E);
    } else {
        next.S += -newInfected;
        next.E += newInfected - (rnd * rates.sigma * curr.E);
        stats.sumInfected += newInfected;
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

int Model::simulateSIR() {
    // Everybody who isn't infected can be at the start of an epidemic
    curr.S = next.S = (N - next.I);
    stats.sumInfected = next.I;

    printInfo("header", false);

    // Handle output file
    output.open(filenames.SIR);
    output << "S,I,R\n"; // CSV header
    for (unsigned long i = 0; i < days; i++) { // Simulation start
        // Enforce measures
        if (i % 7 == 0 && restrictions) {
            rates.beta = rates.betaNative;
            setRestriction();
        }

        // Track max infected people (graph spike)
        if (round(curr.I) > stats.maxInfected) {
            stats.maxInfected = round(curr.I);
            stats.dayMaxInfected = i + 1ul;
        }

        // Track the biggest increment in infected people
        if (round(derr.I) > stats.maxIncrement) {
            stats.maxIncrement = round(derr.I);
            stats.dayMaxIncrement = i + 1ul;
        }

        output << round(curr.S) << "," << round(curr.I) << "," << round(curr.R) << endl;
        calculateSIR();
    }

    printInfo("footer", false);
    output.close();
    return 0;
}

int Model::simulateSEIRD() {
    curr.S = next.S = (N - next.I - next.E); // / N;  // Susceptible = Population - Infected - Exposed

    printInfo("header", true);

    // Handle output file
    output.open(filenames.SEIRD);
    output << "S,E,I,R,D\n"; // CSV header
    for (unsigned long i = 0; i < days; i++) { // Simulation start
        // Enforce measures
        if (i % 7 == 0 && restrictions) {
            rates.beta = rates.betaNative;
            setRestriction();
        }

        // Track max infected people (graph spike)
        if (round(curr.I) > stats.maxInfected) {
            stats.maxInfected = round(curr.I);
            stats.dayMaxInfected = i + 1ul;
        }

        // Track the biggest increment in infected people
        if (round(derr.I) > stats.maxIncrement) {
            stats.maxIncrement = round(derr.I);
            stats.dayMaxIncrement = i + 1ul;
        }

        output
            << round(curr.S) << ","
            << round(curr.E) << ","
            << round(curr.I) << ","
            << round(curr.R) << ","
            << round(curr.D) << endl;
        calculateSEIRD();
    }

    printInfo("footer", true);
    output.close();
    return 0;
}

// SIR exp
void Model::exp1() {
    N = 5000000;

    curr.I = next.I = 1;
    rates.beta = rates.betaNative = 0.6;
    rates.alpha = 0.095;
    days = 100;

    simulateSIR();
}

// SIR exp
void Model::exp2() {
    N = 5000000;

    curr.I = next.I = 1;
    rates.beta = rates.betaNative = 0.6; // probability of meeting a new person * number of people that infected person met
    rates.alpha = 0.095;
    restrictions = true;

    days = 400;

    simulateSIR();
}

// SEIRD exp
void Model::exp3() {
    N = 5000000;

    curr.I = next.I = 1;
    rates.beta = rates.betaNative = 0.6; // probability of meeting a new person * number of people that infected person met
    rates.alpha = 0.095;
    rates.sigma = 0.143;
    rates.omega = 0.0034;

    days = 200;

    simulateSEIRD();
}

// SEIRD exp
void Model::exp4() {
     N = 5000000;

    curr.I = next.I = 0;
    curr.E = next.E = 100;
    rates.beta = rates.betaNative = 0.6; // probability of meeting a new person * number of people that infected person met
    rates.alpha = 0.095;
    rates.sigma = 0.143;
    rates.omega = 0.0034;
    restrictions = true;

    days = 400;

    simulateSEIRD();
}

void Model::setRestriction() {
    // M4 = only trips to work and for groceries are allowed
    if (curr.I >= N * 0.05) { rates.beta = 0.001; return; }

    // M3 = masks in exterior
    if (curr.I >= N * 0.02) { rates.beta *= 0.3; return; }

    // M2 = masks in interior
    if (curr.I >= N * 0.01) { rates.beta *= 0.5; return; }

    // M1 = closed schools, cinemas
    if (curr.I >= N * 0.001) { rates.beta *= 0.75; }
}

void Model::printInfo(const string& which, bool SEIRD) {
    // Inputs
    if (which == "header") {
        cout << "Input: " << endl;
        cout << "\tPopulation count = " << N << endl;
        cout << "\tInfected = " << curr.I << endl;
        if (SEIRD) {
            cout << "\tExposed = " << curr.E << endl;
        }
        cout << "\tTransmission rate = " << rates.beta << endl;
        cout << "\tRecovery rate = " << rates.alpha << endl;
        if (SEIRD) {
            cout << "\tInfectivity = " << rates.sigma << endl;
            cout << "\tFatality rate = " << rates.omega << endl;
        }
        cout << "\tGovernment takes measures = " << (restrictions ? "YES" : "NO") << endl;
        cout << "\tDays of simulation = " << days << endl;
    }

    // Outputs
    if (which == "footer") {
        cout << "Output: " << endl;
        cout << "\tSum of all the infected = " << stats.sumInfected << "(" << stats.sumInfected / N * 100 << "%)" << endl;
        cout << "\tBiggest daily increment = " << stats.maxIncrement << "(" << stats.dayMaxIncrement << ")" << endl;
        cout << "\tMost infected people at single moment = " << stats.maxInfected << "(" << stats.dayMaxInfected << ")" << endl;
        if (SEIRD) {
            cout << "\tDead = " << curr.D << endl;
        }
    }
}
