// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CSV_Utility.h"

int main()
{
    CSV_Utility csv;
    csv.PrintAnyCSVFile("test/temp.csv");
   
    csv.SetFileName("test/temp.csv");
    csv.ChangeCSVUtilityMode(UTILITY_MODE::READ_WRITE_TRUNC);
    csv.OpenFile();

    std::vector<std::vector<std::string>> tester{ {"a", "b", "c", "d"},
                                                    {"e", "f", "g", "h"},
                                                    {"i", "j", "k", "l"},
                                                    {"m", "n", "o", "p"},
    };
    if (csv.WriteAFullCSV("test/any.csv", tester))
    {
        printf("\nreturn success\n\n");
    }

    csv.SetFileName("test/temp.csv");
    csv.ChangeCSVUtilityMode(UTILITY_MODE::READ_WRITE_TRUNC);
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

    printf("Read Column Test:\n");
    std::vector<std::string>ci;
    csv.ReadColumn(ci, 2);
    for (std::vector<std::string>::iterator it = ci.begin(); it != ci.end(); ++it)
    {
        std::string temp = *it;
        int index = (int)std::distance(ci.begin(), it);
        printf_s("\t\tRow %d: \"%s\"\n", index + 1, temp.c_str());
    }

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

    printf("\nStruct Test:\n");
    CSVFileInfo cs;
    csv.GetFileInfo(cs);
    std::cout << cs;
    
    csv.CloseFile();

    return 0;
}