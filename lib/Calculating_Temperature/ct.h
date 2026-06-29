#include <cmath>

double result;

double result1;




double calculateT(double reqtemp)

{

    result = (-log((reqtemp - 22.00) / (100.00 - 22.00))) / 0.00047632; // reqtemp = wished endtemperature; Ambient temperature = 22; Initial temperature = 100; Cooling coefficient(k) = 0.00047632; k = h*A/C

    result1 = result / 60.00;                                           // change the result from seconds to minutes

}