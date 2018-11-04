#include <string.h>
#include "MameNames.h"

extern const char* mameNameToRealName[];

namespace MameNames
{

  int getMameTitleCount()
	{
		const char** mameNames = mameNameToRealName;
		int count = 0;
		while (*mameNames != NULL)
		{
			mameNames += 2;
			count++;
		}
		return count;
	}


	const char* getRealName(const char* from)
	{
		// The start and end index range from [0, number of roms]
		int iStart = 0;
		static int mameCount = getMameTitleCount();
		int iEnd = mameCount;

		while (iStart < iEnd)
		{
			// The middle entry is halfway between the start and end index
			const int iMiddle = (iStart + iEnd) / 2;

			// mameNameToRealName contains 2 sequential entries for every entry, so the indexes look like this:
			// 0: key, value,
			// 2: key, value,
			// 4: key, value
			// This means that there are twice as many indexes as there are numbers of ROMs. So to get the
			// iMiddle'th entry, we need to multiply by 2 because iMiddle goes from [0, number of roms].
			const int iKey = iMiddle * 2;
			const int comp = strcmp(mameNameToRealName[iKey], from);
			if (comp < 0)
			{
				// Remember, iMiddle ranges from [0, number of roms] so we only increment by 1
				iStart = iMiddle + 1;
			}
			else if (comp > 0)
			{
				iEnd = iMiddle;
			}
			// The Key was found, now return the Value
			else return mameNameToRealName[iKey + 1];
		}
		return from;
	}

}
