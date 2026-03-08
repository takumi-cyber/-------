//初期速度０、周期境界条件でのみ完全に成り立つ
#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include "pvector.hpp"

class TFVKit{
    public:
    double U; //セル平均
    double p; //∂u/∂t
    double w; //c*∂u/∂x
    double f; //p+w
    double g; //-p+w
    double df;
    double dg;
    TFVKit():U(0.0),p(0.0),w(0.0),f(0.0),g(0.0),df(0.0),dg(0.0){}
};

class Wave1dSolver{
    public:
    int nx;
    double xmax=1.0;
    double dt;
    double dx;
    double c;
    double CFL;
    double time;
    int nt=0;
    double E0;//初期エネルギー
    double E;//エネルギー保存を確認
    double Ubar0;
    class Cell
    {
        public:
        double X; //セル中心
        pvector<TFVKit> Kit;
        Cell():Kit(3){}
        auto &operator[](int n) { return Kit[n]; }
        const auto &operator[](int n) const { return Kit[n]; }
    };
    pvector<Cell> Data;
    std::ofstream ofs;
    double MinMod(const double a,const double b)//minmod関数、補間で使う
    {
            if (a*b<0) return 0.;
            if (std::abs(a)<std::abs(b)) return a;
            return b;
    }
    
    Wave1dSolver(const size_t &nx, const double &dt, const double &c,
        const std::function<double(double)> &U0=[](double x){return 0;}):dt(dt),nx(nx),c(c)
        {
            time = 0.0;
            dx=xmax/nx;
            CFL=c*dt/dx;
            Data.resize(nx);
            E0=0.0;
            E=0.0;
            //U0の周期化
            auto U0_periodic = [&](double x)->double{ 
                while (x < 0.0) x += xmax; 
                while (x >= xmax) x -= xmax; 
                return U0(x); 
            };
            //初期値設定
            for(size_t i=0;auto &P:Data)
            {
                P.X=i*dx+0.5*dx;
                double up = U0_periodic(P.X + dx); 
                double um = U0_periodic(P.X - dx);
                P[nt].U=P[nt-1].U=P[nt-2].U=U0(P.X);
                P[nt].p=P[nt-1].p=P[nt-2].p=0.0;
                P[nt].w=P[nt-1].w=P[nt-2].w=((U0(P.X)+up)-(um+U0(P.X)))/(2*dx);
                P[nt].f=P[nt].p+P[nt].w;
                P[nt-1].f=P[nt-1].p+P[nt-1].w;
                P[nt-2].f=P[nt-2].p+P[nt-2].w;
                P[nt].g=-P[nt].p+P[nt].w;
                P[nt-1].g=-P[nt-1].p+P[nt-1].w;
                P[nt-2].g=-P[nt-2].p+P[nt-2].w;
                i++;
            }
            //初期エネルギー計算
            for(int i=0;i<nx;i++)
            {
                double pi =Data[i][nt].p;
                double wi =Data[i][nt].w;
                E0 += 0.5 * (pi*pi + wi*wi) * dx;
            }
            E=E0;

            Ubar0 = 0.0;
            for(int i=0;i<nx;i++) Ubar0 += Data[i][nt].U;
            Ubar0 /= nx;
        }
    void write(std::filesystem::path filename)
    {
        std::filesystem::path myhome=getenv("HOME");
        myhome/=filename;
        std::filesystem::create_directories(myhome.parent_path());
        ofs.open(myhome, std::ios::app);
        ofs << std::endl << "# time= " << time << std::endl;
        for (auto &v : Data) ofs << v.X << " " << v[nt].U << std::endl;
        ofs.close();
        std::cout << " E = "<< E << " dE= "<< E-E0 << std::endl;
    }

    void solve()
    { 
          //-p+wを解く
        for(int i=0;i<Data.size();i++)
        Data[i][nt].dg=MinMod(Data[i][nt].g-Data[i-1][nt].g,Data[i+1][nt].g-Data[i][nt].g)/dx;

        int j =std::floor(CFL);
        double k =CFL-j;
        for(int i=0; i<Data.size(); i++)
        Data[i][nt+1].g=(1-k)*Data[i-j][nt].g-0.5*(1-k)*k*dx*Data[i-j][nt].dg+k*Data[i-j-1][nt].g+0.5*k*(1-k)*dx*Data[i-j-1][nt].dg;

        //p+wを解く
        for(int i=0;i<Data.size();i++)
        Data[i][nt].df=MinMod(Data[i][nt].f-Data[i-1][nt].f,Data[i+1][nt].f-Data[i][nt].f)/dx;

        for(int i=0; i<Data.size(); i++)
        Data[i][nt+1].f=(1-k)*Data[i+j][nt].f+0.5*(1-k)*k*dx*Data[i+j][nt].df+k*Data[i+j+1][nt].f-0.5*k*(1-k)*dx*Data[i+j+1][nt].df;
        
        //pを求める
        for(int i=0; i<Data.size(); i++)
        Data[i][nt+1].p=0.5*(Data[i][nt+1].f-Data[i][nt+1].g);

        //wを求める
        for(int i=0; i<Data.size(); i++)
        Data[i][nt+1].w=0.5*(Data[i][nt+1].f+Data[i][nt+1].g);

        //Uを求める
        pvector<double> U_left(nx);

        // 左端から右へ積分
        U_left[0] = Data[0][nt].U; // 左端セルの値を基準
        for(int i=1;i<nx;i++)
        U_left[i] = U_left[i-1] + dx/c * Data[i-1][nt+1].w;

        for(int i=0;i<nx;i++)
        Data[i][nt+1].U = U_left[i];

        //平均補正を追加
        double mean = 0.0;
        for(int i=0;i<nx;i++)
        mean += Data[i][nt+1].U;
        mean /= nx;

        for(int i=0;i<nx;i++)
        Data[i][nt+1].U -= (mean-Ubar0);

        time = (++nt) * dt;

        //Eを求める
        E=0.0;
        for(int i=0;i<nx;i++)
        {
            double pi = Data[i][nt].p;
            double wi = Data[i][nt].w;
            E += 0.5 * (pi*pi + wi*wi) * dx;
        }
    }
};