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
	mUser = "CSVUtility";
	mDelimiter = ',';
	mExtension = ".csv";
}

CSV_Utility::CSV_Utility(const std::string filename, const UTILITY_MODE mode = UTILITY_MODE::READWRITE, const UTILITY_WRITE_TYPE type = UTILITY_WRITE_TYPE::TRUNC)
{
	mFilename = filename;
	mUser = "CSVUtility";
	mDelimiter = ',';
	mExtension = ".csv";
	mMode = mode;
	mType = type;
}

CSV_Utility::~CSV_Utility()
{
	CloseFile();
}

int CSV_Utility::SetFileName(const std::string filename)
{
	if (IsFileOpen())
	{
		return -1;
	}

	mFilename = filename;

	if (mFilename == filename)
	{
		return 1;
	}
	
	return 0;
}

bool CSV_Utility::ChangeDelimiter(char delimiter)
{
	printf_s("Received new delimiter: %c\n", delimiter);
	mDelimiter = delimiter;

	mDelimiter = delimiter;

	if (mDelimiter == delimiter)
	{
		return true;
	}

	return false;
}

bool CSV_Utility::ChangeCSVUtilityMode(UTILITY_MODE mode, bool close = false)
{
	if (mFile.is_open())
	{
		if (close)
		{
			CloseFile();
		}
		else
		{
			return false;
		}
	}

	mMode = mode;

	if(mMode == mode)
	{
		return true;
	}

	return false;
}

bool CSV_Utility::ChangeCSVUtilityWritingType(UTILITY_WRITE_TYPE type, bool close = false)
{
	if (mFile.is_open())
	{
		if (close)
		{
			CloseFile();
		}
		else
		{
			return false;
		}
	}

	mType = type;

	if (mType == type)
	{
		return true;
	}

	return false;
}

int CSV_Utility::WriteColumnHeaders(const std::vector<std::string>& names)
{
	if (!mFile.is_open())
	{
		return -1;
	}

	// TODO - Get current position

	mFile.seekg(0, std::ios::beg);

	/*
	if not append mode, we need to change the mode.
	*/

	int count = WriteRow(names);

	// TODO return to position

	return count;
}

int CSV_Utility::WriteRow(const std::vector<int>& values)
{
	if (!IsFileOpen())
	{
		return -1;
	}

	int count = 0;

	for (std::vector<int>::const_iterator it = values.begin(); it != values.end(); ++it)
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

int CSV_Utility::WriteRow(const std::vector<std::string>& values)
{
	if (!IsFileOpen())
	{
		return -1;
	}

	int count = 0;

	for (std::vector<std::string>::const_iterator it = values.begin(); it != values.end(); ++it)
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

bool CSV_Utility::ReadRow(std::string& values, int rowNum = 0)
{
	if (!IsFileOpen())
	{
		return false;
	}

	// TODO - Capture current file location.

	// Go to top of file. 
	//mFile.seekg(0, std::ios::beg);

	// TODO - for loop to get to the desired row number. 

	if (mFile.good())
	{
		std::getline(mFile, values);
	}
	else
	{
		CatchFailReason();
	}

	// TODO - return to captured file location.

	return true;
}

void CSV_Utility::PrintFile()
{
	if (!mFile.is_open())
	{
		return;
	}

	mFile.seekg(0, std::ios::beg);

	if (mFile.good())
	{
		mFile.seekg(0, std::ios::beg);
		std::string line;

		while (std::getline(mFile, line))
		{
			printf("%s\n", line.c_str());
		}
	}
	else
	{
		CatchFailReason();
	}

	printf("\n");
}

int CSV_Utility::GetColumnNames(const std::vector<std::string>* names)
{
	if (!IsFileOpen())
	{
		return -1;
	}

	// store - Get current location

	// Seek to top of file

	// Get first line

	// Parse at delimiter

	// Seek back to pre-location.

	return 0;
}

int CSV_Utility::GetNumberOfColumns()
{
	if (!IsFileOpen())
	{
		return -1;
	}

	return 0;
}

int CSV_Utility::GetNumberOfRows()
{
	if (!IsFileOpen() || (mMode != UTILITY_MODE::READ && mMode != UTILITY_MODE::READWRITE))
	{
		return -1;
	}

	int count = 0;
	std::string line = "";

	// TODO Get current location

	// Move to top of file
	mFile.seekg(0, std::ios::beg);

	// While loop to count numbers of rows. 
	while (getline(mFile, line))
	{
		count++;
	}

	// TODO return to location

	return count;
}

//template<class T>
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

//template<typename T>
int CSV_Utility::ParseCSVBuffer(char* buffer, const std::vector<std::string>& values)
{
	return 0;
}

template <typename T>
int CSV_Utility::ParseCSVFile(FILE* handle, std::vector<T>& values)
{
	return 0;
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

	mFile.open(mFilename, std::ios::out | std::ios::trunc);

	mFile.close();

	return true;
}

size_t CSV_Utility::GetFileSize()
{
	// Check if file is not open. 
	if (!IsFileOpen())
	{
		return -1;
	}

	// TODO get current position.

	long fsize = 0;
	mFile.seekg(0, std::ios::end);
	fsize = mFile.tellg();
	mFile.seekg(0, std::ios::beg);

	// TODO put at original position

	return fsize;
}

bool CSV_Utility::OpenFile()
{
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
	mFile.open(mFilename, std::ios::in);
	if (!mFile.is_open())
	{
#ifdef CPP_LOGGER
		log->AddEntry(LOG_LEVEL::LOG_ERROR, mUser, "Failed to open the output file.");
#else
		printf_s("%s - Failed to open the output file.\n", mUser.c_str());
#endif
		return false;
	}

	if (mFile.good()) { printf("good"); }
	if (mFile.eof()) { printf("eof"); }
	if (mFile.bad()) { printf("bad"); }
	if (mFile.fail()) { printf("fail"); }

	// Success
#ifdef CPP_LOGGER
	log->AddEntry(LOG_LEVEL::LOG_INFO, mUser, "File open successful: %s", mFilename.c_str());
#else
	printf_s("%s - File open successful: %s\n", mUser.c_str(), mFilename.c_str());
#endif

	return true;
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