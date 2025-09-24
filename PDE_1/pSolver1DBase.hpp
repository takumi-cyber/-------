#pragma once
#include "pvector.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <filesystem>
#include <fstream>
template <typename T>

class pSolver1DBase{
    protected:
    double nx;
    double dt;
    double dx;
    double CFL;
    double time;
    int nt=0;
    double xmax=1.0;

    class Point{
        public:
        Point* pre = nullptr;
        Point* next = nullptr; 
        double X;
        pvector<T> u;
        Point():u(3){}
        auto &operator[](int n) { return u[n]; }
        const auto &operator[](int n) const { return u[n]; }
    };
    pvector<Point> Data;
    std::ofstream ofs;

    public:
    pSolver1DBase(const size_t &nx, const double &dt, 
        const std::function<double(double)> &U0=[](double x){return 0;}):dt(dt),nx(nx)
        {
        time = 0.0;
        dx=xmax/nx;
        CFL=dt/dx;
        Data.resize(nx);
        for(size_t i=0;auto &P:Data){
            P.pre=&Data[i-1];
            P.next=&Data[i+1];
            P[nt]=P[nt-1]=P[nt-2]=U0(P.X=i*dx);
            i++;
            }
        std::cout << "Booting pSolver1DBaase with nx=" << Data.size() << ", dt=" << dt << std::endl;
        }

        std::string what() const
        {
       return "pSolver1DBase: nx=" + std::to_string(Data.size()) + ", dt=" + std::to_string(dt) +
              ", CFL=" + std::to_string(CFL) + ", time=" + std::to_string(time);
        }

        bool Write(const std::string& filename)
        {
        if (ofs) ofs.close(); 
        std::filesystem::path datafile = getenv("HOME");
        datafile /= filename;
        std::filesystem::create_directories(datafile.parent_path());
        ofs.open(datafile, std::ios::app);
        if (!ofs) throw std::ios_base::failure("Failed to open file: " + datafile.string());
        return Write();       
        }
        
        bool Write(){
        try
        {
           ofs << std::endl << "# time= " << time << std::endl;
           for (auto &v : Data) ofs << v.X << " " << v[nt].U << std::endl;
        }
        catch (const std::exception &e)
        {
           std::cerr << "Error writing to file: " << e.what() << std::endl;
           return false;
        }
        return true;
        }

        virtual void Step()=0;
        virtual void Initialize(void *parm=nullptr){};
};