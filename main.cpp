// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CSV_Utility.h"

int main()
{
    CSV_Utility csv;
    csv.SetFileName("test/temp.csv");
    csv.ClearFile();
    csv.ChangeCSVUtilityMode(UTILITY_MODE::READWRITE, true);
    csv.ChangeCSVUtilityWritingType(UTILITY_WRITE_TYPE::TRUNC, true);
    csv.OpenFile();

    std::vector<std::string> cols{ "one", "two", "three" };
    std::vector<int> vals{ 1,2,3 };
    csv.WriteRow(vals);
    csv.WriteRow(vals);
    csv.WriteRow(vals);
    csv.WriteColumnHeaders(cols);
    std::cout << "\n" << csv.GetFileSize();

    int c = csv.GetNumberOfColumns();
    int r = csv.GetNumberOfRows();
    printf("\nColumns: %d , Rows: %d", c, r);

    std::string v = "";
    csv.ReadRow(v, 0);

    printf("\n\n%s", v.c_str());

    csv.CloseFile();
}