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
#include "CSV_Utility.h"				// CSV Parser class header
///////////////////////////////////////////////////////////////////////////////

CSV_Utility::CSV_Utility()
{
	mUser = "CSVUtility";
	dCSVFileInfo.delimiter = ',';
	mExtension = ".csv";
}

CSV_Utility::CSV_Utility(const std::string filename, const UTILITY_MODE mode = UTILITY_MODE::READ_WRITE_TRUNC)
{
	dCSVFileInfo.filename = filename;
	mUser = "CSVUtility";
	dCSVFileInfo.delimiter = ',';
	mExtension = ".csv";
	mMode = mode;
}

CSV_Utility::~CSV_Utility()
{
	mFile.close();
}

int CSV_Utility::SetFileName(const std::string filename)
{
	// File already open, cant set name. 
	if (mFile.is_open())
	{
		return -1;
	}

	// Set name. 
	dCSVFileInfo.filename = filename;

	// if filenames are equal, success. 
	if (dCSVFileInfo.filename == filename)
	{
		return 1;
	}
	// failed. 
	return 0;
}

bool CSV_Utility::ChangeDelimiter(const char delimiter)
{
	// Notify of change
#ifdef CPP_LOGGER
	log->AddEntry(LOG_LEVEL::LOG_ERROR, mUser, "Received new delimiter: %c", delimiter);
#else
	printf_s("%s - Received new delimiter: %c\n", mUser.c_str(), delimiter);
#endif

	// Set the new delimiter and verify return true if good.
	dCSVFileInfo.delimiter = delimiter;
	if (dCSVFileInfo.delimiter == delimiter)
	{
		return true;
	}

	// Default return
	return false;
}

bool CSV_Utility::ChangeCSVUtilityMode(const UTILITY_MODE mode)
{
	bool open = false;
	// If the file is open, close it. 
	if (mFile.is_open())
	{
		open = true;
		mFile.close();
	}

	// Set the new mode
	mMode = mode;

	// If modes equal, attempt to re-open file. 
	if(mMode == mode)
	{
		// If file was open before change, attempt to re-open. 
		if (open)
		{
			// If file is open is successful, return true, else return false.
			if (OpenFile())
			{
				return true;
			}
			return false;
		}
	}

	// Default return
	return false;
}

int CSV_Utility::WriteColumnHeaders(const std::vector<std::string>& names)
{
	// TODO 	
	/*
		if not trunc mode, or in append mode and at the top of the file, return fail.
		or do we want to force insert? <- this could be costly performance wise to copy full content, insert headers, and then place all data back. 
	*/

	// Make sure file is open and we are in a write mode
	if (!mFile.is_open() || (mMode != UTILITY_MODE::WRITE_APPEND && mMode != UTILITY_MODE::WRITE_TRUNC &&
		mMode != UTILITY_MODE::READ_WRITE_APPEND && mMode != UTILITY_MODE::READ_WRITE_TRUNC))
	{
		return -1;
	}

	// Make sure we are in trunc mode

	// Make sure we are at the top of the file. 
	mFile.seekg(0, std::ios::beg);

	int count = WriteRow(names);

	return count;
}

bool CSV_Utility::ReadRow(std::string& values, const int row = 0)
{
	// Make sure file is open and we are in a read mode
	if (!mFile.is_open() || (mMode != UTILITY_MODE::READ && mMode != UTILITY_MODE::READ_WRITE_APPEND && mMode != UTILITY_MODE::READ_WRITE_TRUNC))
	{
		return false;
	}

	// Verify file handle is good. 
	if (mFile.good() || mFile.eof())
	{
		// if reading current position, get line and return. 
		if (row == 0)
		{
			std::getline(mFile, values);
			return true;
		}

		// Save current position and then go to top of file. 
		auto curr_pos = mFile.tellg();
		mFile.seekg(0, std::ios::beg);

		// Find the row and get the contents
		for (int i = 1; i < row+1; i++)
		{
			if (i == row)
			{
				std::getline(mFile, values);
				break;
			}

			std::string temp;
			std::getline(mFile, temp);
		}

		// Return to position and return true
		mFile.clear();
		mFile.seekp(curr_pos);
		return true;
	}
	else
	{
		CatchFailReason();
	}

	// Default return
	return false;
}

