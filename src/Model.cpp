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
    // long double rnd = ((double)rand() / RAND_MAX) * 2; // pseudorandom float on <0,2>
    long double newInfected = (rates.beta * curr.S * curr.I) / N;

    if (newInfected > next.S) {
        next.S = 0;
        next.E += curr.S - ( rates.sigma * curr.E);
    } else {
        next.S += -newInfected;
        next.E += newInfected - (rates.sigma * curr.E);
        stats.sumInfected += newInfected;
    }
    next.I += (rates.sigma * curr.E - rates.alpha * curr.I - rates.omega * curr.I);
    next.R +=  rates.alpha * curr.I;
    next.D += rates.omega * curr.I;

    stats.sumRecovered += rates.alpha * curr.I;

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
    curr.S = next.S = (N - next.I - next.E);  // Susceptible = Population - Infected - Exposed
    stats.sumInfected = next.I;

    printInfo("header", true);

    // Handle output file
    output.open(filenames.SEIRD);
    output << "S,E,I,R,D,Isum,Rsum\n"; // CSV header
    for (unsigned long i = 0; i < days; i++) { // Simulation start
        // Enforce measures
        // if (i % 7 == 0 && restrictions) {
        //     rates.beta = rates.betaNative;
        //     setRestriction();
        // }
        
        // China province Hubei took drastic government measures in January 23rd with all cities quarantined
        // Basic reproduction number(R0) according to studies dropped under 1
        // Approximately after the 2 months of Covid19 spreadin Hubei, China took restrections 
        
       
        // if(i == 60){
        //     rates.R0 = 0.8;
        //     rates.beta = rates.alpha * rates.R0;
       // }

        if(i == 60 && restrictions){
            rates.R0 = 0.8;
            rates.beta = rates.alpha * rates.R0;
        }
        if(i == 23 && restrictions){
            rates.R0 = 6.6037;
            rates.beta = rates.alpha * rates.R0;
        }
         if(i == 27 && restrictions){
            rates.R0 = 3.7732;
            rates.beta = rates.alpha * rates.R0;
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
            << round(curr.D) << ","
            << round(stats.sumInfected) << ","
            << round(stats.sumRecovered) << endl;
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
    rates.beta = rates.betaNative = 0.6; // probability of meeting a new person * number of people that infected person met
    rates.alpha = 0.095;
    
    days = simulationDays();

    simulateSIR();
}

// SIR exp
void Model::exp2() {
    N = 5000000;

    curr.I = next.I = 1;
    rates.beta = rates.betaNative = 0.6; // probability of meeting a new person * number of people that infected person met
    rates.alpha = 0.095;
    restrictions = true;

    days = simulationDays();

    simulateSIR();
}

// SEIRD exp
void Model::exp3() {
    N = 58500000;

    curr.I = next.I = 1;
    curr.E = next.E = 20 * curr.I; // estimated by scientific paper of Mr.Wang the initial number of exposed is 20 times greater than the number infected
    rates.R0 = 6;
    rates.alpha = 0.0556; // 1 / hospitalization period -> hospitalization period was estimated as 18 days
    rates.beta = rates.betaNative = rates.alpha * rates.R0; // R0 * alpha -> R0 is initial basic infection-reproduction number and aplha is recovery rate
    rates.sigma = 0.1923; // 1 / mean incubation period -> mean incubation period is set to 5.2 days
    rates.omega = 0.0034;

    days = simulationDays();

    simulateSEIRD();
}

// SEIRD exp
void Model::exp4() {
    restrictions = true;
    
    N = 58500000;

    curr.I = next.I = 1;
    curr.E = next.E = 20 * curr.I; // estimated by scientific paper of Mr.Wang
    rates.R0 = 6;
    rates.alpha = 0.0556; // 1 / hospitalization period -> hospitalization period was estimated as 18 days
    rates.beta = rates.betaNative = rates.alpha * rates.R0; // R0 * alpha -> R0 is initial basic infection-reproduction number and aplha is recovery rate
    rates.sigma = 0.1923; // 1 / mean incubation period -> mean incubation period is set to 5.2 days
    rates.omega = 0.0034;

    days = simulationDays();

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

int Model::simulationDays() {
    const int monthDays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

    // start of Covid19 in chinese province Hubei
    int yearStart = 2019;
    int monthStart = 12;
    int dayStart = 31;

    // simulating until
    int yearEnd = 2020;
    int monthEnd = 12;
    int dayEnd = 1;

    auto n1 = yearStart * 365 + dayStart;
    for (int i = 0; i < monthStart - 1; i++) n1 += monthDays[i];

    auto n2 = yearEnd * 365 + dayEnd;
    for (int i = 0; i < monthEnd - 1; i++) n2 += monthDays[i];

    return (n2 - n1);
}

void Model::printInfo(const string& which, bool SEIRD) {
    cout << fixed;
    cout.precision(5);
    // Inputs
    if (which == "header") {
        cout << endl;
        cout << "------------------------------------------------------------------------------------------" << endl;
        cout << "|Simulation of SIR and SEIRD epidemic models of COVID19 disease in chinese province Hubei|" << endl;
        cout << "------------------------------------------------------------------------------------------" << endl << endl;
        cout << "Input: " << endl;
        cout << "\tPopulation count = " << (int)N << endl;
        cout << "\tInfected = " << (int)round(curr.I) << endl;
        if (SEIRD) {
            cout << "\tExposed = " << (int)round(curr.E) << endl;
        }
        cout << "\tBasic reproduction number of COVID19 = " << rates.R0 << endl;
        cout << "\tTransmission rate = " << rates.beta << endl;
        cout << "\tRecovery rate = " << rates.alpha << endl;
        if (SEIRD) {
            cout << "\tInfectivity = " << rates.sigma << endl;
            cout << "\tFatality rate = " << rates.omega << endl;
        }
        cout << "\tGovernment takes measures = " << (restrictions ? "YES" : "NO") << endl;
        cout << "\tFirst reported COVID19 case was in 31 December 2019" << endl;
        cout << "\tDays of simulation until now = " << days << endl;
    }

    // Outputs
    if (which == "footer") {
        cout << "Output: " << endl;
        cout << "\tSum of all the recovered = " << (int)round(stats.sumRecovered) << "(" << stats.sumRecovered / N * 100 << "% of total population of Hubei)" << endl;
        cout << "\tSum of all the infected = " << (int)round(stats.sumInfected) << "(" << stats.sumInfected / N * 100 << "% of total population of Hubei)" << endl;
        cout << "\tBiggest daily increment = " << (int)round(stats.maxIncrement) << "(day No. " << stats.dayMaxIncrement << ")" << endl;  // TODO vysvetlit trochu prehladnejise cislo v zatvorke
        cout << "\tMost infected people at single moment = " << (int)round(stats.maxInfected) << "(day No. " << stats.dayMaxInfected << ")" << endl;  // TODO detto
        if(restrictions) {
            cout << "\tBasic reproduction number of COVID19 dropped to : "<< rates.R0 << endl;
        }
        if (SEIRD) {
            cout << "\tDead = " << (int)round(curr.D) << endl;
        }
        cout << "------------------------------------------------------------------------------------------" << endl;
        cout << "|########################################################################################|" << endl;
        cout << "------------------------------------------------------------------------------------------" << endl << endl;
    }
}
