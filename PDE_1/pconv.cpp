#include "pconv.hpp"

int main(){
    size_t ntmax=50;
    size_t ntsave=50;
    pconv pde_solver(320,0.02,[](double x){ return std::pow(sin(2*M_PI*x),5); });
    pde_solver.Write("プログラム演習/PDE_1/pconv_result.txt");
    for (size_t nt=0;nt<ntmax;nt++){
        pde_solver.stepLQ();
        if((nt+1)%ntsave==0) pde_solver.Write();
    }
    return 0;
}