bool CSV_Utility::ReadColumn(std::vector<std::string>& values, const int column)
{
	// Make sure file is open and we are in a read mode
	if (!mFile.is_open() || (mMode != UTILITY_MODE::READ && mMode != UTILITY_MODE::READ_WRITE_APPEND && mMode != UTILITY_MODE::READ_WRITE_TRUNC))
	{
		return false;
	}

	// make sure column is more than 0
	if (column < 1)
	{
#ifdef CPP_LOGGER
		Log* log = log->GetInstance();
		log->AddEntry(LOG_LEVEL::LOG_ERROR, mUser, "ReadColumn - Column input must be more than 0");
#else
		printf_s("%s - ReadColumn - Column input must be more than 0.\n", mUser.c_str());
#endif
	}

	// Verify file handle is good. 
	if (mFile.good() || mFile.eof())
	{
		// Save current position and then go to top of file. 
		auto curr_pos = mFile.tellg();
		mFile.seekg(0, std::ios::beg);

		// Update the file info, then loop through the rows, grabbing the desired column from each row.
		UpdateFileInfo();
		std::vector<std::string>temp;
		
		for (int rowNum = 1; rowNum < dCSVFileInfo.n_rows + 1; rowNum++)
		{
			std::string row;
			int count = 0;
			if (ReadRow(row, rowNum))
			{
				temp.clear();
				count = ParseCSVBuffer(const_cast<char*>(row.c_str()), temp);
			}

			values.push_back(temp[static_cast<std::vector<std::string, std::allocator<std::string>>::size_type>(column) - 1]);
		}
			
		// Return to position and return true
		mFile.clear();
		mFile.seekp(curr_pos);
		return true;
	}
	else
	{
		CatchFailReason();
	}

	// Default return
	return false;
}

bool CSV_Utility::RemoveRow(const int row)
{
	// TODO
	return false;
}

int CSV_Utility::GetColumnHeaders(std::vector<std::string>& names)
{
	// Make sure file is open and we are in a read mode
	if (!mFile.is_open() || (mMode != UTILITY_MODE::READ && mMode != UTILITY_MODE::READ_WRITE_APPEND && mMode != UTILITY_MODE::READ_WRITE_TRUNC))
	{
		return false;
	}

	// Verify file handle is good.  
	if (mFile.good() || mFile.eof())
	{
		// Read row 1 and parse the data to get the names.
		std::string row;
		int count = 0;
		if (ReadRow(row, 1))
		{
			count = ParseCSVBuffer(const_cast<char*>(row.c_str()), names);
		}

		// Return
		return count;
	}
	else
	{
		CatchFailReason();
	}

	// Default Return
	return 0;
}

int CSV_Utility::GetNumberOfColumns()
{
	// Make sure file is open and we are in a read mode
	if (!mFile.is_open() || (mMode != UTILITY_MODE::READ && mMode != UTILITY_MODE::READ_WRITE_APPEND && mMode != UTILITY_MODE::READ_WRITE_TRUNC))
	{
		return -1;
	}

	// Verify file handle is good. 
	if (mFile.good() || mFile.eof())
	{
		// Get the number of columns in row 1.
		std::string row;
		int count = 0;
		bool r = ReadRow(row, 1);
		if (r)
		{
			std::vector < std::string> values;
			count = ParseCSVBuffer(const_cast<char*>(row.c_str()), values);
		}

		// return number of columns
		return count;
	}
	else
	{
		CatchFailReason();
	}

	// Default return
	return 0;
}

