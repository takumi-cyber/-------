#pragma once
#include <iostream>
#include <filesystem>
#include <cmath>
#include <vector>
#include <fstream>
#include <Eigen/Dense>

class LinearAdvection {
public:
    double c;
    double xmax;
    double tmax;
    int N; // Number of spatial points
    double dx; // Spatial step size
    double dt; // Time step size
    int M; // Number of time steps, calculated from tmax and dt
    Eigen::MatrixXd u; // Solution matrix, u[i][j] represents the value at time step i and spatial point j
    LinearAdvection(double c, double xmax, double tmax, int N, int M)
                  : c(c), xmax(xmax), tmax(tmax), N(N), M(M){
        dx = xmax / N;
        dt = tmax / M; // Courant condition
        u.resize(M + 1, N + 1);
        u.setZero();
    }
    void Solve(){
        for(int i=0; i<=N; i++){
            u(0,i) = 0;// Initial condition at t = 0
        }
        for(int j=0; j<=M; j++){
            u(j,0) = 1; // Boundary condition at x = 0
        }
        double k= c * dt / dx; // Courant number
        if(k>1){
            std::cout << "Warning: Courant number k = " << k << " >1.0 " << std::endl;
        }
        for(int j=0; j<=M-1; j++){
            for(int l=1; l<=N; l++){
                u(j+1,l) = (1-k)*u(j,l) + k*u(j,l-1);
            }
        }
    }
    void Save(std::filesystem::path filename) {
        if (!filename.parent_path().empty()) {
            std::filesystem::create_directories(filename.parent_path());
        }
        std::vector<double> j_list = {10.0, 20.0, 30.0, 40.0, 50.0};
        std::ofstream ofs(filename);
        for (int j : j_list) {
            ofs << std::endl << "# time= " << j << std::endl;
            for (int i = 0; i <= N; i++) {
                ofs << i * dx << " " << u(j, i) << std::endl;
            }
            ofs << std::endl; // Separate time steps
        }
        ofs.close();
    }
};