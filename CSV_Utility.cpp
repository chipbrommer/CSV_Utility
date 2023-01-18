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
	mDelimiter = ',';
	mExtension = ".csv";
	mFilename = "";
	mFileSet = false;
}

CSV_Utility::CSV_Utility(const std::string filename, const UTILITY_MODE mode = UTILITY_MODE::READWRITE, const UTILITY_WRITE_TYPE type = UTILITY_WRITE_TYPE::TRUNC)
{
	mFilename = filename;
	mFileSet = true;
	mUser = "CSVUtility";
	mDelimiter = ',';
	mExtension = ".csv";
	mMode = mode;
	mType = type;
}

CSV_Utility::~CSV_Utility()
{
	mFile.close();
}

int CSV_Utility::SetFileName(const std::string filename)
{
	// already open. 
	if (mFile.is_open())
	{
		return -1;
	}

	// Set name. 
	mFilename = filename;

	// if filenames are equal, success. 
	if (mFilename == filename)
	{
		mFileSet = true;
		return 1;
	}
	// failed. 
	return 0;
}

bool CSV_Utility::ChangeDelimiter(const char delimiter)
{
#ifdef CPP_LOGGER
	log->AddEntry(LOG_LEVEL::LOG_ERROR, mUser, "Received new delimiter: %c", delimiter);
#else
	printf_s("%s - Received new delimiter: %c\n", mUser.c_str(), delimiter);
#endif

	mDelimiter = delimiter;

	if (mDelimiter == delimiter)
	{
		return true;
	}

	return false;
}

bool CSV_Utility::ChangeCSVUtilityMode(const UTILITY_MODE mode)
{
	bool open = false;
	// if the file is open, close it. 
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
		// if file was open before change, attempt to re-open. 
		if (open)
		{
			// if file is open is successful, return true, else return false.
			if (OpenFile())
			{
				return true;
			}
			return false;
		}
	}
	return false;
}

bool CSV_Utility::ChangeCSVUtilityWritingType(const UTILITY_WRITE_TYPE type)
{
	bool open = false;
	// if the file is open, close it
	if (mFile.is_open())
	{
		open = true;
		mFile.close();
	}

	// Set the new type
	mType = type;

	// If modes equal, attempt to re-open file. 
	if (mType == type)
	{
		// if file was open before change, attempt to re-open. 
		if (open)
		{
			// if file is open is successful, return true, else return false.
			if (OpenFile())
			{
				return true;
			}
			return false;
		}
	}
	return false;
}

int CSV_Utility::WriteColumnHeaders(const std::vector<std::string>& names)
{
	if (!mFile.is_open() || (mMode != UTILITY_MODE::WRITE && mMode != UTILITY_MODE::READWRITE))
	{
		return -1;
	}

	mFile.seekg(0, std::ios::beg);

	/*
	if not trunc mode, we need to change the mode.
	*/

	int count = WriteRow(names);

	return count;
}

bool CSV_Utility::ReadRow(std::string& values, const int row = 0)
{
	if (!mFile.is_open() || (mMode != UTILITY_MODE::READ && mMode != UTILITY_MODE::READWRITE))
	{
		return false;
	}

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

		// Return to position
		mFile.seekp(curr_pos);

		return true;
	}
	else
	{
		CatchFailReason();
	}

	return false;
}

bool ReadColumn(std::vector<std::string>values, const int column)
{
	return false;
}

int CSV_Utility::GetColumnNames(std::vector<std::string>& names)
{
	if (!mFile.is_open() || (mMode != UTILITY_MODE::READ && mMode != UTILITY_MODE::READWRITE))
	{
		return false;
	}

	if (mFile.good() || mFile.eof())
	{
		std::string row;
		int count = 0;
		bool r = ReadRow(row, 1);
		if (r)
		{
			count = ParseCSVBuffer((char*)row.c_str(), names);
		}

		return count;
	}
	else
	{
		CatchFailReason();
	}

	return 0;
}

int CSV_Utility::GetNumberOfColumns()
{
	// Make sure file is open and we are in a read mode
	if (!mFile.is_open() || (mMode != UTILITY_MODE::READ && mMode != UTILITY_MODE::READWRITE))
	{
		return -1;
	}

	// Veify file handle is good. 
	if (mFile.good() || mFile.eof())
	{
		// Get the number of columns in row 1.
		std::string row;
		int count = 0;
		bool r = ReadRow(row, 1);
		if (r)
		{
			std::vector < std::string> values;
			count = ParseCSVBuffer((char*)row.c_str(), values);
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
	if (!IsFileOpen() || (mMode != UTILITY_MODE::READ && mMode != UTILITY_MODE::READWRITE))
	{
		return -1;
	}

	if (mFile.good() || mFile.eof())
	{
		int count = 0;
		std::string line = "";

		// Get current position - then move to top of file. 
		auto curr_pos = mFile.tellg();
		mFile.seekg(0, std::ios::beg);

		// While loop to count numbers of rows. 
		while (getline(mFile, line))
		{
			count++;
		}

		// Return to position
		mFile.seekp(curr_pos);

		return count;
	}

	return -1;
}

int CSV_Utility::WriteFullCSV(const std::string filename, std::vector<int> const& values)
{
	// Get the Logger instance if its included
#ifdef CPP_LOGGER
	Log* log = log->GetInstance();
#endif

	// Create a local copy of the filename.
	std::string outputFile = filename;

	// Verify outputFile is a valid file path / file name
	size_t i = outputFile.rfind('/', outputFile.length());
	if (i == std::string::npos)
	{
#ifdef CPP_LOGGER
		log->AddEntry(LOG_LEVEL::LOG_ERROR, mUser, "Log path is not a valid path.");
#else
		printf_s("%s - Log path is not a valid path.\n", mUser.c_str());
#endif
		return -1;
	}
	std::string directoryPath = outputFile.substr(0, i);

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
		}
	}

	// Check if outputFile has an extension =, if not, add default extension.
	if (!std::filesystem::path(outputFile).has_extension())
	{
		outputFile += mExtension;
#ifdef CPP_LOGGER
		log->AddEntry(LOG_LEVEL::LOG_INFO, mUser, "WriteCSV received a file name with no extension, adding default extension: %s", mExtension.c_str());
#else
		printf_s("%s - WriteCSV received a file name with no extension, adding default extension: %s.\n", mUser.c_str(), mExtension.c_str());
#endif
	}

	// Create file and catch a bad handle
	FILE* handle;
	int open = fopen_s(&handle, outputFile.c_str(), "w");

	if (open != 0)
	{
#ifdef CPP_LOGGER
		log->AddEntry(LOG_LEVEL::LOG_ERROR, mUser, "Failed to open the output file.");
#else
		printf_s("%s - Failed to open the output file.\n", mUser.c_str());
#endif
		return -1;
	}
	else
	{
		for (std::vector<int>::const_iterator it = values.begin(); it != values.end(); ++it)
		{
			fprintf_s(handle, "%d%c", *it, mDelimiter);
		}

		printf("Size: %zd\n", values.size());
	}

#ifdef CPP_LOGGER
	log->AddEntry(LOG_LEVEL::LOG_INFO, mUser, "File write successful to %s", outputFile.c_str());
#else
	printf_s("%s - File write successful to %s\n", mUser.c_str(), outputFile.c_str());
#endif

	return 1;
}

