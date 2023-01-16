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

//! @brief A CSV utility class to parse and write CSV files. 
class CSV_Utility
{
public:
	//! @brief Default Constructor
	CSV_Utility();

	CSV_Utility(const std::string filename);

	//! @brief Default Deconstructor
	~CSV_Utility();

	//! @brief Changes the delimiter used when parsing. 
	//! @param delimiter - Character to use as a delimiter.
	void ChangeDelimiter(char delimiter);

	int WriteColumnHeaders(const FILE* handle, const std::vector<std::string>& names);

	int WriteRow(const FILE* handle, std::vector<int>const& values);

	//! @brief Write a full group of data to a CSV file
	//! @param filename - char array containing the filename to be opened and written to
	//! @param values - a vector of any type to write 
	//! @return -1 on error, else the number of values successfully written to file. 
	//template<class T>
	int WriteFullCSV(const std::string filename, const std::vector<int>& values);

	int GetColumnNames(const std::vector<std::string>& names);

	int GetNumberOfRows();

	int GetNumberOfRows(const std::string filename);

	int GetNumberOfRows(const FILE* handle);

	//! @brief Parse a CSV Buffer.
	//! @param buffer - A char buffer to be parsed.
	//! @param values - A vector to store the parsed values into.
	//! @return -1 on error, else the number of values successfully parsed. 
	template <typename T>
	int ParseCSVBuffer(const char* buffer, std::vector<T>& values);

	//! @brief Read in a CSV file and parse it. 
	//! @param handle - A pointer to a file handle. 
	//! @param values - A vector to store the parsed values into.
	//! @return -1 on error, else the number of values successfully parsed. 
	template <typename T>
	int ParseCSVFile(FILE* handle, std::vector<T>& values);

	int PrintCSVData(int rows, int columns, std::vector<int>& values);

	bool IsFileOpen();

	bool EndOfFile();

	int ClearFile();

	size_t GetFileSize();

protected:
private:
	int OpenFile();
	int CloseFile();

	std::string		mUser;					//!< Name for the class when using CPP_Logger
	char			mDelimeter;				//!< char used as a delimiter when parsing
	std::string		mExtension;				//!< File extension to be used. 
	std::string		mFilename;
	FILE*			mHandle;
};
