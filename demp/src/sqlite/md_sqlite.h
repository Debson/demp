#pragma once
#ifndef MD_SQLITE_H
#define MD_SQLITE_H

#include "../md_types.h"
#include "../audio/mp_audio.h"

namespace mdEngine
{
	namespace Database
	{
		b8 OpenDB();
		b8 PushToDatabase(Audio::AudioProperties* item);
		b8 CloseDB();

		// temporary
		void GetItemsInfo();
	}
}

#endif // !MD_SQLITE_H
