#include "bsolver.hpp"
#include <Eigen/Sparse>

class Bsolver_implicit: public Bsolver<double>{
    public:
    Eigen::SparseLU<Eigen::SparseMatrix<double>> solver;
    Bsolver_implicit(const int& nx, const double& dt, const std::function<double(double)>& U0):
    Bsolver<double>(nx,dt,U0)
    {
        std::cout << "Starting Heat1DImplicit solver with nx=" << Data.size() << ", dt=" << dt << std::endl;
    }

    virtual void Initialize(void* parm=nullptr) override
    {
        Eigen::SparseMatrix<double> coef;
        coef.resize(Data.size(),Data.size());
        std::vector<Eigen::Triplet<double>> triplets;
        triplets.emplace_back(0,0,1.0);
        triplets.emplace_back(Data.size()-1,Data.size()-1,1.0);
        for(int i=1;i<Data.size()-1;i++){
        triplets.emplace_back(i,i-1,-DN);
        triplets.emplace_back(i,i,1+2*DN);
        triplets.emplace_back(i,i+1,-DN);
        }
        coef.setFromTriplets(triplets.begin(),triplets.end());
        solver.compute(coef);
        if(solver.info()!=Eigen::Success)
        {
            throw std::runtime_error("LU factorization failed!");
        }
    }

    virtual void step() override
    {
        Eigen::VectorXd b(Data.size());
        b[0]=LBC_value;
        for(int i=1;i<Data.size()-1;i++) b[i]=Data[i][nt];
        b[Data.size()-1]=RBC_value;
        auto result=solver.solve(b);
        time = (++nt)*dt;
        for(int i=0;i<Data.size();i++) Data[i][nt]=result[i];
    }
};
