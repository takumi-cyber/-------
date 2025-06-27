#include <iostream>
#include <cmath>

double integrand(double th, double U){
    double UC = U * cos(th);
    double US = U * sin(th);
    double ERF = erf(UC);
    double EXP = exp(-US * US);
    double COEF = M_2_SQRTPI/4.0;
    return COEF * EXP * ERF * UC;
}

double trapezoidal_rule(int N, double U){
    double dth = 2.0 * M_PI / N;
    double sum = 0.0;
    for(int i = 0; i<N; i++){
    double y0 = integrand(i * dth, U);
    double y1 = integrand((i + 1) * dth, U);
    sum += (y0 + y1) * dth / 2.0;
    }
    sum += exp(-U * U) - 1.0; 
    return sum;
}
