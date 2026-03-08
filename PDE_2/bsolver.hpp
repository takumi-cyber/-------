#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <filesystem>
#include <fstream>
#include "pvector.hpp"
template <typename T>

class Bsolver{
    protected:
        double xmax;
        int nx;
        double dx;
        double dt;
        double time;
        int nt=0;
        double CFL;
        double DN;
        double LBC_value=0.0;
        double RBC_value=0.0;
        class Point{
            public:
                Point *pre=nullptr;
                Point *next=nullptr;
                double x;
                pvector<T> u;
                Point():u(3){}
                auto &operator[](int index){return u[index];}
                const auto &operator[](int index) const {return u[index];}
        };
        std::vector<Point> Data;
        std::ofstream ofs;

    public:
    Bsolver(const int& nx, const double& dt, const std::function<double(double)>& U0):nx(nx),dt(dt){
        xmax=1.0;
        dx=xmax/nx;
        time=0.0;
        CFL=dt/dx;
        DN=dt/(dx*dx);
        Data.resize(nx+1);
        for(int i=0;i<Data.size();i++)
        {
            auto &P=Data[i];
            if(i>0)P.pre = &Data[i-1];
            else P.pre = nullptr;
            if (i < Data.size()-1)P.next = &Data[i+1];
            else P.next = nullptr;
            P.x = i*dx;
            P[nt]=P[nt-1]=P[nt-2]=U0(P.x);
        }
    }
    double& LBC(){return LBC_value;}
    double& RBC(){return RBC_value;}

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
               << "# nt= " << nt << " time= " << time << " CFL= " << CFL << " NX= " << Data.size() <<" dx= "<<dx<< std::endl;
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
    virtual void step()=0;
    virtual void Initialize(void* parm=nullptr){}
};