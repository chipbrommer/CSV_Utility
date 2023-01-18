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

    std::vector<std::string> cols{ "one", "two", "three" };
    std::vector<int> vi{ 1,2,3 };
    std::vector<double> vf{ 01.23, 23.45, 45.67};
    std::vector<char> vc{ 'a','b','c' };
    //csv.WriteColumnHeaders(cols);
    csv.WriteRow(vi);
    csv.WriteRow(vf);
    csv.WriteRow(vc);
    
    printf("\nFile Size: %d\n", csv.GetFileSize());

    //int c = csv.GetNumberOfColumns();
    //int r = csv.GetNumberOfRows();
    //printf("\nColumns: %d , Rows: %d", c, r);

    std::string s = "";
    csv.ReadRow(s, 0);
    printf("ReadRow: %s\n", s.c_str());

    csv.PrintCSVData();

    std::vector<std::string> names;
    csv.GetColumnNames(names);
    
    for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); ++it)
    {
        printf_s("%s %c", *it, ',');
    }

    csv.CloseFile();
    return 0;
}
