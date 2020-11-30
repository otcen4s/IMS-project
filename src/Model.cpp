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

int Model::simulate() {
    curr.S = next.S = (N - next.I - next.E);  // Susceptible = Population - Infected - Exposed
    stats.sumInfected = next.I;

    printHeader();

    // Handle output file
    output.open("statistics/data.csv");
    if (SIERD) {
        output << "S,E,I,R,D,Isum,Rsum\n";
    } else {
        output << "S,I,R,Isum\n";
    }

    // Simulation start
    for (unsigned long i = 0; i < days; i++) {
        // Chinese new year celebration epidemic spot
        if(i == 23 && restrictions){
            rates.R0 = 6.6037;
            rates.beta = rates.alpha * rates.R0;
        }

        // China province Hubei took drastic government measures in January 23rd with all cities quarantined
        if(i == 27 && restrictions){
            rates.R0 = 3.7732;
            rates.beta = rates.alpha * rates.R0;
        }

        // Approximately after the 2 months of Covid19 spreading Hubei, China took restrictions
        // Basic reproduction number(R0) according to studies dropped under 1
        if(i == 60 && restrictions){
            rates.R0 = 0.8;
            rates.beta = rates.alpha * rates.R0;
        }

        // Track max infected people (graph spike)
        if (round(curr.I) > stats.maxInfected) {
            stats.maxInfected = round(curr.I);
            stats.dayMaxInfected = i + 1ul;
        }

        // Track the biggest increment in infected people
        if (round(derrI) > stats.maxIncrement) {
            stats.maxIncrement = round(derrI);
            stats.dayMaxIncrement = i + 1ul;
        }

        if (SIERD) {
            output
                << round(curr.S) << ","
                << round(curr.E) << ","
                << round(curr.I) << ","
                << round(curr.R) << ","
                << round(curr.D) << ","
                << round(stats.sumInfected) << ","
                << round(stats.sumRecovered) << endl;
            nextStep();
        } else {
            output
                << round(curr.S) << ","
                << round(curr.I) << ","
                << round(curr.R) << ","
                << round(stats.sumInfected) << endl;
            nextStep();
        }
    }

    printFooter();
    output.close();
    return 0;
}

void Model::nextStep() {
    // Calculate new values
    long double newInfected = (rates.beta * curr.S * curr.I) / N;
    if (SIERD) {
        if (newInfected > next.S) {
            next.S = 0;
            next.E += curr.S - (rates.sigma * curr.E);
        } else {
            next.S += -newInfected;
            next.E += newInfected - (rates.sigma * curr.E);
            stats.sumInfected += newInfected;
        }
        next.I += (rates.sigma * curr.E - rates.alpha * curr.I - rates.omega * curr.I);
        next.D += rates.omega * curr.I;

        // Update values
        curr.E = next.E;
        curr.D = next.D;
    } else {
        if (newInfected > next.S) {
            next.S = 0;
            next.I += curr.S - (rates.alpha * curr.I);
        } else {
            next.S += -newInfected;
            next.I += newInfected - rates.alpha * curr.I;
            stats.sumInfected += newInfected;
        }
    }

    // Recover
    next.R +=  rates.alpha * curr.I;
    stats.sumRecovered += rates.alpha * curr.I;

    // Calculate change in current step
    derrI = next.I - curr.I;

    // Update values
    curr.S = next.S;
    curr.I = next.I;
    curr.R = next.R;
}

void Model::simulationDays() {
    const int monthDays[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

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

    days = n2 - n1;
}

void Model::printHeader() {
    cout << fixed;
    cout.precision(5);
    cout << "------------------------------------------------------------------------------------------" << endl;
    cout << "|Simulation of SIR and SIERD epidemic models of COVID19 disease in chinese province Hubei|" << endl;
    cout << "------------------------------------------------------------------------------------------" << endl;
    cout << endl << "Input: " << endl;
    cout << "\tPopulation count = " << (int)N << endl;
    cout << "\tInfected = " << (int)round(curr.I) << endl;
    if (SIERD) {
        cout << "\tExposed = " << (int)round(curr.E) << endl;
    }
    cout << "\tBasic reproduction number of COVID19 = " << rates.R0 << endl;
    cout << "\tTransmission rate = " << rates.beta << endl;
    cout << "\tRecovery rate = " << rates.alpha << endl;
    if (SIERD) {
        cout << "\tInfectivity = " << rates.sigma << endl;
        cout << "\tFatality rate = " << rates.omega << endl;
    }
    cout << "\tGovernment takes measures = " << (restrictions ? "YES" : "NO") << endl;
    cout << "\tFirst reported COVID19 case was in 31 December 2019" << endl;
    cout << "\tDays of simulation until now = " << days << endl;
}

void Model::printFooter() {
    cout << fixed;
    cout.precision(5);
    cout << "Output: " << endl;
    cout
        << "\tSum of all the recovered = " << (int)round(stats.sumRecovered)
        << "(" << stats.sumRecovered / N * 100 << "% of total population of Hubei)" << endl;

    cout
        << "\tSum of all the infected = " << (int)round(stats.sumInfected)
        << "(" << stats.sumInfected / N * 100 << "% of total population of Hubei)" << endl;

    cout
        << "\tBiggest daily increment = " << (int)round(stats.maxIncrement)
        << "(day No. " << stats.dayMaxIncrement << ")" << endl;

    cout
        << "\tMost infected people at single moment = " << (int)round(stats.maxInfected)
        << "(day No. " << stats.dayMaxInfected << ")" << endl;

    if(restrictions) {
        cout << "\tBasic reproduction number of COVID19 dropped to : "<< rates.R0 << endl;
    }
    if (SIERD) {
        cout << "\tDead = " << (int)round(curr.D) << endl;
    }
    cout << "------------------------------------------------------------------------------------------" << endl;
    cout << "|########################################################################################|" << endl;
    cout << "------------------------------------------------------------------------------------------" << endl;
}

int Model::exp1() {
    return simulate();
}

int Model::exp2() {
    restrictions = true;
    return simulate();
}

int Model::exp3() {
    SIERD = true;
    return simulate();
}

int Model::exp4() {
    restrictions = true;
    SIERD = true;
    return simulate();
}

int Model::performExp(int num) {
    simulationDays();
    switch (num) {
        case 1:
            return exp1();

        case 2:
            return exp2();

        case 3:
            return exp3();

        case 4:
            return exp4();

        default:
            return 1;
    }
}
