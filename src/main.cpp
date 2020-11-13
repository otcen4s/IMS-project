#include "main.h"
#include <cmath>

using namespace std;

/// Globals
// long double
//          S = 9999.0,    I = 1.0, R = 0.0,
//       Sold = 9999.0, Iold = 1.0;
// long double transmissionRate = 0.00001, recoveryRate = 0.0001;




// long double
//     S = 0.0,    I = 0.0, R = 0.0,
//     Sold = 0.0, Iold = 0.0,
//     E = 0.0, Eold = 0.0, D = 0.0;

//long double transmissionRate = 0.0, recoveryRate = 0.0, exposedRate = 0.0;
//long double Population = 0.0;



/**
 * Globals
 */

bool modelSEIRD = false;
string SEIRD = "data_SEIRD.csv";
string SIR = "data_SIR.csv";

// beta:  The rate of how often a susceptible-infected contact results in a new infection.(transmission rate)
// alpha: The rate an infected recovers and moves into the resistant phase.(recovery rate)
// sigma: The rate at which an exposed person becomes infective.
// omega: The infection fatality rate (IFR).
long double beta = 0.0, alpha = 0.0, sigma = 0.0, omega = 0.0; // coefficients
unsigned long N = 0; // population
long double S = 0.0, E = 0.0, I = 0.0, D = 0.0; // suspectible-exposed-infective
long double dS = 0.0, dE = 0.0, dI = 0.0, dR = 0.0, dD = 0.0; // new values are stored here in every step
unsigned long steps = 0; 


/**
 * S + I + R = N (Entire population)
 */
void calculateSIR() {
    dS += (-beta * S * I) / N;
    dI += ((beta * S * I) / N) - alpha * I;
    dR += (alpha * I);

    // dI += (S * I * beta);
    // dI -= (alpha * I);
    // dR += (alpha * I);

    S = dS;
    I = dI;
}

/**
 * S + E + I + R + D = N (Entire population)
 * Results of ordinary differential equations for S, E, I, R, D values at every time point of the simulation
 */
void calculateSEIRD() {
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


/**
 * Function prints help guide to stdout and terminates the program successfully
 */
void print_help() {
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


/**
 * Function parses the given arguments from stdin using getopt
 */
void parseArgs(int argc, char **argv) {
    int opt = 0;

    while(true){
        opt = getopt(argc, argv, "i:p:b:a:s:e:o:m:x:h"); // i - infected; p - population; b(beta coeficient) - transmission rate; a(alpha coeficient) - recovery rate; s - steps
        
        if(opt != -1) {
            switch(opt){
                case 'i':                    
                    if(optarg && optarg[0]) {
                        dI = stod(optarg);
                        I = dI;
                    }
                    break;

                case 'p':                    
                    if(optarg && optarg[0]) N = stod(optarg);
                    break;

                case 'b':                    
                    if(optarg && optarg[0]) beta = stod(optarg);
                    break;

                case 'a':                    
                    if(optarg && optarg[0]) alpha = stod(optarg);
                    break;

                case 'e':                    
                    if(optarg && optarg[0]) {
                        dE = stod(optarg);
                        E = dE;
                    }
                    break;
                
                case 'o':                    
                    if(optarg && optarg[0]) omega = stod(optarg);
                    break;

                case 's':                    
                    if(optarg && optarg[0]) steps = strtol(optarg, nullptr, 10);
                    break;

                case 'm':                    
                    if(optarg && optarg[0]) sigma = stod(optarg);
                    break;

                case 'x':                    
                    if(optarg && optarg[0]) if(strtol(optarg, nullptr, 10) == 1) modelSEIRD = true;
                    break;

                case 'h':
                    print_help();
                    break;

                case ':':
                    cerr << "option needs a value" << endl;
                    exit(1);

                case '?':
                    cerr << "unknown option: " << optopt << endl;
                    exit(1);
            }
        }

        else break;
    }
}



int main(int argc, char** argv){
    parseArgs(argc, argv);

    ofstream dataFile;

    if(modelSEIRD) {
        dataFile.open(SEIRD);
        dataFile << "Population,Susceptible,Exposed,Infected,Recovered,Dead\n"; // CSV head

        dS = N - dI - dE;  // Susceptible = Population - Infected - Exposed
        S = dS;
        for (unsigned long i = 0; i < steps; i++) { // Simulation start
            dataFile << N << "," << round(dS) << "," << round(dE) << "," << round(dI) << "," << round(dR) << "," << round(dD) << "\n";
            calculateSEIRD();
        }
        dataFile.close();
    }
    else {
        dataFile.open(SIR);
        dataFile << "Population,Susceptible,Infected,Recovered\n"; // CSV head
        
        dS = N - dI;  // Susceptible = Population - Infected
        S = dS;
        for (unsigned long i = 0; i < steps; i++) { // Simulation start
            dataFile << N << "," << round(dS) << "," << round(dI) << "," << round(dR) << "\n";
            calculateSIR();
        }
        dataFile.close();
    }

    return 0;
}