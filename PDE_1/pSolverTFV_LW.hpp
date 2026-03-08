#include "pSolverTFVBase.hpp"

class pSolverTFVLW:public pSolverTFVBase{
    public:
    pSolverTFVLW(const size_t &nx, const double &dt, const double &c,
        const std::function<double(double)> &U0):
        pSolverTFVBase(nx,dt,c,U0){}

    virtual void getGrad()override
    {
        for(int i=0; i<Data.size(); i++)
        Data[i][nt].dU=(Data[i+1][nt].U-Data[i][nt].U)/dx;
    }
};