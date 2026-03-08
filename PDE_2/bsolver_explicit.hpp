#include "bsolver.hpp"

class Bsolver_explicit : public Bsolver<double> {
    public:
    Bsolver_explicit(const int& nx, const double& dt, const std::function<double(double)>& U0)
    :Bsolver<double>(nx, dt, U0)
    {
        std::cout << "Starting Heat1DExplicit solver with nx=" << Data.size() << ", dt=" << dt << std::endl;
    }
    
    virtual void step() override{
        time = (++nt)*dt;
        for(int i=1;i<nx;i++) 
        {
            auto &P=Data[i];
            P[nt]=DN*(*P.pre)[nt-1]+(1-2*DN)*P[nt-1]+DN*(*P.next)[nt-1];
        }
            Data.front()[nt]=LBC_value;
            Data.back()[nt]=RBC_value;
    }
};