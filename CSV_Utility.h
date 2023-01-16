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
#include <vector>                       // Vectors
#include <sstream>                      // Stream for parsing strings
#include <string>                       // Strings
#include <iostream>						// Standard IO
// 
//	Defines:
//          name                        reason defined
//          --------------------        ---------------------------------------
#ifndef     CSV_UTILITY					// Define the csv utility class. 
#define     CSV_UTILITY
#endif
///////////////////////////////////////////////////////////////////////////////

class CSV_Utility
{
public:
	//! @brief Default Constructor
	CSV_Utility();

	//! @brief Default Deconstructor
	~CSV_Utility();

	//! @brief Changes the delimiter used when parsing. 
	//! @param newDelimiter - Character to use as a delimiter.
	//! @return bool - true if changed, false if error;
	bool ChangeDelimiter(char newDelimiter);

	//! @brief Write data to a CSV file
	//! @param filename - char array containing the filename to be opened and written to
	//! @param values - a vector of any type to write 
	//! @return -1 on error, else the number of values successfully written to file. 
	template <typename T>
	int WriteCSV(char* filename, std::vector<T>& values);

	//! @brief Parse a CSV Buffer.
	//! @param buffer - A char buffer to be parsed.
	//! @param values - A vector to store the parsed values into.
	//! @return -1 on error, else the number of values successfully parsed. 
	template <typename T>
	int ParseCSVBuffer(char* buffer, std::vector<T>& values);

	//! @brief Read in a CSV file and parse it. 
	//! @param handle - A pointer to a file handle. 
	//! @param values - A vector to store the parsed values into.
	//! @return -1 on error, else the number of values successfully parsed. 
	template <typename T>
	int ParseCSVFile(FILE* handle, std::vector<T>& values);

protected:
private:
	char delimeter;				//!< char used as a delimiter when parsing
};
