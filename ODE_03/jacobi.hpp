//ヤコビ法失敗
//ブランチ変更したよ
//数値積分で収束の遅さを表現
#pragma once
#include <iostream>
#include <fstream>
#include <Eigen/Dense>
#include <unordered_set>
#include <iomanip>

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
    std::filesystem::path logname;
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

    bool Solve(std::filesystem::path logname=""){
        size_t max_iter = 0;
        for(auto &n:Savetimes) max_iter=std::max(max_iter, n);//保存指定回数の最大値が，最大繰り返し数で，ええやん
        std::ofstream lout;     // 記録を取るファイル．下で見えるように，ここで定義
        if (!logname.empty())   // logname が指定されていれば記録を取る
        {
           std::filesystem::create_directories(logname.parent_path());
           lout.open(logname);
        }
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

            if (lout)   // 記録を取る場合
           {
               double sum=0.0,x_pre=Y0;
               for(auto& v:x_new) sum+=(x_pre*x_pre+v*v)*dx*0.5, x_pre=v;
               sum+=(x_pre+Y1*Y1)*dx*0.5;
               lout << iter << " " << std::setprecision(15) << std::sqrt(sum) << std::endl;
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