int CSV_Utility::ParseCSVBuffer(std::string buffer, std::vector<std::string>& values)
{
	if (!mFile.is_open() || (mMode != UTILITY_MODE::READ && mMode != UTILITY_MODE::READWRITE))
	{
		return false;
	}

	if (mFile.good() || mFile.eof())
	{
		// Returns first value
		char* token = strtok((char*)buffer.c_str(), (char*)mDelimiter);

		while (token != NULL)
		{
			values.push_back(token);
			token = strtok(NULL, (char*)mDelimiter);
		}

		return values.size();
	}
	else
	{
		CatchFailReason();
	}

	return 0;
}

int CSV_Utility::ParseCSVFile(FILE* handle, std::vector<int>& values)
{
	return 0;
}

void CSV_Utility::PrintCSVData()
{
	if (!mFile.is_open())
	{
		return;
	}

	if (mFile.good() || mFile.eof())
	{
		// Save current position and then go to top of file. 
		auto curr_pos = mFile.tellg();

		// Seek to the top and print the entire file. 
		mFile.seekg(0, std::ios::beg);
		std::string line;
		while (std::getline(mFile, line))
		{
			printf("%s\n", line.c_str());
		}

		// Return to position
		mFile.seekp(curr_pos);
	}
	else
	{
		CatchFailReason();
	}
}

bool CSV_Utility::IsEndOfFile()
{
	return mFile.eof();
}

bool CSV_Utility::ClearFile()
{
	if (mFile.is_open())
	{
		mFile.close();
	}

	if (mFilename != "")
	{
		// Open the file as output and truncating to clear all content - then re-close.
		mFile.open(mFilename, std::ios::out | std::ios::trunc);
		mFile.close();

		return true;
	}

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

		// Return to position
		mFile.seekp(curr_pos);

		return fsize;
	}
	else
	{
		CatchFailReason();
	}

	return -1;
}

bool CSV_Utility::OpenFile()
{
	// Check if filename has been set.
	if (!mFileSet)
	{
		return false;
	}

	// Get the Logger instance if its included
#ifdef CPP_LOGGER
	Log* log = log->GetInstance();
#endif

	// Verify outputFile is a valid file path / file name
	size_t i = mFilename.rfind('/', mFilename.length());
	if (i == std::string::npos)
	{
#ifdef CPP_LOGGER
		log->AddEntry(LOG_LEVEL::LOG_ERROR, mUser, "Log path is not a valid path.");
#else
		printf_s("%s - Log path is not a valid path.\n", mUser.c_str());
#endif
		return false;
	}
	std::string directoryPath = mFilename.substr(0, i);

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
	if (!std::filesystem::path(mFilename).has_extension())
	{
		mFilename += mExtension;
#ifdef CPP_LOGGER
		log->AddEntry(LOG_LEVEL::LOG_INFO, mUser, "OpenFile - filename has no extension, adding default extension: %s", mExtension.c_str());
#else
		printf_s("%s - OpenFile - filename has no extension, adding default extension: %s.\n", mUser.c_str(), mExtension.c_str());
#endif
	}

	// Create the file and verify its open.
	mFile.open(mFilename, std::ios::out | std::ios::in | std::ios::trunc);
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
		// Success
#ifdef CPP_LOGGER
		log->AddEntry(LOG_LEVEL::LOG_INFO, mUser, "File open successful: %s", mFilename.c_str());
#else
		printf_s("%s - File open successful: %s\n", mUser.c_str(), mFilename.c_str());
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
	if (mFile.is_open())
	{
		mFile.close();
		mFilename = "";
		mFileSet = false;

		if (mFile.is_open())
		{
			return false;
		}
		return true;
	}

	return false;
}

void CSV_Utility::CatchFailReason()
{
	if (mFile.eof()) { printf("Eof bit set.\n"); }
	else if (mFile.bad()) { printf("Bad bit set.\n"); }
	else if (mFile.fail()) { printf("Fail bit set.\n"); }
	else { printf("Unknown failure.\n"); }
}