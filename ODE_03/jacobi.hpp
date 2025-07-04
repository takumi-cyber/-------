//ヤコビ法失敗
#pragma once
#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <unordered_set>

class Jacobi{
 private:
    double C = 0.2; 
    double Xmin = 0.0;
    double Xmax = 1.0;
    double Y0 = 0.0;      
    double Y1 = 1.0; 
    double dx;
    Eigen::MatrixXd myMat;
    Eigen::VectorXd myVec;
    Eigen::VectorXd mySol; 

public:
    std::unordered_set<size_t> Savetimes;
    std::filesystem::path filename;
    Jacobi(){
        std::cout << "Jacobi solver initialized." << std::endl;
        Savetimes.clear();
    };
    void resize(size_t N){
        myMat.resize(N-1, N-1);
        myVec.resize(N-1);
        mySol.resize(N+1);
        myMat.setZero();
        myVec.setZero();
        mySol.setZero();
    }

    void Init(){
    dx = (Xmax - Xmin) /  (myVec.size() + 1);
    for (int i = 0; i < myMat.rows(); ++i)
    {
        myMat(i, i) = 2.0 - C * dx * dx; // 対角成分
        if (i > 0)
            myMat(i, i - 1) = -1.0; // 左隣
        if (i < myMat.rows() - 1)
            myMat(i, i + 1) = -1.0; // 右隣
    }
    myVec[0] = Y0; // 左端の境界条件
    myVec[myVec.size() - 1] = Y1; // 右端の境界条件
    mySol[0] = Y0;
    mySol[mySol.size() - 1] = Y1; // 右端の境界条件
    }   

    bool Solve(size_t max_iter = 20000){
        std::filesystem::create_directories(filename.parent_path());
        std::ofstream ofs(filename);
        size_t message_iter = 1000; // 途中で「XX回やったよ〜」とメッセージを出したい
        double tol = 1e-04;         // 途中でもズレがこの値になったら中断して良いことにする（収束限界）
        Eigen::VectorXd x_old = myVec;
        Eigen::VectorXd x_new = myVec;
        x_old.setZero();                  // Eigenベクトルを0ベクトルにする
        x_new.setZero();
        // 初期化
        size_t iter = 0;
        double err = 1.0;
        while (iter < max_iter)
        {
            iter++;
            for (int i = 0; i < myMat.rows(); ++i)    //  行列 A と ベクトル　x_old の積を計算する
            {
                x_new[i] = 0.0; // 初期化
                for (int j = 0; j < myMat.rows(); ++j)
                    if (j != i) // 対角要素を除く行列積
                        x_new[i] += myMat(i, j) * x_old[j];
                // D * x_new = b - (A-D)*x_old   対角行列だから割れば良い
                x_new[i] = (myVec[i] - x_new[i]) / myMat(i, i);
            }

            if ((err = (x_new - x_old).norm()) < tol)
                break;

            x_old = x_new;

            if (Savetimes.contains(iter)){
                std::cout << "Iteration=" << iter <<": "<< "ERROR=" << err << " "<<"Saving to [" << filename.string() << std::endl;
                ofs << std::endl << "# iter = " << iter << std::endl;
                ofs << 0 * dx << " " << Y0 << std::endl;
                for(size_t i=0;i<x_new.size();++i) ofs << (i+1)*dx << " " << x_new[i] << std::endl; 
                ofs << x_new.size() * dx << " " << Y1 << std::endl;
            }
            
        }
        if (iter == max_iter)
            std::cout << "Timeout. Error= " << err << std::endl;
        else
            std::cout << "Converged in " << iter << " iterations." << std::endl;

        for (int i = 0; i < myMat.rows(); ++i)
            mySol[i + 1] = x_new[i];
        return iter != max_iter; // 収束したかどうかを返す
    }

    void Save(std::filesystem::path filename){
        if(!filename.parent_path().empty()){
            std::filesystem::create_directories(filename.parent_path());
        }
        std::ofstream ofs(filename);
        for(int i = 0; i < mySol.size(); ++i)
            ofs << i * dx << " " << mySol[i] << std::endl;
        ofs.close();
    }

    void Inv(){
        auto Sol = myMat.inverse() * myVec; // 逆行列を計算してベクトルと掛ける
        mySol[0] = Y0; // 左端の境界条件
        for(int i = 0; i < Sol.size(); ++i)
            mySol[i + 1] = Sol[i]; // 右端の境界条件
        mySol[mySol.size() - 1] = Y1; // 右端の境界条件
    }
};
