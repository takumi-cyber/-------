#include "pSolverTFV_LW.hpp"

int main(){
    int nx=1000;
    double dt=10.8/nx;
    double c=1.0;
    int ntmax=1/dt;
    int ntsave=0.5/dt;
    auto I_LEVEQUE=[](double x){
        double b=200;
        double xc=x-std::floor(x+0.2)-0.3;
        double y=exp(-b*xc*xc);
        if ((xc>0.3)&&(xc<0.5)) y+=1;
        return y;
    };
    std::string filename="プログラム演習/PDE_1/Data/TFVLW1000_CFL10.8.dat";
    pSolverTFVLW TFVsolver(nx,dt,c,I_LEVEQUE);
    for(int nt=0;nt<ntmax;nt++){
        TFVsolver.step();
        if((nt+1)%ntsave==0) TFVsolver.Write(filename);
    }
}