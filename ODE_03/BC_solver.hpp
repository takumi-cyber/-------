#pragma once   
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>

class BC_Solver {
public:
    double C=200.0;
    int N = 100;
    double Xmin = 0.0;
    double Xmax = 1.0;
    double Y0 = 0.0;      
    double Y1 = 1.0;      
    std::vector<double> y;
    double dx;
    void Solve(){
        dx=(Xmax-Xmin)/N;
        std::vector<double> b;
        b.resize(N-1);              
        y.resize(N+1);              
        class ROW {
        public:
            double L,C,R;           
        };
        std::vector<ROW> A(N-1);    
        for(auto& row:A) {  row.L=-1.;   row.C=2.-C*dx*dx;  row.R=-1.;}
        for(auto& v:b) v=0;
        b[0] = Y0;
        b[N-2] = Y1;
        for(int i=1; i<N-1; i++){
            auto m=A[i].L/A[i-1].C;
            A[i].C-= m*A[i-1].R;
            b[i]-=m*b[i-1];
        }
        for(int i=N-1; i>1; i--){
            y[i]= (b[i-1]- ((i<N-1)?(A[i-1].R*y[i+1]):(0.0))/A[i-1].C);
        }
    }
    void Save(std::filesystem::path filename){
            if(!filename.parent_path().empty()){
                std::filesystem::create_directories(filename.parent_path());
            }
            std::ofstream ofs(filename);
            for(int i=0; i<=N; i++)
            ofs << i*dx << " " << y[i] <<std::endl;
            ofs.close();
    }
};
