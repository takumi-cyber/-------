#include <iostream>
#include <filesystem>
#include <cmath>
#include <fstream>

class IC_solver {
    double C=1;
    double Xmin=0.0;
    double Xmax=2*M_PI;
    double Ymin=1.0;
    double Zmin=0.0;
    std::filesystem::path DataFile;
    std::filesystem::path DataFolder;

    public:
    int N;
    IC_solver(std::filesystem::path path):DataFolder(path){
       if (!std::filesystem::exists(path)) std::filesystem::create_directories(path);
       DataFile=path;
       std::cout << "IC_Solver::Using DataFolder[" << path.string() << "]" << std::endl;
    }
    IC_solver(){
        throw std::runtime_error("IC_Solver::IC_Solver()::なにやっとんじゃぼけ〜");
    }
    void Solve(std::string onamae){
       DataFile=DataFolder/onamae;           
       std::cout << "IC_Solver::Using DataFile[" << DataFile.string() << "]" << std::endl;
       std::ofstream ofs(DataFile); 
       double dx=Xmax/N;     
  double y=Ymin,z=Zmin; 
  for(int i=0;i<N;i++)
  {
    y=y+z*dx;
    z=z-C*y*dx;
    ofs << dx*(i+1) << " " << y << std::endl;
  }
       ofs.close();
    }
};
