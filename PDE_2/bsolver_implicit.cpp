#include "bsolver_implicit.hpp"

int main(){
    int nx=20;
    double dt=0.5;
    int ntmax=10;
    int ntsave=1;
    auto U0=[](double x){return 0;};
    std::string filename="プログラム演習/PDE_2/Data/heat1d_implicit20.dat";
    Bsolver_implicit solver(nx,dt,U0);
    solver.Initialize();
    solver.LBC()=1;
    for(int i=0;i<ntmax;i++){
        solver.step();
        if((i+1)%ntsave==0) solver.Write(filename);
    }
    return 0;
}