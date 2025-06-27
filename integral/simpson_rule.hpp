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

double new_integrand(double th, double U){
    double a = 0.0;
    double b = 2.0 * M_PI;
    double m = (a + b) / 2.0;
    return integrand(a,U)*(th - m)*(th - b)/
    ((a - m)*(a - b)) + integrand(b,U)*(th - a)*(th - m)/
    ((b - a)*(b - m)) + integrand(m,U)*(th - a)*(th - b)/
    ((m - a)*(m - b));
}

double simpson_rule(int N, double U){
    double dth = 2.0 * M_PI / N;
    double sum = 0.0;
    for(int i=0; i<N; i++){
        double y0 = new_integrand(i * dth, U);
        double y1 = new_integrand((i + 1) * dth, U);
        sum += (y0 + y1) * dth / 2.0;
    }
    sum += exp(-U * U) - 1.0;
    return sum;
}