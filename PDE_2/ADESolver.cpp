#include "ADESolver.hpp"

int main(){
    int nx=1000;
    double dt=0.8/nx;
    double k=0.001;
    int ntmax=5/dt;
    int ntsave=0.05/dt;
    auto LEVEQUE=[](double x)
    {
       double b=200;
       double xc=x-std::floor(x+0.2)-0.3;
       double y=exp(-b*xc*xc);
       if ((xc>0.3)&&(xc<0.5)) y+=1;
       return y;
    };
    ADESolver solver(nx,dt,k,LEVEQUE);
    solver.make();
    for(int i=0;i<ntmax;i++){
        solver.step();
        if((i+1)%ntsave==0) solver.Write("プログラム演習/PDE_2/Data/ade_k0.001.dat");
    }
    return 0;

}