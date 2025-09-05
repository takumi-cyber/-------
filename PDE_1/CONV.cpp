#include <iostream>
#include "CONV.hpp"
int main()
{
    size_t nx=100; // 空間分割数
    double dt=0.01; // 時間刻み幅
    size_t ntmax=1000; // 最大時間ステップ数
    size_t ntsave=10; // データ保存間隔
    CONV mySolver(nx,dt,"Data/pde100.data");
    mySolver.Write();                  //初期値も保存しよう
    for(size_t nt=0;nt<ntmax;nt++)
    {
        mySolver.Step();
        if (nt % ntsave == 0) mySolver.Write();
    }
    return 0;
}