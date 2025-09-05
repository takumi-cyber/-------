#include <iomanip>
#include <iostream>
#include "intde.hpp"
int main() {
   IntDE myIntegrator;
   size_t count=0;
   double err,err_req=1e-6;
   myIntegrator.Initialize(err_req); // 初期化
   auto v=myIntegrator.Integrate([&count](double x, void *param) {
       count++;
       return 1.0 / std::sqrt(x * (1.0 - x)); 
   }, 0.0, 1.0, err);
   std::cout << "Integral value: " << std::setprecision(12) << v << ", Error: " << err
    << " requested: " << err_req << " by " << count << " steps"<< std::endl;
   return 0;
}