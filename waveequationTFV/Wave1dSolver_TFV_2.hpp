//波動方程式を積分して考える
#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include <cmath>
#include "pvector.hpp"

class Wave1dSolver{
    public:
    double X; //セル中心
    int nx;
    double xmax=1.0;
    int nt=0;
    double dt;
    double dx;
    double c;
    double CFL;
    double w; //x=0における∂u/∂x
    double w_sum; //wの時間方向の総和は保存
    int j;
    double k;
    double time;
    pvector<double> U_old,U_new; //セル平均
    std::function<double(double)> U1_func;//初期速度
    std::vector<double> P_old,P_new; //int_0~x u dx
    pvector<double> Q_old,Q_new; //int_0~t c*u dt
    std::vector<double> S_old, S_new; //P+Q
    std::vector<double> T_old, T_new; //P-Q
    std::vector<double> dS;//Sを補間するときの傾き
    std::vector<double> dT;//Tを補間するときの傾き

    std::ofstream ofs;
    double MinMod(const double a,const double b)//minmod関数、補間で使う
    {
            if (a*b<0) return 0.;
            if (std::abs(a)<std::abs(b)) return a;
            return b;
    }

    Wave1dSolver(const int &nx, const double &dt, const double &c,
        const std::function<double(double)> &U0=[](double x){return 0;},
        const std::function<double(double)> &U1=[](double x){return 0;}):dt(dt),nx(nx),c(c),U1_func(U1)
    {
        time = 0.0;
        dx=xmax/nx;
        CFL=c*dt/dx;
        j = std::floor(CFL);
        k = CFL -j;
        U_old.resize(nx);
        U_new.resize(nx);
        P_old.resize(nx+2*(j+2),0.0);
        P_new.resize(nx+2*(j+2),0.0);
        Q_old.resize(nx);
        Q_new.resize(nx);
        S_old.resize(nx+j+3,0.0);//始まりはx=-dxから終わりはx=xmax+(j+1)*dxまで
        S_new.resize(nx+j+3,0.0);
        T_old.resize(nx+j+3,0.0);//始まりはx=-(j+1)*dxから終わりはx=xmax+dxまで
        T_new.resize(nx+j+3,0.0);
        dS.resize(nx+j+3,0.0);
        dT.resize(nx+j+3,0.0);
        //U0の周期化
        auto U0_periodic = [&](double x)->double
        { 
            while (x < 0.0) x += xmax; 
            while (x >= xmax) x -= xmax; 
            return U0(x); 
        };

        //初期値設定
        for(int i=0;i<nx;i++)
        {
            X=(i+0.5)*dx;
            U_old[i]=U0_periodic(X);
            U_new[i]=0.0;
        }
        w=(U0_periodic(0.5*dx) - U0_periodic(-0.5*dx))/dx;
        w_sum=0.0;

        for(int i=0;i<nx+j+2;i++)
        {
            if(i==0)
            P_old[i+j+2]=0.5*dx*U_old[i];
            else
            P_old[i+j+2]=P_old[i+j+1]+dx*U_old[i];
        }
        for(int i=j+1;i>=0;i--)
        {
            if(i==j+1)
            P_old[i]=-0.5*dx*U_old[i];
            else
            P_old[i]=P_old[i+1]-dx*U_old[i];
        }

        for(int i=0;i<nx;i++)
        {
            Q_old[i]=0.0;
            Q_new[i]=0.0;
        }

        for(int i=0;i<nx+j+3;i++)
        {
            S_old[i]=P_old[i+j+1]+Q_old[i-1];
            T_old[i]=P_old[i]-Q_old[i-j-2];
        }
    }

    //時間発展に必要で初期速度に関わるものを準備
    //台形則
    double trapezoidal(double a, double b, int N, const std::function<double(double)>& func) {
        double h = (b - a) / N;
        double sum = 0.5 * (func(a) + func(b));
        for (int i = 1; i < N; ++i) {
            double x = a + i * h;
            sum += func(x);
        }
        return sum * h;
    }
    //二重積分
    double double_integral(double x1, double x2, double c, int Nt, int Nx) { //x1はx_i-1/2,x2はx_i+1/2
    double h_t = dt / Nt;
    double total = 0.0;

    for (int j = 0; j <= Nt; ++j) {
        double t = j * h_t;
        double a = x1 + c * t;
        double b = x2 + c * t;

        // 内側積分 (x方向)
        double inner = trapezoidal(a, b, Nx, U1_func);

        // 外側の重み
        double weight = (dt - t);

        // 台形則の端点補正
        double coeff = (j == 0 || j == Nt) ? 0.5 : 1.0;
        total += coeff * weight * inner;
    }

    return total * h_t;
    }
    //int_(x_i-1/2)~(x_i+1/2) U1_func * (x_i+1/2-ξ)dξを計算するための関数
    double integral_U1_shifted(double x1, double x2, int N) {
    double h = (x2 - x1) / N;
    double sum = 0.0;
    for (int i = 0; i <= N; ++i) {
        double y = x1 + i * h;
        double coeff = (i == 0 || i == N) ? 0.5 : 1.0; // 台形則の端点補正
        sum += coeff * U1_func(x2 - y);
    }
    return sum * h;
    }

