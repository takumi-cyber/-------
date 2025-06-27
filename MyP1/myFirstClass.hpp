#pragma once
#include <iostream>

class myFirstClass {
public:
    myFirstClass() = default;
    ~myFirstClass() = default;

    void setValue(int v) { value = v; }
    int getValue() const { return value; }

private:
    int value = 0;
};