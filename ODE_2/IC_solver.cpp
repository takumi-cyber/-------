#include <iostream>
#include <filesystem>
#include "IC_solver.hpp"

int main(){
    std::filesystem::path myPath=getenv("HOME");
    myPath/="プログラム演習/Data/test02";
    IC_solver ode(myPath);
    ode.N=100;
    ode.Solve("C1N100.data");
    ode.N=10;
    ode.Solve("C1N10.data");
    ode.N=5;
    ode.Solve("C1N5.data");
    ode.N=3;
    ode.Solve("C1N3.data");
    return 0;
}
