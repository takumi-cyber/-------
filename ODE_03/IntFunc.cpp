#include "IntFunc.hpp"
#include <cmath>

int main() {
   IntFunc OBJ;
   std::cout << OBJ.Integrate([](double x) {
           return 1.0/std::sqrt(x*(1.0-x));   //ラムダ式の途中で改行とか，好きにしてね
       }, 0.0, 1.0) << std::endl;             //C++は, { と　} しか，見てないんで〜
   return 0;
}