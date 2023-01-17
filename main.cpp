// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CSV_Utility.h"

int main()
{
    CSV_Utility csv;
    csv.SetFileName("test/temp.csv");
    //csv.ClearFile();
    csv.ChangeCSVUtilityMode(UTILITY_MODE::READ, true);
    csv.ChangeCSVUtilityWritingType(UTILITY_WRITE_TYPE::APPEND, true);
    csv.OpenFile();

    //std::vector<std::string> cols{ "one", "two", "three" };
    //std::vector<int> vals{ 1,2,3 };
    //csv.WriteRow(vals);
    //csv.WriteRow(vals);
    //csv.WriteRow(vals);
    //csv.WriteColumnHeaders(cols);
    //std::cout << "\n" << csv.GetFileSize();

    //int c = csv.GetNumberOfColumns();
    //int r = csv.GetNumberOfRows();
    //printf("\nColumns: %d , Rows: %d", c, r);

    std::string s = "";
    csv.ReadRow(s, 0);
    printf("ReadRow: %s\n", s.c_str());

    csv.PrintFile();

    csv.CloseFile();
    
    return 0;
}