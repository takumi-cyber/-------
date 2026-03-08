#pragma once
#include <iostream>
#include <cassert>
#include <vector>
template <typename T>

class MyArray{
    private:
        std::vector<size_t> _sizes;
        std::vector<T> _data;
    public:
        const size_t size() const
        {
            size_t s = 1;
            for(auto d : _sizes) s *= d;
            return s;
        }
        double *data() 
        {return _data.data();}
        const double *data()
        const {return _data.data();} 
        double &operator[](const int k) // myArray[k]で1次元配列扱いをしよう. これでk番目のデータ
        {
            return _data[k];
        }
        const double &operator[](const int k)
        const {
                return _data[k];
              }
        auto begin() { return _data.begin(); }
        auto end() { return _data.end(); }
        const auto begin() const { return _data.begin(); }
        const auto end() const { return _data.end(); }
        auto &front() { return _data.front(); }
        auto &back() { return _data.back(); }
        const auto &front() const { return _data.front(); }
        const auto &back() const { return _data.back(); }

        //1D
        MyArray(const size_t n0){resize(n0);}
        void resize(const size_t n0)
        {
            _sizes.resize(1);
            _sizes[0]=n0;
            _data.resize(size());
        }
        const size_t serial(const int i0) const
        {
            assert(0<=i0);
            assert(i0<_sizes[0]);
            return i0;
        }
        double &operator()(const int i0)
        {
            assert(_sizes.size()==1);
            return _data[serial(i0)];
        }
        const double &operator()(const int i0) const
        {
            assert(_sizes.size()==1);
            return _data[serial(i0)];
        }
        //2D
        MyArray(const size_t n0, const size_t n1){resize(n0,n1);}
        void resize(const size_t n0, const size_t n1)
        {
            _sizes.resize(2);
            _sizes[0]=n0;
            _sizes[1]=n1;
            _data.resize(size());
        }
        const size_t serial(const int i0, const int i1) const
        {
            assert(i1>=0);
            assert(i1<_sizes[1]);
            return serial(i0)+_sizes[0]*i1;
        }
        double &operator()(const int i0, const int i1) // オペレータ1:(i,j)を呼び出す
        {
            assert(_sizes.size()==2);
            return _data[serial(i0, i1)];
        }
        const double &operator()(const int i0, const int i1) // オペレータ1:(i,j)を呼び出す
        const {
            assert(_sizes.size()==2);
            return _data[serial(i0, i1)];
        }
        //3D
        MyArray(const size_t n0, const size_t n1, const size_t n2){resize(n0,n1,n2);}
        void resize(const size_t n0, const size_t n1, const size_t n2)
        {
            _sizes.resize(3);
            _sizes[0]=n0;
            _sizes[1]=n1;
            _sizes[2]=n2;
            _data.resize(size());
        }
        const size_t serial(const int i0, const int i1, const int i2) const
        {
            assert(i2>=0);
            assert(i2<_sizes[2]);
            return serial(i0, i1)+_sizes[0]*_sizes[1]*i2;
        }
        double &operator()(const int i0, const int i1, const int i2) // オペレータ1:(i,j)を呼び出す
        {
            assert(_sizes.size()==3);
            return _data[serial(i0, i1, i2)];
        }
        const double &operator()(const int i0, const int i1, const int i2) const // オペレータ1:(i,j)を呼び出す
        {
            assert(_sizes.size()==3);
            return _data[serial(i0, i1, i2)];
        }
};