#include "Wave1dSolver_ex.hpp"
#include <sstream>
#include <iomanip>

int main(){
    int nx=625;
    double dt =0.8/nx;
    double c=1.0;
    double nu=1000.0;
    double mu=500.0;
    int ntmax=1.0/dt;
    int ntsave=1.0/dt;
    auto U0=[](double x ){
        return std::sin(2.0 * M_PI * x);
    };
    Wave1dSolver solver(nx,dt,c,nu,mu,U0);
    for(int nt=0;nt<=ntmax;nt++)
    {
        if(nt % ntsave == 0)
        {
            std::ostringstream oss;
            oss << "プログラム演習/waveequationTFV/Data/new_2/sin_CFL0.8_t"
                << std::fixed << std::setprecision(2)
                << solver.time << "_nu1000.0mu500.0_ex_625.dat";

            solver.write(oss.str());
        }
        solver.solve();
    }
}