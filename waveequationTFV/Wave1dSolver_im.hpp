#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <functional>
#include <Eigen/Sparse>
#include <Eigen/SparseLU>
#include "pvector.hpp"

class Wave1dImplicit {
public:

    int nx;
    double xmax = 1.0;
    double dx;
    double dt;
    double c;
    double nu;
    double mu;
    double time;

    pvector<double> u_new;
    pvector<double> u;
    pvector<double> u_old;

    Eigen::SparseMatrix<double> A;
    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;

    Wave1dImplicit(
        const int &nx,
        const double &dt,
        const double &c,
        const double &nu,
        const double &mu,
        const std::function<double(double)> &U0 = [](double x){return 0.0;}
    )
    : nx(nx), dt(dt), c(c), nu(nu), mu(mu)
    {
        time = 0.0;
        dx = xmax / nx;

        u.resize(nx);
        u_old.resize(nx);
        u_new.resize(nx);

        // 初期条件
        for(int i=0;i<nx;i++){
            double x = (i+0.5)*dx;
            u[i] = U0(x);
        }

        // 初期速度ゼロ
        // 初期速度ゼロ（テイラー展開から u^{-1} を構築）
        for(int i=0;i<nx;i++){
            u_old[i] = u[i] + 0.5*dt*dt*(c*c*(u[i+1]-2*u[i]+u[i-1])/(dx*dx)-c*mu*(u[i+1]-u[i-1])/(2*dx));
        }       
        build_matrix();
    }

    void build_matrix()
    {
        double r = c*c*dt*dt/(dx*dx);
        double tcoef = c*mu*dt*dt/(2.0*dx);
        double s = 0.5 * nu * dt;   // ★ 中心差分用に変更

        std::vector<Eigen::Triplet<double>> triplets;

        for(int i=0;i<nx;i++){

            triplets.emplace_back(i,i,1.0 + s + 2.0*r);
            triplets.emplace_back(i,(i+1)%nx, -(r - tcoef));
            triplets.emplace_back(i,(i-1+nx)%nx, -(r + tcoef));

        }

        A.resize(nx,nx);
        A.setFromTriplets(triplets.begin(), triplets.end());

        solver.analyzePattern(A);
        solver.factorize(A);

        if(solver.info() != Eigen::Success){
            std::cerr << "Matrix factorization failed\n";
            exit(1);
        }
    }

    void solve()
    {
        double s = 0.5 * nu * dt;   // ★ 中心差分用に変更

        Eigen::VectorXd rhs(nx);

        for(int i=0;i<nx;i++)
            rhs(i) = 2.0*u[i] - (1.0 - s)*u_old[i];  // ★ ここが変更点

        Eigen::VectorXd sol = solver.solve(rhs);

        if(solver.info() != Eigen::Success){
            std::cerr << "Solve failed\n";
            exit(1);
        }

        for(int i=0;i<nx;i++)
            u_new[i] = sol(i);

        u_old = u;
        u = u_new;
        time += dt;
    }

    void write(std::filesystem::path filename)
    {
        std::ofstream ofs;
        std::filesystem::path myhome = getenv("HOME");
        myhome /= filename;
        std::filesystem::create_directories(myhome.parent_path());

        ofs.open(myhome, std::ios::app);
        ofs << "\n# time = " << time << "\n";

        for(int i=0;i<nx;i++)
            ofs << (i+0.5)*dx << " " << u[i] << "\n";

        ofs.close();
    }
};
