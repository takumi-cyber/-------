#include <iostream>
#include "LinearAdvection.hpp"

int main(){
    double c = 1.0; // Wave speed
    double xmax = 10.0; // Maximum spatial extent
    double tmax = 50; // Maximum time
    int N = 10; // Number of spatial points
    int M = 500; // Number of time steps

    LinearAdvection ae(c, xmax, tmax, N, M);
    ae.Solve();
    ae.Save("linear_advection_output.data");
    return 0;
}

