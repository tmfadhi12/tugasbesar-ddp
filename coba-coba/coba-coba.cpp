// coba-coba.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include<sstream> 
using namespace std;

int main()
{
	srand(time(0));
	int random = 100000000 + (rand() % 999999999);
	stringstream ss;
	ss << random;
	string test;
	ss >> test;
	string rekening = "069" + test;
	cout << random;
    /*srand((unsigned)time(NULL));*/
	
}