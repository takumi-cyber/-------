#include <iostream>
#include <filesystem>
#include "BC_solver.hpp"
#include "jacobi.hpp"

#define USE_JACOBI

int main(){
    std::filesystem::path mypath =getenv("HOME");
    mypath /= "プログラム演習/ODE_03";
    #ifdef USE_JACOBI
    Jacobi JJ;
    JJ.resize(200);
    JJ.Init();
    JJ.filename=mypath/"C02_N200.data";
    JJ.Savetimes.insert(1000);
    JJ.Savetimes.insert(2000);
    JJ.Savetimes.insert(5000);
    JJ.Savetimes.insert(10000);
    JJ.Savetimes.insert(20000);
    JJ.Savetimes.insert(25000);
    JJ.Solve();
    JJ.Save(mypath/"C02_N200.jac");
    JJ.Inv();
    JJ.Save(mypath/"C02_N200.inv");

    #else
    BC_Solver ode;
    ode.Solve();
    ode.Save("BC_solver_output.data");

    #endif
    return 0;
};

