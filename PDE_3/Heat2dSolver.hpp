#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include "MyArray.hpp"

class Heat2dSolver{
    private:
        size_t n;
        double xmax, ymax;
        double dx, dy;
        double dt;
        double k;
        double rx;
        double ry;
        double time;
        MyArray<double> u_old;
        MyArray<double> u_new;
        double u0,u1,u2,u3; //境界条件 u0:左, u1:右, u2:下, u3:上
        std::vector<double> b1,b2;
        std::ofstream ofs;
    public:
        Heat2dSolver(size_t n, double dt, double k, const std::function<double(double,double)> &U0):n(n),dt(dt),k(k),u_old(n,n),u_new(n,n)
        {
            time=0.0;
            xmax=ymax=1.0;
            dx=xmax/(n-1);
            dy=ymax/(n-1);
            rx=k*dt/(2*dx*dx);
            ry=k*dt/(2*dy*dy);
            b1.resize(n-2);
            b2.resize(n-2);
            //初期条件
            for(int i=0;i<n;i++){
                for(int j=0;j<n;j++){
                    double x=i*dx;
                    double y=j*dy;
                    u_old(i,j)=U0(x,y);
                }
            }
            //境界条件
            for(int j=0;j<n;j++){
                u_old(0,j)=u_new(0,j)=u0=0.0; //左
                u_old(n-1,j)=u_new(n-1,j)=u1=0.0; //右
            }
            for(int i=0;i<n;i++){
                u_old(i,0)=u_new(i,0)=u2=0.0; //下
                u_old(i,n-1)=u_new(i,n-1)=u3=0.0; //上
            }
        }

        void write(const std::filesystem::path &filename)
        {
            std::filesystem::path myhome=getenv("HOME");
            myhome/=filename;
            std::filesystem::create_directories(myhome.parent_path());
            ofs.open(myhome);
            //vtk
            ofs << "# vtk DataFile Version 3.0\n";
            ofs << "Heat2D data at time " << time << "\n";
            ofs << "ASCII\n";
            ofs << "DATASET STRUCTURED_POINTS\n";
            ofs << "DIMENSIONS " << n << " " << n << " 1\n";
            ofs << "ORIGIN 0 0 0\n";
            ofs << "SPACING " << dx << " " << dy << " 1\n";
            ofs << "POINT_DATA " << n*n << "\n";
            ofs << "SCALARS temperature double\n";
            ofs << "LOOKUP_TABLE default\n";
            for(int j=0;j<n;j++){
                for(int i=0;i<n;i++) ofs<<u_old(i,j)<<std::endl;
            }
            ofs.close();
        }

        void solve()
        {
            class ROW1{public:double L1,C1,R1;};
            class ROW2{public:double L2,C2,R2;};
            
            //一回目半ステップ
            for(int j=1;j<n-1;j++){
                std::vector<ROW1> A1(n-2);
                for(auto &a1:A1)
                {
                a1.L1=-rx;
                a1.C1=1+2*rx;
                a1.R1=-rx;
                }
                for(int i=1;i<n-1;i++)
                {
                if(i==1) b1[i-1]=ry*u_old(i,j-1)+(1-2*ry)*u_old(i,j)+ry*u_old(i,j+1)+rx*u0;
                else if (i==n-2) b1[i-1]=ry*u_old(i,j-1)+(1-2*ry)*u_old(i,j)+ry*u_old(i,j+1)+rx*u1;
                else b1[i-1]=ry*u_old(i,j-1)+(1-2*ry)*u_old(i,j)+ry*u_old(i,j+1);
                }

                for(int i=1;i<n-2;i++)
                {
                auto m1=A1[i].L1/A1[i-1].C1;
                A1[i].C1-=m1*A1[i-1].R1;
                A1[i].L1=0.0;
                b1[i]-=m1*b1[i-1];
                }

                for(int i=n-2;i>0;i--)
                {
                    u_new(i,j)=(b1[i-1]-((i<n-2)? A1[i-1].R1*u_new(i+1,j) : 0.0))/A1[i-1].C1;
                }
            }
            //二回目半ステップ
            for(int i=1;i<n-1;i++){
                std::vector<ROW2> A2(n-2);
                for(auto &a2:A2)
                {
                    a2.L2=-ry;
                    a2.C2=1+2*ry;
                    a2.R2=-ry;
                }
                for(int j=1;j<n-1;j++)
                {
                    if(j==1) b2[j-1]=rx*u_new(i-1,j)+(1-2*rx)*u_new(i,j)+rx*u_new(i+1,j)+ry*u2;
                    else if (j==n-2) b2[j-1]=rx*u_new(i-1,j)+(1-2*rx)*u_new(i,j)+rx*u_new(i+1,j)+ry*u3;
                    else b2[j-1]=rx*u_new(i-1,j)+(1-2*rx)*u_new(i,j)+rx*u_new(i+1,j);
                }
                for(int j=1;j<n-2;j++)
                {
                    auto m2=A2[j].L2/A2[j-1].C2;
                    A2[j].C2-=m2*A2[j-1].R2;
                    A2[j].L2=0.0;
                    b2[j]-=m2*b2[j-1];
                }
                for(int j=n-2;j>0;j--)
                {
                    u_old(i,j)=(b2[j-1]-((j<n-2)? A2[j-1].R2*u_old(i,j+1) : 0.0))/A2[j-1].C2;
                }
            }
            time+=dt;
        }
};