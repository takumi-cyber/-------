#pragma once
#include <iostream>
#include <vector>
#include <cmath>
template <typename T>
class pvector{
    private:
        std::vector<T> data;
        auto pindex(int index) const{
            int n = data.size();
            return (index % n + n) % n;
        }
    public:
        pvector()=default;
        pvector(size_t size):data(size){}
        T &operator[](int index)
        {return data[pindex(index)];}
        const T &operator[](int index) const
        {return data[pindex(index)];}
        auto size() const
        {return data.size();}
        void resize(size_t newSize)
        {data.resize(newSize);}
        auto begin() { return data.begin(); }
        auto end() { return data.end(); }
        auto begin() const { return data.begin(); }
        auto end() const { return data.end(); }
        T &front() { return data.front(); }
        const T &front() const { return data.front(); }
        T &back() { return data.back(); }
        const T &back() const { return data.back(); }
};