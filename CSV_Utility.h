///////////////////////////////////////////////////////////////////////////////
//!
//! @file		CSV_Utility.h
//! 
//! @brief		A CSV utility class to parse and write CSV files. 
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
#if defined _WIN32
#include	<windows.h>					// Windows necessary stuff
#include	<direct.h>					// Make Directory
#else
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<unistd.h>
#endif
//
#include <fstream>						// File Stream
#include <vector>                       // Vectors
#include <iterator>						// Iterating vector
#include <sstream>                      // Stream for parsing strings
#include <string>                       // Strings
#include <iostream>						// Standard IO
#include <mutex>						// Data protection
#include <filesystem>					// Checking for file extension
//
#include "CSV_Info.h"					// CSV Utility Information
// 
//	Defines:
//          name                        reason defined
//          --------------------        ---------------------------------------
#ifndef     CSV_UTILITY					// Define the csv utility class. 
#define     CSV_UTILITY
#endif
//
///////////////////////////////////////////////////////////////////////////////

//! @brief A CSV utility class to parse and write CSV files. 
class CSV_Utility
{
public:
	//! @brief Default Constructor
	CSV_Utility();

	//! @brief Overloaded Constructor
	//! @param filename - [in] - string containing the desired filename to read or write to.
	//! @param mode - [in] - the mode of the CSV utility (read, write, readwrite)
	CSV_Utility(const std::string filename, const UTILITY_MODE mode);

	//! @brief Default Deconstructor
	~CSV_Utility();

	//! @brief Set the filename of the CSV file to be opened/created
	//! @param filename - [in] - filepath + name to be opened. 
	//! @return int: -1 if file is already open, 0 if failed, 1 if successful. 
	int SetFileName(const std::string filename);

	//! @brief Changes the delimiter used when parsing. 
	//! @param delimiter - [in] - Character to use as a delimiter.
	//! @return bool: true if successful, false is failed
	bool ChangeDelimiter(const char delimiter);

	//! @brief Change the mode of the CSV utility
	//! @param mode - [in] - UTILITY_MODE to open the file as
	//! @return bool: true if successful, false is failed
	bool ChangeCSVUtilityMode(const UTILITY_MODE mode);

	//! @brief Write out column headers.
	//! @param names - [in] - vector of strings to write as columns headers
	//! @return int: -1 on error, else the number of columns successfully written. 
	int WriteColumnHeaders(const std::vector<std::string>& names);

	//! @brief A function to write out a vector of any type to the csv file.
	//! @note This function is implemented in the header because of the use of template.
	//! @param A vector of any type <template> to store the values into. 
	//! @return int: -1 on error, else the number of values read. 
	template<typename T>
	int WriteRow(const std::vector<T>& values)
	{
		// Make sure file is open and we are in a write mode
		if (!mFile.is_open() || (mMode != UTILITY_MODE::WRITE_APPEND && mMode != UTILITY_MODE::WRITE_TRUNC && 
								mMode != UTILITY_MODE::READ_WRITE_APPEND && mMode != UTILITY_MODE::READ_WRITE_TRUNC))
		{
			return false;
		}

		// Verify file handle is good.  
		if (mFile.good() || mFile.eof())
		{
			// write the values to the file, adding the delimited in between. 
			int count = 0;
			for (typename std::vector<T>::const_iterator it = values.begin(); it != values.end(); ++it)
			{
				mFile << *it;
				if (it + 1 != values.end())
				{
					mFile << dCSVFileInfo.delimiter;
				}
				count++;
			}
			mFile << "\n";

			// Increment the number of rows and return count.
			dCSVFileInfo.n_rows++;
			return count;
		}
		else
		{
			CatchFailReason();
		}

		// Default return
		return -1;
	}

	//! @brief Read a row of data from the file.
	//! @param values - [in] - A string that contains the read line of data. 
	//! @param row - [in] - if zero (0), reads current position, else reads specified row. 
	//! @return bool: True if successful read, false if fail. 
	bool ReadRow(std::string& values, const int row);

