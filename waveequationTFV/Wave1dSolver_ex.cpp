#include "Wave1dSolver.hpp"
#include <sstream>
#include <iomanip>

int main(){
    int nx=1000;
    double dt =0.8/nx;
    double c=1.0;
    double nu=1000.0;
    double mu=500.0;
    int ntmax=1.0/dt;
    int ntsave=0.25/dt;
    auto U0=[](double x ){
        if(0.4<=x && x<=0.6) return 1.0;
        else return 0.0;
    };
    Wave1dSolver solver(nx,dt,c,nu,mu,U0);
    for(int nt=0;nt<=ntmax;nt++)
    {
        if(nt % ntsave == 0)
        {
            std::ostringstream oss;
            oss << "プログラム演習/waveequationTFV/Data/new_2/step_CFL0.8_traditional_t"
                << std::fixed << std::setprecision(2)
                << solver.time << "_nu1000.0mu500.0.dat";

            solver.write(oss.str());
        }
        solver.solve();
    }
}