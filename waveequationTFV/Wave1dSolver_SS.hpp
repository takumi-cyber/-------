// utt-c^2uxx=-νut-cμuxを解く　strang-splitting
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
    int j1; //cΔt/Δxの整数部分
    int j2; //cΔt/2Δxの整数部分
    double k1; //cΔt/Δxの小数部分
    double k2; //cΔt/2Δxの小数部分
    double nu;//ν
    double mu;//μ
    pvector<double> n;
    pvector<double> n_old;
    pvector<double> v;
    pvector<double> v_old;
    pvector<double> F; //n+v
    pvector<double> F_1;
    pvector<double> F_2;
    pvector<double> F_old;
    pvector<double> G; //n-v
    pvector<double> G_1;
    pvector<double> G_2;
    pvector<double> G_old;
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
        j1=std::floor(CFL);
        k1=CFL-j1;
        j2=std::floor(CFL/2.0);
        k2=CFL/2.0-j2;
        n.resize(nx);
        v.resize(nx);
        F.resize(nx);
        G.resize(nx);
        n_old.resize(nx);
        v_old.resize(nx);
        F_old.resize(nx);
        G_old.resize(nx);
        F_1.resize(nx);
        F_2.resize(nx);
        G_1.resize(nx);
        G_2.resize(nx);
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
        for(int i=0;i<nx;i++)//移流半ステップ
        {
            F_1[i]=(1-k2)*F_old[i-j2]-0.5*(1-k2)*k2*dx*dF[i-j2]+k2*F_old[i-j2-1]+0.5*k2*(1-k2)*dx*dF[i-j2-1];
            G_1[i]=(1-k2)*G_old[i+j2]+0.5*(1-k2)*k2*dx*dG[i+j2]+k2*G_old[i+j2+1]-0.5*k2*(1-k2)*dx*dG[i+j2+1];
        }
        double U1; //F_1+G_1
        double U2; //F_2+G_2
        double V1; //F_1-G_1
        double V2; //F_2-G_2
        for(int i=0;i<nx;i++)//反応項ステップ
        {
            U1=F_1[i]+G_1[i];
            V1=F_1[i]-G_1[i];
            U2=U1;
            V2=exp(-nu*dt)*V1+ (mu/nu)*(1-exp(-nu*dt))*U1;
            F_2[i]=0.5*(U2+V2);
            G_2[i]=0.5*(U2-V2);
        }
        for(int i=0;i<nx;i++)
        {
            dF[i]=MinMod(F_2[i+1]-F_2[i],F_2[i]-F_2[i-1])/dx;
            dG[i]=MinMod(G_2[i+1]-G_2[i],G_2[i]-G_2[i-1])/dx;
        }
        for(int i=0;i<nx;i++)
        {
            F[i]=(1-k2)*F_2[i-j2]-0.5*(1-k2)*k2*dx*dF[i-j2]+k2*F_2[i-j2-1]+0.5*k2*(1-k2)*dx*dF[i-j2-1];
            G[i]=(1-k2)*G_2[i+j2]+0.5*(1-k2)*k2*dx*dG[i+j2]+k2*G_2[i+j2+1]-0.5*k2*(1-k2)*dx*dG[i+j2+1];
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