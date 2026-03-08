#include "Heat2dSolver.hpp"
#include <sstream>

int main(){
    size_t n=101;
    double dt=0.001;
    double k=0.1;
    int ntmax=200;
    int ntsave=10;
    auto U0=[](double x, double y){
        if((0.4<=x)&&(x<=0.6)&&(0.4<=y)&&(y<=0.6)) return 1.0;
        else return 0.0;
    };
    Heat2dSolver solver(n,dt,k,U0);
    for(int i=0;i<=ntmax;i++){
        if(i%ntsave==0){
            std::ostringstream ss;
            ss << "プログラム演習/PDE_3/Data/heat2d_" 
               << std::setw(4) << std::setfill('0') << i/ntsave
               << ".vtk";
            solver.write(ss.str());
        }
        solver.solve();
    }
}