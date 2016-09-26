#include <iostream>
#include <string>
using namespace std;

int main()
{
        // Tax rates and threshholds
    const double RATE_FIRST_200M = 0.041;
    const double RATE_NEXT_300M = 0.053;
    const double RATE_NEXT_300M_NJNY = 0.057;
    const double RATE_OVER_500M = 0.06;
    const double THRESHHOLD_1 = 200;
    const double THRESHHOLD_2 = 500;
    
        // Market value
    cout << "Market value (in millions): ";
    double marketValue;
    cin >> marketValue;
    cin.ignore(9999, '\n');
    if (marketValue < 0)
    {
        cout << "---" << endl << "The market value must be nonnegative." << endl;
        return 1;
    }
    
        // Identification
    cout << "Identification: ";
    string identification;
    getline(cin, identification);
    if (identification == "")
    {
        cout << "---" << endl << "You must enter a property identification." << endl;
        return 1;
    }
    
        //State
    cout << "State: ";
    string state;
    getline(cin, state);
    if (state == "")
    {
        cout << "---" << endl << "You must enter a state." << endl;
        return 1;
    }
    
        // Calculating and printing tax
    double tax;
    cout.setf(ios::fixed);
    cout.precision(3);
    
    if (marketValue <= THRESHHOLD_1)
        tax = marketValue * RATE_FIRST_200M;
    
    else if (marketValue <= THRESHHOLD_2 && (state == "NJ" || state == "NY"))
        tax = THRESHHOLD_1 * RATE_FIRST_200M + (marketValue - THRESHHOLD_1) * RATE_NEXT_300M_NJNY;
    
    else if (marketValue <= THRESHHOLD_2)
        tax = THRESHHOLD_1 * RATE_FIRST_200M + (marketValue - THRESHHOLD_1) * RATE_NEXT_300M;
    
    else if (marketValue > THRESHHOLD_2 && (state == "NJ" || state == "NY"))
        tax = THRESHHOLD_1 * RATE_FIRST_200M + (THRESHHOLD_2 - THRESHHOLD_1) * RATE_NEXT_300M_NJNY + (marketValue - THRESHHOLD_2) * RATE_OVER_500M;
    
    else if (marketValue > THRESHHOLD_2)
        tax = THRESHHOLD_1 * RATE_FIRST_200M + (THRESHHOLD_2 - THRESHHOLD_1) * RATE_NEXT_300M + (marketValue - THRESHHOLD_2) * RATE_OVER_500M;
    
    cout << "---" << endl << "The tax for " << identification << " is $" << tax << " million." << endl;

}