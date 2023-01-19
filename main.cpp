// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CSV_Utility.h"

int main()
{
    CSV_Utility csv;
    csv.SetFileName("test/temp.csv");
    csv.ChangeCSVUtilityMode(UTILITY_MODE::READWRITE);
    csv.ChangeCSVUtilityWritingType(UTILITY_WRITE_TYPE::TRUNC);
    csv.OpenFile();

    std::vector<std::string> cols{ "one", "two", "three" };
    csv.WriteColumnHeaders(cols);

    std::vector<int> vi{ 1,2,3 };
    csv.WriteRow(vi);

    std::vector<double> vf{ 01.23, 23.45, 45.67 };
    csv.WriteRow(vf);

    std::vector<char> vc{ 'a','b','c' };
    csv.WriteRow(vc);
    
    printf("\nFile Size: %zu\n", csv.GetFileSize());

    int c = csv.GetNumberOfColumns();
    int r = csv.GetNumberOfRows();
    printf("Columns: %d , Rows: %d\n", c, r);

    std::string s = "";
    csv.ReadRow(s, 4);
    printf("ReadRow 4: %s\n", s.c_str());
    csv.ReadRow(s, 1);
    printf("ReadRow 1: %s\n\n", s.c_str());

    printf("Print Test:\n");
    csv.PrintCSVData();

    csv.ReadRow(s, 2);
    printf("\nReadRow 2: %s\n", s.c_str());

    printf("\nColumn Names:\n");
    std::vector<std::string> names;
    csv.GetColumnHeaders(names);
    
    for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); ++it)
    {
        std::string temp = *it;

        if (it == names.end()-1)
        {
            printf_s("%s\n", temp.c_str());
        }
        else
        {
            printf_s("%s%s", temp.c_str(), ", ");
        }
    }

    csv.CloseFile();
    return 0;
}
