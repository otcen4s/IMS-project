#include "main.h"

using namespace std;

/// Globals
long double
         S = 9999.0,    I = 1.0, R = 0.0,
      Sold = 9999.0, Iold = 1.0;

long double transmissionRate = 0.00001, recoveryRate = 0.0001;

/// Next step
void calculateSIR() {
    S -= (Sold * Iold * transmissionRate);

    I += (Sold * Iold * transmissionRate);
    I -= (recoveryRate * Iold);

    R += (recoveryRate * Iold);

    Sold = S;
    Iold = I;
}

int main(int argc, char** argv){
    if (argc != 2) {
        return 1;
    }

    unsigned long steps = std::strtol(argv[1], nullptr, 10);
    /// Simulation start
    for (unsigned long i = 0; i < steps; i++) {
        cout << i + 1 << " S = " << round(S) << " I = " << round(I) << " R = " << round(R) << endl;
        calculateSIR();
    }

    return 0;
}