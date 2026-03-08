#include "Wave1dSolver_TFV_2.hpp"

int main(){
    int nx=1000;
    double dt=1.0/nx;
    double c=1.0;
    int ntmax=1.0/dt;
    int ntsave=0.25/dt; 
    auto U1 =[](double x){
        return 0.0;
    };
    auto U0 = [](double x) {
        return std::sin(2.0*M_PI*x);
    };

    Wave1dSolver solver(nx,dt,c,U0,U1); 
    for(int nt=0;nt<=ntmax;nt++)
    {
        if(nt%ntsave==0)
        solver.write("プログラム演習/waveequationTFV/Data/test6.dat");
        solver.solve();
    }
}