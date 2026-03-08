//陽的差分法で波動方程式を解く、初期速度０、周期境界条件
#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include "pvector.hpp"

class Wave1dSolver{
    public:
    int nx;
    double xmax=1.0;
    double dx;
    double dt;
    double c;
    double CFL;
    double nu;//ν
    double mu;//μ
    double time;
    pvector<double> u_new;//u_n+1
    pvector<double> u;//u_n
    pvector<double> u_old;//u_n-1
    Wave1dSolver(const int &nx, const double &dt, const double &c, const double &nu, const double &mu,
                const std::function<double(double)> &U0=[](double x){return 0;}):nx(nx),dt(dt),c(c),nu(nu),mu(mu)
    {
        time=0.0;
        dx=xmax/nx;
        CFL=c*dt/dx;
        u_new.resize(nx);
        u.resize(nx);
        u_old.resize(nx);
        //初期値設定
        for(int i=0;i<nx;i++)
        {
            double x=(i+0.5)*dx;
            u[i]=U0(x);
        }
        for(int i=0;i<nx;i++)
        {
            u_old[i]=u[i]+0.5*dt*dt*(c*c*(u[i+1]-2*u[i]+u[i-1])/(dx*dx)-c*mu*(u[i+1]-u[i-1])/(2*dx));
        }
    }
    void solve()
    {
        double alpha = 0.5 * nu * dt;
        for(int i=0;i<nx;i++)
        {
        u_new[i]=(2.0*u[i]-(1-alpha)*u_old[i]+c*c*dt*dt*(u[i+1]-2.0*u[i]+u[i-1])/(dx*dx)
                    -c*mu*dt*dt*(u[i+1]-u[i-1])/(2*dx))/(1.0+alpha);
        }
        u_old=u;
        u=u_new;
        time +=dt;
    }
        void write(std::filesystem::path filename)
    {
        std::ofstream ofs;
        std::filesystem::path myhome=getenv("HOME");
        myhome/=filename;
        std::filesystem::create_directories(myhome.parent_path());
        ofs.open(myhome, std::ios::app);
        ofs << std::endl <<"# time = " << time << std::endl;
        for(int i=0;i<nx;i++)
        ofs << (i+0.5)*dx << " " << u[i] << std::endl;
        ofs.close();
    }
};
