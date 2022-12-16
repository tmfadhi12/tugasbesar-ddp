// coret=coret.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>

int main()
{
    srand(time(0));
    int random = 0 + (rand() % 999);

    std::cout << random;
}
