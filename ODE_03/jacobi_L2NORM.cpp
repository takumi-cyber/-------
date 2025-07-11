#include <iostream>
#include <filesystem>
#include "jacobi.hpp"

int main(){
    std::filesystem::path mypath =getenv("HOME");
    mypath /= "プログラム演習/ODE_03";
    Jacobi JJ; 
    JJ.resize(200);
    JJ.Init();
    JJ.logname = mypath / "C02_N200.data";
    JJ.Savetimes.insert(1000);
    JJ.Savetimes.insert(2000);
    JJ.Savetimes.insert(5000);
    JJ.Savetimes.insert(10000);
    JJ.Savetimes.insert(20000);
    JJ.Savetimes.insert(25000);
    JJ.Solve(JJ.logname);
    
    return 0;
};