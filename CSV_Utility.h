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
//	Defines:
//          name                        reason defined
//          --------------------        ---------------------------------------
#ifndef     CSV_UTILITY					// Define the csv utility class. 
#define     CSV_UTILITY
#endif
//
///////////////////////////////////////////////////////////////////////////////

	/** Returned by get_file_info() */
struct CSVFileInfo 
{
	std::string filename;					// Filename 
	std::vector<std::string> col_names;		// CSV column names 
	char delimiter;							// Delimiting character 
	size_t n_rows;							// Number of rows in a file 
	size_t n_cols;							// Number of columns in a CSV 
};

enum class UTILITY_MODE : const int
{
	WRITE		= 1,
	READ		= 2,
	READWRITE	= 3,
};

enum class UTILITY_WRITE_TYPE : const int
{
	APPEND,
	TRUNC,
};

//! @brief A CSV utility class to parse and write CSV files. 
class CSV_Utility
{
public:
	//! @brief Default Constructor
	CSV_Utility();

	//! @brief Overloaded Constructor
	//! @param filename - string containing the desired filename to read or write to.
	//! @param mode - the mode of the CSV utility (read, write, readwrite)
	//! @param type - the type of the CSV utility (trunc, append)
	CSV_Utility(const std::string filename, const UTILITY_MODE mode, const UTILITY_WRITE_TYPE type);

	//! @brief Default Deconstructor
	~CSV_Utility();

	//! @brief Set the filename of the CSV file to be opened/created
	//! @param filename - filepath + name to be opened. 
	//! @return int: -1 if file is already open, 0 if failed, 1 if successful. 
	int SetFileName(const std::string filename);

	//! @brief Changes the delimiter used when parsing. 
	//! @param delimiter - Character to use as a delimiter.
	//! @return bool: true if successful, false is failed
	bool ChangeDelimiter(const char delimiter);

	//! @brief Change the mode of the CSV utility
	//! @param mode - UTILITY_MODE to open the file as
	//! @return bool: true if successful, false is failed
	bool ChangeCSVUtilityMode(const UTILITY_MODE mode);

	//! @brief Change the mode of the CSV utility
	//! @param mode - UTILITY_WRITE_TYPE to open the file as
	//! @return bool: true if successful, false is failed
	bool ChangeCSVUtilityWritingType(const UTILITY_WRITE_TYPE type);

	//! @brief Write out column headers.
	//! @param names - vector of strings to write as columns headers
	//! @return int: -1 on error, else the number of columns successfully written. 
	int WriteColumnHeaders(const std::vector<std::string>& names);

	//! @brief A function to write out a vector of any type to the csv file.
	//! @note This function is implemented in the header because of the use of template.
	//! @param A vector of any type <template> to store the values into. 
	//! @return int: -1 on error, else the number of values read. 
	template<typename T>
	int WriteRow(const std::vector<T>& values)
	{
		if (!IsFileOpen())
		{
			return -1;
		}
		if (mFile.good())
		{
			int count = 0;
			for (typename std::vector<T>::const_iterator it = values.begin(); it != values.end(); ++it)
			{
				mFile << *it;
				if (it + 1 != values.end())
				{
					mFile << mDelimiter;
				}
				count++;
			}
			mFile << "\n";
			return count;
		}
		else
		{
			CatchFailReason();
		}
		return -1;
	}

	//! @brief Read a row of data from the file.
	//! @param values - A string that contains the read line of data. 
	//! @param row - if zero (0), reads current position, else reads specified row. 
	//! @return bool: True if successful read, false if fail. 
	bool ReadRow(std::string& values, const int row);

	//! @brief Read a column of data from the file.
	//! @param values - A vector of strings that contains the read column of data. 
	//! @param column - reads specified column. 
	//! @return bool: True if successful read, false if fail. 
	bool ReadColumn(std::vector<std::string>values, const int column);

	//! @brief Get the column headers of a CSV file ( line 1 ). 
	//! @param names - a vector of strings to store the parsed column names. 
	//! @return int: -1 on error, else the number of column names. 
	int GetColumnHeaders(std::vector<std::string>& names);

	//! @brief Get the number of columns in a CSV file. 
	//! @return int: -1 on error, else the number of columns in the file
	int GetNumberOfColumns();

	//! @brief Get the number of rows in the open file.
	//! @return int: -1 if no file opened, else the number of rows found (including column names)
	int GetNumberOfRows();

	//! @brief Write a full group of data to a CSV file
	//! @param filename - char array containing the filename to be opened and written to
	//! @param values - a vector of any type to write 
	//! @return int: -1 on error, else the number of values successfully written to file. 
	int WriteFullCSV(const std::string filename, const std::vector<int>& values);

	//! @brief Parse a CSV Buffer.
	//! @param buffer - A char buffer to be parsed.
	//! @param values - A vector to store the parsed values into.
	//! @return -1 on error, else the number of values successfully parsed. 
	int ParseCSVBuffer(char* buffer, std::vector<std::string>& values);

	//! @brief Read in a CSV file and parse it. 
	//! @param handle - A pointer to a file handle. 
	//! @param values - A vector to store the parsed values into.
	//! @return -1 on error, else the number of values successfully parsed. 
	int ParseCSVFile(FILE* handle, std::vector<int>& values);

	//! @brief Prints a CSV files data contents to console. 
	void PrintCSVData();

	//! @brief Check if the file is at the end.
	//! @return bool: true if the end, false if not.
	bool IsEndOfFile();

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
	void CatchFailReason();

	std::string			mUser;					//!< Name for the class when using CPP_Logger
	//CSVFileInfo		dCSVFileInfo;			//!< Current CSV File
	std::fstream		mFile;					//!< File stream for in and out
	std::string			mExtension;				//!< File Extension
	UTILITY_MODE		mMode;					//!< Current mode of the utility
	UTILITY_WRITE_TYPE	mType;					//!< Current writing type of the utility
	std::string			mFilename;				//!< Current filename
	bool				mFileSet;				//!< Flag to check if filename has been set.
	char				mDelimiter;				//!< Delimiter to use in file output
};
