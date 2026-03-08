#include "VTK_practice.hpp"

int main(){
    VTK_Practice Writer;
    Writer.filename="プログラム演習/test1/test.vtk";
    Writer.ox<< -3.0,-3.0,0.0;
    Writer.dx<< 0.1,0.1,1.0;
    Writer.nx<< 61,61,1;
    Writer.write_Legacy_VTK2d([&Writer](int i,int j)
    {
        double x=Writer.ox[0]+i*Writer.dx[0];
        double y=Writer.ox[1]+j*Writer.dx[1];
        return exp(-(x*x+y*y));
    });
    return 0;
}