int CSV_Utility::GetNumberOfRows()
{
	// Make sure file is open and we are in a read mode
	if (!mFile.is_open() || (mMode != UTILITY_MODE::READ && mMode != UTILITY_MODE::READ_WRITE_APPEND && mMode != UTILITY_MODE::READ_WRITE_TRUNC))
	{
		return -1;
	}

	// Verify file handle is good. 
	if (mFile.good() || mFile.eof())
	{
		// Get current position - then move to top of file. 
		auto curr_pos = mFile.tellg();
		mFile.seekg(0, std::ios::beg);

		// While loop to count numbers of rows. 
		int count = 0;
		std::string line = "";
		while (getline(mFile, line))
		{
			count++;
		}

		// Clear the state, return to position and return count
		mFile.clear();
		mFile.seekp(curr_pos);
		return count;
	}

	// Default return
	return -1;
}

bool CSV_Utility::WriteAFullCSV(const std::string filename, const std::vector<std::vector<std::string>>& values)
{
	// Get the current file info and save it - then close the file.
	std::string currFile = dCSVFileInfo.filename;
	auto currMode = mMode;
	std::streampos currPos;
	bool fileOpen = false;
	char currDelim = dCSVFileInfo.delimiter;
	if (mFile.is_open())
	{
		fileOpen = true;
		currPos = mFile.tellg();
		CloseFile();
	}

	// Set the new file info
	dCSVFileInfo.filename = filename;
	mMode = UTILITY_MODE::WRITE_TRUNC;
	dCSVFileInfo.delimiter = ',';

	// Open the file, if successful, write the data. 
	if (OpenFile())
	{
		for (int i = 0; i < values.size(); i++)
		{
			int r = WriteRow(values[i]);

			if (r < 0)
			{
				return false;
			}
		}

		CloseFile();
	}

	// Reopen the original file if it was opened. 
	if (fileOpen)
	{
		dCSVFileInfo.filename = currFile;
		dCSVFileInfo.delimiter = currDelim;
		mMode = currMode;

		if (OpenFile())
		{
			// Return to position
			mFile.clear();
			mFile.seekp(currPos);
			return true;
		}

		return false;
	}

	// Default return
	return true;
}

int CSV_Utility::ParseCSVBuffer(char* buffer, std::vector<std::string>& values)
{
	// Make sure file is open and we are in a read mode
	if (!mFile.is_open() || (mMode != UTILITY_MODE::READ && mMode != UTILITY_MODE::READ_WRITE_APPEND && mMode != UTILITY_MODE::READ_WRITE_TRUNC))
	{
		return false;
	}

	// Make sure no fail bits are set. 
	if (mFile.good() || mFile.eof())
	{
		// Returns first value from the buffer prior to the delimiter
		char* nextToken = NULL;
		char* token = strtok_s(buffer, &dCSVFileInfo.delimiter, &nextToken);

		// While there are more values in the string, attempt to parse at the delimiter
		while (token != NULL)
		{
			values.push_back(token);
			token = strtok_s(NULL, &dCSVFileInfo.delimiter, &nextToken);
		}

		// Return the number of values found
		return (int)values.size();
	}
	else
	{
		CatchFailReason();
	}
	
	// Default return
	return 0;
}

bool CSV_Utility::ParseAnyCSVFile(const std::string filename, std::vector<std::vector<std::string>>& values)
{
	// Open the file.
	std::fstream file;
	file.open(filename, UTILITY_MODE::READ);
	if (file.is_open())
	{
		// grab the data from the file and push into a 2D vector of strings.
		std::string temp;
		while (std::getline(file, temp))
		{
			std::vector<std::string> data;
			char* nextToken = NULL;
			char* token = strtok_s(const_cast<char*>(temp.c_str()), &dCSVFileInfo.delimiter, &nextToken);

			// While there are more values in the string, attempt to parse at the delimiter
			while (token != NULL)
			{
				data.push_back(token);
				token = strtok_s(NULL, &dCSVFileInfo.delimiter, &nextToken);
			}

			values.push_back(data);
		}

		// Close and return
		file.close();
		return true;
	}

	// Default return
	return false;
}

