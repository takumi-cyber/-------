#pragma once
#include <vector>

class EulerSolver {
public:
    EulerSolver(double y0, double t0, double h)
        : y(y0), t(t0), h(h) {
        t_list.push_back(t);
        y_list.push_back(y);
    }

    // 1ステップ進める
    void step() {
        y += h * f(t, y);
        t += h;
        t_list.push_back(t);
        y_list.push_back(y);
    }

    // 任意の時刻まで進める
    void solve(double t_end) {
        while (t < t_end) {
            step();
        }
    }

    double getY() const { return y; }
    double getT() const { return t; }
    const std::vector<double>& getTList() const { return t_list; }
    const std::vector<double>& getYList() const { return y_list; }

private:
    // y' = -y
    double f(double /*t*/, double y) {
        return -y;
    }

    double y;
    double t;
    double h;
    std::vector<double> t_list;
    std::vector<double> y_list;
};