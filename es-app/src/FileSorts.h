#pragma once
#ifndef ES_APP_FILE_SORTS_H
#define ES_APP_FILE_SORTS_H

#include <vector>
#include "FileData.h"

namespace FileSorts
{
	bool compareFileName(const FileData* file1, const FileData* file2);
	bool compareRating(const FileData* file1, const FileData* file2);
	bool compareTimesPlayed(const FileData* file1, const FileData* fil2);
	bool compareLastPlayed(const FileData* file1, const FileData* file2);

	extern const std::vector<FileData::SortType> SortTypes;
};

#endif // ES_APP_FILE_SORTS_H
