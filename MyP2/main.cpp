#include<iostream>
#include "obe_euler.hpp"

int main() {
    EulerSolver solver1(1.0, 0.0, 0.01);
    solver1.solve(1.0);
    std::cout << "y(1.0) = " << solver1.getY() << std::endl;
    std::cout << "t(1.0) = " << solver1.getT() << std::endl;

    const auto& t_list = solver1.getTList();
const auto& y_list = solver1.getYList();
for (size_t i = 0; i < t_list.size(); ++i) {
    std::cout << t_list[i] << "," << y_list[i] << std::endl;
}
    return 0;   
}
