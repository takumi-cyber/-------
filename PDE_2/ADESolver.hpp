#pragma once
#include <iostream>
#include <filesystem>
#include <fstream>
#include "pvector.hpp"
#include <cmath>
#include <Eigen/Sparse>

class ADESolver{
    private:
        double xmax=1.0;
        double dx;
        double dt;
        double c=1.0;
        double k;
        double CFL;
        double DN;
        int nx;
        int nt=0;
        double time;
        class Point{
            public:
                double x;//position
                pvector<double> u;
                Point():u(3){}
                auto &operator[](int index) { return u[index]; }
                const auto &operator[](int index) const { return u[index]; }
        };
        pvector<Point> Data;
        std::ofstream ofs;
        Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;

    public:
    ADESolver(const int &nx, const double &dt, const double &k, const std::function<double(double)>& U0):nx(nx),dt(dt),k(k)
    {
        time=0.0;
        dx=xmax/nx;
        CFL=c*dt/dx;
        DN=k*dt/(dx*dx);
        Data.resize(nx+1);
        for(int i=0;i<=nx;i++)
        {
            auto &p=Data[i];
            p.x=i*dx;
            p[nt]=p[nt-1]=p[nt-2]=U0(p.x);
        }
    }

     bool Write(const std::string &filename)
   {
       if (ofs) ofs.close(); // 既存のファイルストリームを閉じる
       std::filesystem::path datafile = getenv("HOME");
       datafile /= filename;
       std::filesystem::create_directories(datafile.parent_path());
       ofs.open(datafile, std::ios::app);
       if (!ofs) throw std::ios_base::failure("Failed to open file: " + datafile.string());
       return Write();
   }
   bool Write()
   {
       try
       {
           ofs << std::endl
               << "# nt= " << nt << " time= " << time << " CFL= " << CFL << " DN= " << DN << " NX= " << Data.size() <<" dx= "<<dx<< std::endl;
           for (auto &v : Data)
               ofs << v.x << " " << v[nt] << std::endl;
       }
       catch (const std::exception &e)
       {
           std::cerr << "Error writing to file: " << e.what() << std::endl;
           return false;
       }
       std::cout << "time[" << time << "]written to file successfully." << std::endl;
       return true;
    }

    void make()
    {
        Eigen::SparseMatrix<double> coef;
        coef.resize(Data.size(),Data.size());
        coef.insert(0,0)=2*(1+DN);
        coef.insert(0,1)=-(DN-CFL*0.5);
        coef.insert(0,Data.size()-1)=-(DN+CFL*0.5);
        coef.insert(Data.size()-1,Data.size()-1)=2*(1+DN);
        coef.insert(Data.size()-1,Data.size()-2)=-(DN+CFL*0.5);
        coef.insert(Data.size()-1,0)=-(DN-CFL*0.5);
        for(int i=1;i<Data.size()-1;i++)
        {
            coef.insert(i,i-1)=-(DN+CFL*0.5);
            coef.insert(i,i)=2*(1+DN);
            coef.insert(i,i+1)=-(DN-CFL*0.5);
        }
        coef.makeCompressed();
        solver.compute(coef);
        if(solver.info()!=Eigen::Success)
        {
            throw std::runtime_error("LU factorization failed!");
        }


    }
    void step()
    {
        Eigen::VectorXd b(Data.size());
        for(int i=0;i<Data.size();i++)
        {
            b[i]=(DN+CFL*0.5)*Data[i-1][nt]+2*(1-DN)*Data[i][nt]+(DN-CFL*0.5)*Data[i+1][nt];
        }
        auto result=solver.solve(b);
        time = (++nt)*dt;
        for(int i=0;i<Data.size();i++) Data[i][nt]=result[i];
    }
};