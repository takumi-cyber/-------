#pragma once
#include "pSolver1DBase.hpp"

class FVKit{
    public:
    double U;
    double dU;
    double Rflux;
    FVKit& operator=(double const x){
        U=x;
        dU=Rflux=0.0;
        return *this;
    }
    operator double() const { return U; }
};
    class pSolverFVBase:public pSolver1DBase<FVKit>{
        protected:
        void Grad2Flux(){
            for(auto &P:Data)
                P[nt].Rflux=P[nt].U+0.5*(1-CFL)*P[nt].dU*dx;
        }
        public:
        pSolverFVBase(const size_t &nx, const double &dt, 
            const std::function<double(double)> &U0):
            pSolver1DBase(nx,dt,U0){
                Data.resize(Data.size()-1);
                Data.front().pre=&Data.back();
                Data.back().next=&Data.front();
                for(auto &P:Data)P.X+=0.5*dx;
            }
        void Initialize(void *parm=nullptr) override{    
                std::cout << "pSolverFVBase::Initialize()" << std::endl;}

        virtual void getFlux()=0;
        
        void Step()override{
             getFlux();
             for(auto &P:Data)
                P[nt+1].U=P[nt].U-CFL*(P[nt].Rflux-(*P.pre)[nt].Rflux);
                time=(++nt)*dt;
        }
                        
};