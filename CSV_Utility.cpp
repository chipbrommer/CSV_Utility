///////////////////////////////////////////////////////////////////////////////
//!
//! @file		CSV_Utility.cpp
//! 
//! @brief		Implementation for the CSV_Utility class
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
#include "CSV_Utility.h"					// CSV Parser class header
///////////////////////////////////////////////////////////////////////////////

CSV_Utility::CSV_Utility()
{
	delimeter = ',';
}

CSV_Utility::~CSV_Utility()
{
}

bool CSV_Utility::ChangeDelimiter(char newDelimiter)
{
	printf_s("Received new delimiter: %c", newDelimiter);
	delimeter = newDelimiter;
	return true;
}

template<typename T>
int CSV_Utility::WriteCSV(char* filename, std::vector<T>& values)
{
	return 0;
}

template<typename T>
int CSV_Utility::ParseCSVBuffer(char* buffer, std::vector<T>& values)
{
	return 0;
}

template <typename T>
int CSV_Utility::ParseCSVFile(FILE* handle, std::vector<T>& values)
{

}