void CSV_Utility::PrintCSVData()
{
	// Make sure file is open and we are in a read mode
	if (!mFile.is_open() || (mMode != UTILITY_MODE::READ && mMode != UTILITY_MODE::READ_WRITE_APPEND && mMode != UTILITY_MODE::READ_WRITE_TRUNC))
	{
		return;
	}

	// Make sure no fail bits are set. 
	if (mFile.good() || mFile.eof())
	{
		// Save current position
		auto curr_pos = mFile.tellg();

		// Seek to the top and print the entire file. 
		mFile.seekg(0, std::ios::beg);
		std::string line;
		while (std::getline(mFile, line))
		{
			printf("%s\n", line.c_str());
		}

		// Clear the state and return to position
		mFile.clear();
		mFile.seekp(curr_pos);
		return;
	}
	else
	{
		CatchFailReason();
	}
}

bool CSV_Utility::PrintAnyCSVFile(const std::string filename)
{
	// 2D vector to hold the file contents. 
	std::vector<std::vector<std::string>> data;

	// Open the file and parse, print if successful
	if(ParseAnyCSVFile(filename, data))
	{
		if (data.size() > 0)
		{
			// Print column headers
			for (int i = 0; i < data[0].size(); i++)
			{
				printf("\tCol %d", i + 1);
			}
			printf("\n");

			// Print vector data.
			for (int i = 0; i < data.size(); i++)
			{
				printf("Row %d:\t", i + 1);
				for (int j = 0; j < data[i].size(); j++)
				{
					std::string it = data[i][j];
					printf("%s \t", it.c_str());
				}
				printf("\n");
			}
		}

		// Return true
		return true;
	}

	// Default return
	return false;
}

bool CSV_Utility::IsEndOfFile()
{
	return mFile.eof();
}

bool CSV_Utility::GetFileInfo(CSVFileInfo& info)
{
	// Update info
	UpdateFileInfo();

	// If file info is valid, set the data
	if (dCSVFileInfo.Valid())
	{
		info = dCSVFileInfo;

		return true;
	}

	// Default return
	return false;
}

bool CSV_Utility::GetFileInfo(std::string& filename, double size, int columns, int rows)
{
	// Update info
	UpdateFileInfo();

	// If file info is valid, set the data
	if (dCSVFileInfo.Valid())
	{
		filename = dCSVFileInfo.filename;
		size = (double)dCSVFileInfo.filesize;
		columns = dCSVFileInfo.n_cols;
		rows = dCSVFileInfo.n_rows;

		return true;
	}

	// Default return
	return false;
}

bool CSV_Utility::ClearFile()
{
	// Close the file if open.
	if (mFile.is_open())
	{
		mFile.close();
	}

	// While the filename isnt empty
	if (!dCSVFileInfo.filename.empty())
	{
		// Open the file as output and truncating to clear all content - then re-close.
		mFile.open(dCSVFileInfo.filename, UTILITY_MODE::WRITE_TRUNC);
		mFile.close();

		return true;
	}

	// Default return - Filename empty if here. 
	return false;
}

size_t CSV_Utility::GetFileSize()
{
	// Check if file is not open. 
	if (!mFile.is_open())
	{
		return -1;
	}

	if (mFile.good() || mFile.eof())
	{
		// Save current position and then go to top of file. 
		auto curr_pos = mFile.tellg();

		// Seek to end and get the size
		long fsize = 0;
		mFile.seekg(0, std::ios::end);
		fsize = (long)mFile.tellg();

		// Return to position and return the size
		mFile.seekp(curr_pos);
		return fsize;
	}
	else
	{
		CatchFailReason();
	}

	// Default return
	return -1;
}

