///////////////////////////////////////////////////////////////////////////////
//!
//! @file		CSV_Info.h
//! 
//! @brief		Info file for the CSV Utility
//! 
//! @author		Chip Brommer
//! 
//! @date		< 1 / 15 / 2022 > Initial Start Date
//!
/*****************************************************************************/
#pragma once
///////////////////////////////////////////////////////////////////////////////
//
//  Include files:
//          name                        reason included
//          --------------------        ---------------------------------------
#include <string>                       // strings
#include <vector>                       // vectors

///////////////////////////////////////////////////////////////////////////////

//! @brief Define Date Class
class CSVFileInfo
{
public:
    std::string filename;					// Filename 
    std::vector<std::string> col_names;		// CSV column names 
    char delimiter;							// Delimiting character 
    int n_rows;							    // Number of rows in a file 
    int n_cols;							    // Number of columns in a CSV 
    size_t filesize;						// Size of the file in bytes

    // constructor initializes everything
    CSVFileInfo(std::string filename = "",
                std::vector<std::string> col_names = {},
                char delimiter = '\0',
                int n_rows = 0,
                int n_cols = 0,
                size_t filesize = 0) :
                filename(filename), col_names(col_names), delimiter(delimiter),
                n_rows(n_rows), n_cols(n_cols), filesize(filesize)

    {}

    //! @brief Does struct have valid data ? 
    bool Valid(void) const
    {
        return (!filename.empty()) && (!col_names.empty()) && (delimiter != '\0') &&
            (n_rows != 0) && (n_cols != 0) && (filesize != 0);
    }

    // Output data to neatly.
    friend std::ostream& operator<<(std::ostream& os, const CSVFileInfo& csv)
    {
        os  << "CSV Data: " << "\n";
        csv.Valid() ? os << "\tValid:             Valid\n"  : os << "\tValid:             Not Valid\n"; 
        os  << "\tFilename:          " << csv.filename << "\n"
            << "\tDelimiter:         '" << csv.delimiter << "'\n"
            << "\tNumber of Rows:    " << csv.n_rows << "\n"
            << "\tNumber of Columns: " << csv.n_cols << "\n"
            << "\tColumn Names:      " << "\n";

        for (std::vector<std::string>::const_iterator it = csv.col_names.begin(); it != csv.col_names.end(); ++it)
        {
            std::string temp = *it;
            int index = (int)std::distance(csv.col_names.begin(), it);
            printf_s("\t\tColumn %d: \"%s\"\n", index+1, temp.c_str());
        }

        os << "\tFile Size:         " << csv.filesize << "\n";

        return os;
    }
};

enum UTILITY_MODE
{
    READ = std::ios::in,
	WRITE_APPEND = std::ios::out | std::ios::app,
    WRITE_TRUNC = std::ios::out | std::ios::trunc,
	READ_WRITE_APPEND = std::ios::in | std::ios::out | std::ios::app,
    READ_WRITE_TRUNC = std::ios::in | std::ios::out | std::ios::trunc,
};