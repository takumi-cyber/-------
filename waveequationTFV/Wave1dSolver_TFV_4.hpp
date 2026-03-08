// utt-c^2uxx=-νut-cμuxを解く　Heun法
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
    int j; //CFLの整数部分
    double k; //CFLの小数部分
    double nu;//ν
    double mu;//μ
    pvector<double> n;
    pvector<double> n_old;
    pvector<double> v;
    pvector<double> v_old;
    pvector<double> F; //n+v
    pvector<double> F_old;
    pvector<double> F_mid;//予測
    pvector<double> G; //n-v
    pvector<double> G_old;
    pvector<double> G_mid;//予測
    pvector<double> dF;
    pvector<double> dG;
    double time;
    double MinMod(const double a,const double b)//minmod関数、補間で使う
    {
            if (a*b<0) return 0.;
            if (std::abs(a)<std::abs(b)) return a;
            return b;
    }

    Wave1dSolver(const int &nx, const double &dt, const double &c, const double &nu, const double &mu,
                const std::function<double(double)> &U0=[](double x){return 0;},
                const std::function<double(double)> &U1=[](double x){return 0;}):nx(nx),dt(dt),c(c),nu(nu),mu(mu)
    {
        time=0.0;
        dx=xmax/nx;
        CFL=c*dt/dx;
        j=std::floor(CFL);
        k=CFL-j;
        n.resize(nx);
        v.resize(nx);
        F.resize(nx);
        G.resize(nx);
        n_old.resize(nx);
        v_old.resize(nx);
        F_old.resize(nx);
        G_old.resize(nx);
        F_mid.resize(nx);
        G_mid.resize(nx);
        dF.resize(nx);
        dG.resize(nx);

        //初期値設定
        for(int i=0;i<nx;i++)
        {
            double x=(i+0.5)*dx;
            n_old[i]=U0(x);
            v_old[i]=U1(x);
            F_old[i]=n_old[i]+v_old[i];
            G_old[i]=n_old[i]-v_old[i];
        }
    }
    
    void solve() //F,Gの時間ステップ
    {
        for(int i=0;i<nx;i++)
        {
            dF[i]=MinMod(F_old[i+1]-F_old[i],F_old[i]-F_old[i-1])/dx;
            dG[i]=MinMod(G_old[i+1]-G_old[i],G_old[i]-G_old[i-1])/dx;
        }
        for(int i=0;i<nx;i++)//予測
        {
            F_mid[i]=(1-k)*F_old[i-j]-0.5*(1-k)*k*dx*dF[i-j]+k*F_old[i-j-1]+0.5*k*(1-k)*dx*dF[i-j-1]
                        +dt*(0.5*(-nu+mu)*((1-k)*F_old[i-j]-0.5*(1-k)*k*dx*dF[i-j]+k*F_old[i-j-1]+0.5*k*(1-k)*dx*dF[i-j-1])
                        +0.5*(nu+mu)*((1-k)*G_old[i-j]-0.5*(1-k)*k*dx*dG[i-j]+k*G_old[i-j-1]+0.5*k*(1-k)*dx*dG[i-j-1]));
            G_mid[i]=(1-k)*G_old[i+j]+0.5*(1-k)*k*dx*dG[i+j]+k*G_old[i+j+1]-0.5*k*(1-k)*dx*dG[i+j+1]
                        +dt*(0.5*(nu-mu)*((1-k)*F_old[i+j]+0.5*(1-k)*k*dx*dF[i+j]+k*F_old[i+j+1]-0.5*k*(1-k)*dx*dF[i+j+1])
                        -0.5*(nu+mu)*((1-k)*G_old[i+j]+0.5*(1-k)*k*dx*dG[i+j]+k*G_old[i+j+1]-0.5*k*(1-k)*dx*dG[i+j+1]));
        }
        for(int i=0;i<nx;i++)//修正
        {
            F[i]=(1-k)*F_old[i-j]-0.5*(1-k)*k*dx*dF[i-j]+k*F_old[i-j-1]+0.5*k*(1-k)*dx*dF[i-j-1]
                        +0.5*dt*(0.5*(-nu+mu)*F_mid[i]+0.5*(nu+mu)*G_mid[i])
                        +0.5*dt*(0.5*(-nu+mu)*((1-k)*F_old[i-j]-0.5*(1-k)*k*dx*dF[i-j]+k*F_old[i-j-1]+0.5*k*(1-k)*dx*dF[i-j-1])
                        +0.5*(nu+mu)*((1-k)*G_old[i-j]-0.5*(1-k)*k*dx*dG[i-j]+k*G_old[i-j-1]+0.5*k*(1-k)*dx*dG[i-j-1]));

            G[i]=(1-k)*G_old[i+j]+0.5*(1-k)*k*dx*dG[i+j]+k*G_old[i+j+1]-0.5*k*(1-k)*dx*dG[i+j+1]
                        +0.5*dt*(0.5*(nu-mu)*F_mid[i]-0.5*(nu+mu)*G_mid[i])
                        +0.5*dt*(0.5*(nu-mu)*((1-k)*F_old[i+j]+0.5*(1-k)*k*dx*dF[i+j]+k*F_old[i+j+1]-0.5*k*(1-k)*dx*dF[i+j+1])
                        -0.5*(nu+mu)*((1-k)*G_old[i+j]+0.5*(1-k)*k*dx*dG[i+j]+k*G_old[i+j+1]-0.5*k*(1-k)*dx*dG[i+j+1]));
        }
        for(int i=0;i<nx;i++)
        {
            n[i]=0.5*(F[i]+G[i]);
            v[i]=0.5*(F[i]-G[i]);
        }

        time +=dt;

        n_old=n;
        v_old=v;
        F_old=F;
        G_old=G;
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
        ofs << (i+0.5)*dx << " " << n_old[i] << std::endl;
        ofs.close();
    }
};