bool CSV_Utility::OpenFile()
{
	// Check if filename has been set.
	if (dCSVFileInfo.filename.empty())
	{
		return false;
	}

	// Get the Logger instance if its included
#ifdef CPP_LOGGER
	Log* log = log->GetInstance();
#endif

	// Verify outputFile is a valid file path / file name
	size_t i = dCSVFileInfo.filename.rfind('/', dCSVFileInfo.filename.length());
	if (i == std::string::npos)
	{
#ifdef CPP_LOGGER
		log->AddEntry(LOG_LEVEL::LOG_ERROR, mUser, "Log path is not a valid path.");
#else
		printf_s("%s - Log path is not a valid path.\n", mUser.c_str());
#endif
		return false;
	}
	std::string directoryPath = dCSVFileInfo.filename.substr(0, i);

	// Make the directory if it doesn't exist
	struct stat st = { 0 };
	if (stat(directoryPath.c_str(), &st) == -1)
	{
		int made = 0;
#ifdef _WIN32
		made = _mkdir(directoryPath.c_str());
#else
		made = mkdir(directoryPath.c_str(), 0777);
#endif
		if (made == -1)
		{
			char buffer[256];
			strerror_s(buffer, sizeof(buffer), errno); // get string message from errno, XSI-compliant version
#ifdef CPP_LOGGER
			log->AddEntry(LOG_LEVEL::LOG_ERROR, mUser, "Error %s", buffer);
#else
			printf_s("%s - Error %s\n", mUser.c_str(), buffer);
#endif
			return false;
		}
	}

	// Check if outputFile has an extension, if not, add default extension.
	if (!std::filesystem::path(dCSVFileInfo.filename).has_extension())
	{
		dCSVFileInfo.filename += mExtension;
#ifdef CPP_LOGGER
		log->AddEntry(LOG_LEVEL::LOG_INFO, mUser, "OpenFile - filename has no extension, adding default extension: %s", mExtension.c_str());
#else
		printf_s("%s - OpenFile - filename has no extension, adding default extension: %s.\n", mUser.c_str(), mExtension.c_str());
#endif
	}

	// open
	mFile.open(dCSVFileInfo.filename, mMode);
	if (!mFile.is_open())
	{
#ifdef CPP_LOGGER
		log->AddEntry(LOG_LEVEL::LOG_ERROR, mUser, "Failed to open the output file.");
#else
		printf_s("%s - Failed to open the output file.\n", mUser.c_str());
#endif
		return false;
	}

	if (mFile.good())
	{
		// Get file data. 
		UpdateFileInfo();

		// Success
#ifdef CPP_LOGGER
		log->AddEntry(LOG_LEVEL::LOG_INFO, mUser, "File open successful: %s", dCSVFileInfo.filename.c_str());
#else
		printf_s("%s - File open successful: %s\n", mUser.c_str(), dCSVFileInfo.filename.c_str());
#endif
		return true;
	}
	else
	{
		CatchFailReason();
	}
	return false;
}

bool CSV_Utility::IsFileOpen()
{
	return mFile.is_open();
}

bool CSV_Utility::CloseFile()
{
	// Check if the file is open.
	if (mFile.is_open())
	{
		// Close the file, clear the filename and reset the file flag
		mFile.close();
		dCSVFileInfo.filename = "";

		// Verify file is closed and return appropriately. 
		if (mFile.is_open())
		{
			return false;
		}
		return true;
	}

	// Default retun
	return false;
}

void CSV_Utility::CatchFailReason()
{
	if (mFile.eof()) { printf("Eof bit set.\n"); }
	else if (mFile.bad()) { printf("Bad bit set.\n"); }
	else if (mFile.fail()) { printf("Fail bit set.\n"); }
	else { printf("Unknown failure.\n"); }
}

void CSV_Utility::UpdateFileInfo()
{
	if (!dCSVFileInfo.filename.empty())
	{
		dCSVFileInfo.col_names.clear();
		GetColumnHeaders(dCSVFileInfo.col_names);
		dCSVFileInfo.n_cols = GetNumberOfColumns();
		dCSVFileInfo.n_rows = GetNumberOfRows();
		dCSVFileInfo.filesize = GetFileSize();
	}
}