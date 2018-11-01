#pragma once

#include "FileData.h"
#include "SystemData.h"

struct SearchParams
{
	SystemData* system;
	FileData* game;

	std::string nameOverride;
};
