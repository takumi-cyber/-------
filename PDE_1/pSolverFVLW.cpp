#include "pSolverFVLW.hpp"

int main(){
    size_t nx=20;
    double dt=0.8/nx;
    size_t ntmax=5/dt;
    size_t ntsave=5/dt;
    auto I_LEVEQUE=[](double x){
        double b=200;
        double xc=x-std::floor(x+0.2)-0.3;
        double y=exp(-b*xc*xc);
        if ((xc>0.3)&&(xc<0.5)) y+=1;
        return y;
    };
    std::string filename="プログラム演習/PDE_1/Data/FVLW20.dat";
    pSolverFVLW FVsolver(nx,dt,I_LEVEQUE);
    FVsolver.Initialize();
    for(size_t nt=0;nt<ntmax;nt++){
        FVsolver.Step();
        if((nt+1)%ntsave==0)FVsolver.Write(filename);
    }
}