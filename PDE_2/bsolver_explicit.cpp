#include "bsolver_explicit.hpp"

int main(){
    int nx=20;
    double dt=0.1/nx/nx;
    int ntmax=5/dt;
    int ntsave=0.005/dt;
    auto U0=[](double x){return 0;};
    std::string filename="プログラム演習/PDE_2/Data/heat1d_explicit20.dat";
    Bsolver_explicit solver(nx,dt,U0);
    solver.LBC()=1.0;
    for(int i=0;i<ntmax;i++){
        solver.step();
        if((i+1)%ntsave==0) solver.Write(filename);
    }
    return 0;
}