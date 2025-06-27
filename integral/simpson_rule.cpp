#include <iostream>
#include <vector>
#include "simpson_rule.hpp"

int main(){
    std::vector<int> N_list = {10, 20, 40, 80};
    double U = 1.0;
    for(int N : N_list){
        double result = simpson_rule(N, U);
        std::cout << "N = " << N << ", Result = " << result << std::endl;
    }
    return 0;
}
