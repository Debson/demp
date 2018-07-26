#pragma once
#ifndef MUSIC_PLAYER_SETTINGS_H
#define MUSIC_PLAYER_SETTINGS_H

#include "md_types.h"

/* Max size of song that can be loaded into ram IN BYTES */
#define _MAX_SIZE_RAM_LOADED 50000000

namespace mdEngine
{
namespace MP
{
namespace Settings
{

	const f32 VolumeKeyMultiplier = 0.8f;

	const f32 VolumeScrollStep = 2.f;

	const u16 VolumeFadeTime = 500;

}
}
}


#endif // !MUSIC_PLAYER_SETTINGS_H
