// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CSV_Utility.h"

int main()
{
    std::vector<int> temp{ 0,1,2,3,4,5,6 };
    CSV_Utility csv;
    csv.WriteCSV((char*)"test/out.csv", temp);
}