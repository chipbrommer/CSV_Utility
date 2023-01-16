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
	mDelimeter = ',';
	mExtension = ".csv";
}

CSV_Utility::~CSV_Utility()
{
}

void CSV_Utility::ChangeDelimiter(char delimiter)
{
	printf_s("Received new delimiter: %c\n", delimiter);
	mDelimeter = delimiter;
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
			fprintf_s(handle, "%d%c", *it, mDelimeter);
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

template<typename T>
int CSV_Utility::ParseCSVBuffer(char* buffer, std::vector<T>& values)
{
	return 0;
}

template <typename T>
int CSV_Utility::ParseCSVFile(FILE* handle, std::vector<T>& values)
{

}