    void solve()
    {
        //Sの時間発展 
        for(int i=0;i<nx+j+3;i++)
        {
            if(i == 0) {
            dS[i] = (S_old[i+1] - S_old[i]) / dx; // 左端：前方差分
            }
            else if(i == nx+j+2) {
            dS[i] = (S_old[i] - S_old[i-1]) / dx; // 右端：後方差分
            }
            else {
            dS[i] = MinMod(S_old[i+1]-S_old[i], S_old[i]-S_old[i-1]) / dx;
            }
        }
        for(int i=0;i<nx+2;i++)//後でUを復元するために左右１セルづつ多めに計算
        {
            double x1=(i-1)*dx; //x_i-1/2
            double x2=i*dx; //x_i+1/2
            double integral_1=double_integral(x1,x2,c,10,10);
            double integral_2=trapezoidal(0,x1,1000,U1_func);
            double integral_3=integral_U1_shifted(x1,x2,10);
            S_new[i]=(1-k)*S_old[i+j]+0.5*(1-k)*k*dx*dS[i+j]+k*S_old[i+j+1]-0.5*(1-k)*k*dx*dS[i+j+1] 
                        + integral_1 + dt*integral_2 + integral_3*(dt/dx)-c*c*dt*(w_sum +0.5*dt*w);
        }
        //Tの時間発展
        for(int i=0;i<nx+j+3;i++)
        {
            if(i == 0) {
            dT[i] = (T_old[i+1] - T_old[i]) / dx; // 左端：前方差分
            }
            else if(i == nx+j+2) {
            dT[i] = (T_old[i] - T_old[i-1]) / dx; // 右端：後方差分
            }
            else {
            dT[i] = MinMod(T_old[i+1]-T_old[i], T_old[i]-T_old[i-1]) / dx;
            }
        }

        for(int i=j+1;i<nx+j+3;i++)
        {
            double x1=(i-j-2)*dx; 
            double x2=(i-j-1)*dx; 
            double integral_1=double_integral(x1,x2,-c,10,10);
            double integral_2=trapezoidal(0,x1,1000,U1_func);
            double integral_3=integral_U1_shifted(x1,x2,10);
            T_new[i]=(1-k)*T_old[i-j]-0.5*(1-k)*k*dx*dT[i-j]+k*T_old[i-j-1]+0.5*(1-k)*k*dx*dT[i-j-1]
                    + integral_1 + dt*integral_2 + integral_3*(dt/dx)-c*c*dt*(w_sum +0.5*dt*w);
        }

        //P,QをS,Tから復元
        for(int i=0;i<nx+2;i++)
        {
            P_new[i+j+1]=0.5*(S_new[i]+T_new[i+j+1]);
        }
        for(int i=0;i<nx;i++)
        {
            Q_new[i]=0.5*(S_new[i+1]-T_new[i+j+2]);
            U_new[i]=(P_new[i+j+3]-P_new[i+j+1])/(2*dx);
        }

        //求まっていないPをU_newを使って求める
        for(int i=nx+j+3;i<nx+2*(j+2);i++)
        P_new[i]=P_new[i-1]+dx*U_new[i-j-2];
        for(int i=j;i>=0;i--)
        P_new[i]=P_new[i+1]-dx*U_new[i-j-2];

        //求まっていないS,Tを求める
        for(int i=nx+2;i<nx+j+3;i++)
        S_new[i]=P_new[i+j+1]+Q_new[i-1];
        for(int i=0;i<j+1;i++)
        T_new[i]=P_new[i]-Q_new[i-j-2];

        //wの更新
        w=(U_new[0]-U_new[nx-1])/dx;
        w_sum += w*dt;

        U_old = U_new;
        P_old = P_new;
        Q_old = Q_new;
        S_old = S_new;
        T_old = T_new;

        time = (++nt) * dt;
    }

    void write(std::filesystem::path filename)
    {
        std::filesystem::path myhome=getenv("HOME");
        myhome/=filename;
        std::filesystem::create_directories(myhome.parent_path());
        ofs.open(myhome, std::ios::app);
        ofs<<std::endl<<"# time = "<<time<<std::endl;
        for(int i=0;i<nx;i++)
        ofs<< (i+0.5)*dx << " " << U_old[i] <<std::endl;
        ofs.close();   
        std::cout << "time=" << nt*dt << " w_sum=" << w_sum << " w=" << w<< std::endl;
    }
};
