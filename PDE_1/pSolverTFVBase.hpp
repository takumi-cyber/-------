#pragma once
#include "pvector.hpp"
#include <iostream>
#include <filesystem>
#include <fstream>

class TFVKit{
    public:
    double U; //セル平均
    double dU; //勾配
    TFVKit():U(0.0),dU(0.0){}
};

class pSolverTFVBase{
    protected:
    int nx;
    double xmax=1.0;
    double dt;
    double dx;
    double c;
    double CFL;
    double time;
    int nt=0;
    class Cell
    {
        public:
        double X; //セル中心
        pvector<TFVKit> Kit; //uのセル平均と勾配
        Cell():Kit(3){} 
        auto &operator[](int n) { return Kit[n]; }
        const auto &operator[](int n) const { return Kit[n]; }
    };
    pvector<Cell> Data;
    std::ofstream ofs;
    public:
    pSolverTFVBase(const size_t &nx, const double &dt, const double &c,
        const std::function<double(double)> &U0=[](double x){return 0;}):dt(dt),nx(nx),c(c)
        {
            time = 0.0;
            dx=xmax/nx;
            CFL=c*dt/dx;
            Data.resize(nx);
            for(size_t i=0;auto &P:Data)
            {
                P.X=i*dx+0.5*dx;
                P[nt].U=P[nt-1].U=P[nt-2].U=U0(P.X);
                i++;
            }
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

    virtual void getGrad()=0;

    void step()
    {
        getGrad();
        int j =std::floor(CFL);
        double k =CFL-j;
        for(int i=0; i<Data.size(); i++)
        Data[i][nt+1].U=(1-k)*Data[i-j][nt].U-0.5*(1-k)*k*dx*Data[i-j][nt].dU+k*Data[i-j-1][nt].U+0.5*k*(1-k)*dx*Data[i-j-1][nt].dU;
        time=(++nt)*dt;
    }
};