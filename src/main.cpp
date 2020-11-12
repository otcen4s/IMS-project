#include "main.h"
#include <cmath>

using namespace std;

/// Globals
// long double
//          S = 9999.0,    I = 1.0, R = 0.0,
//       Sold = 9999.0, Iold = 1.0;
// long double transmissionRate = 0.00001, recoveryRate = 0.0001;


/**
 * Globals
 */

long double
         S = 0.0,    I = 0.0, R = 0.0,
      Sold = 0.0, Iold = 0.0;

long double transmissionRate = 0.0, recoveryRate = 0.0;
long double Population = 0.0;
ulong steps = 0;
string filename = "data.csv";


/// Next step
void calculateSIR() {
    S -= (Sold * Iold * transmissionRate);

    I += (Sold * Iold * transmissionRate);
    I -= (recoveryRate * Iold);

    R += (recoveryRate * Iold);

    Sold = S;
    Iold = I;
}


/**
 * Function prints help guide to stdout and terminates the program successfully
 */
void print_help() {
    std::cout <<
              "-i [infected]             Initial count of infected.\n"
              "-p [population]           Initial count of population.\n"
              "-b [transmission_rate]    Transmission rate of infection COVID-19.\n"  
              "-a [recovery_rate]        Recovery rate from infection COVID-19.\n"      
              "-s [steps]                Number of steps of simulation.\n"
              "-h [help]                 Help guide for program usage.\n";
    exit(0);
}


/**
 * Function parses the given arguments from stdin using getopt
 */
void parseArgs(int argc, char **argv) {
    int opt = 0;

    while(true){
        opt = getopt(argc, argv, "i:p:b:a:s:h"); // i - infected; p - population; b(beta coeficient) - transmission rate; a(alpha coeficient) - recovery rate; s - steps
        
        if(opt != -1) {
            switch(opt){
                case 'i':                    
                    if(optarg && optarg[0]) {
                        I = stod(optarg);
                        Iold = I;
                    }
                    break;

                case 'p':                    
                    if(optarg && optarg[0]) Population = stod(optarg);
                    break;

                case 'b':                    
                    if(optarg && optarg[0]) transmissionRate = stod(optarg);
                    break;

                case 'a':                    
                    if(optarg && optarg[0]) recoveryRate = stod(optarg);
                    break;

                case 's':                    
                    if(optarg && optarg[0]) steps = strtol(optarg, nullptr, 10);
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
    // if (argc != 2) {
    //     return 1;
    // }

    //unsigned long steps = std::strtol(argv[1], nullptr, 10);
    
    parseArgs(argc, argv);
    S = Population - I;  // Susceptible = Population - Infected
    Sold = S;

    ofstream dataFile;
    dataFile.open(filename);
    
    
    dataFile << "Population,Susceptible,Infected,Recovered\n"; // CSV head

    /// Simulation start
    for (ulong i = 0; i < steps; i++) {
        dataFile << Population << "," <<round(S) << "," << round(I) << "," << round(R) << "\n";

        //cout << i + 1 << ". S = " << round(S) << " I = " << round(I) << " R = " << round(R) << endl;
        calculateSIR();
    }

    dataFile.close();


    return 0;
}