#include "Wave1dSolver_TFV.hpp"
#include <cmath>

int main(){
    int nx=1000;
    double dt=6.25/nx;
    double c=1.0;
    int ntmax=1.0/dt;
    int ntsave=0.25/dt; 
    auto U0 = [](double x) {
        if(0.4<=x && x<=0.6) return 1.0;
        else return 0.0;
    };
    Wave1dSolver solver(nx,dt,c,U0); 
    for(int nt=0;nt<=ntmax;nt++)
    {
        if(nt%ntsave==0)
        solver.write("プログラム演習/waveequationTFV/Data/step_vis/step_6.25_0.05.dat");
        solver.solve();
    }
}