	//! @brief Read a column of data from the file.
	//! @param values - [out] - A vector of strings that contains the read column of data. 
	//! @param column - [in] - reads specified column. 
	//! @return bool: True if successful read, false if fail. 
	bool ReadColumn(std::vector<std::string>& values, const int column);

	//! @brief Get the column headers of a CSV file ( line 1 ). 
	//! @param names - [out] - a vector of strings to store the parsed column names. 
	//! @return int: -1 on error, else the number of column names. 
	int GetColumnHeaders(std::vector<std::string>& names);

	//! @brief Get the number of columns in a CSV file. 
	//! @return int: -1 on error, else the number of columns in the file
	int GetNumberOfColumns();

	//! @brief Get the number of rows in the open file.
	//! @return int: -1 if no file opened, else the number of rows found (including column names)
	int GetNumberOfRows();

	//! @brief Write a full group of data to a CSV file
	//! @param filename - [in] - char array containing the filename to be opened and written to
	//! @param values - [in] - a vector of any type to write 
	//! @return int: -1 on error, else the number of values successfully written to file. 
	int WriteFullCSV(const std::string filename, const std::vector<std::vector<std::string>>& values);

	//! @brief Parse a CSV Buffer.
	//! @param buffer - [in] - A char buffer to be parsed.
	//! @param values - [out] - A vector to store the parsed values into.
	//! @return -1 on error, else the number of values successfully parsed. 
	int ParseCSVBuffer(char* buffer, std::vector<std::string>& values);

	//! @brief Read in a CSV file and parse it. 
	//! @param handle - [in] - A pointer to a file handle. 
	//! @param values - [out] - A vector of a vector of strings to store the parsed values into.
	//! @return -1 on error, else the number of values successfully parsed. 
	int ParseCSVFile(std::fstream* handle, std::vector<std::vector<std::string>>& values);

	//! @brief Prints a CSV files data contents to console. 
	void PrintCSVData();

	//! @brief Check if the file is at the end.
	//! @return bool: true if the end, false if not.
	bool IsEndOfFile();

	//! @brief Get the information for the current file. 
	//! @param info - [out] - CSVFileInfo structure to place the files information.
	//! @return bool: true if data is vaid, else false. 
	bool GetFileInfo(CSVFileInfo& info);

	//! @brief Get the information for the current file. 
	//! @param filename - [out] - the current filename opened.
	//! @param size - [out] - the size of the file. 
	//! @param columns - [out] - the number of columns
	//! @param rows - [out] - the number of rows
	//! @return bool: true if data is vaid, else false. 
	bool GetFileInfo(std::string& filename, double size, int columns, int rows);

	//! @brief Clear the contents of the file
	//! @return bool: true if successful, false if failed.
	bool ClearFile();

	//! @brief Get the file size in bytes of the file. 
	//! @return size_t: The size of the file in bytes.
	size_t GetFileSize();

	//! @brief Opens a file stream
	//! @return bool: true if successful, false if failed. 
	bool OpenFile();

	//! @brief Check if a file stream is open.
	//! @return bool: true if open, false if closed. 
	bool IsFileOpen();

	//! @brief Closes a file if its open.
	//! @return bool: true if successful, false if failed or not open.
	bool CloseFile();

protected:
private:
	//! @brief Catch a fail reason for the fstream instance
	void CatchFailReason();

	//! @brief Update the file information to the data structure
	void UpdateFileInfo();

	std::string			mUser;					//!< Name for the class when using CPP_Logger
	CSVFileInfo			dCSVFileInfo;			//!< Current CSV File
	std::fstream		mFile;					//!< File stream for in and out
	std::string			mExtension;				//!< File Extension
	UTILITY_MODE		mMode;					//!< Current mode of the utility
};
