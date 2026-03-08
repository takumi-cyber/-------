#include "Wave1dSolver_TFV_spcase.hpp"
#include <cmath>

int main(){ 
    int nx=1000;
    double dt=5.0/nx; 
    double c=1.0;
    int ntmax=1.0/dt;
    int ntsave=0.25/dt;
    auto U0 = [](double x) { 
        return std::sin(2*M_PI*x);
    };
    Wave1dSolver solver(nx,dt,c,U0);
    for(int nt=0;nt<=ntmax;nt++)
    {
        if(nt%ntsave==0)
        solver.write("プログラム演習/waveequationTFV/Data/spcase/sin_CFL5.0.dat");
        solver.solve();
    }
}