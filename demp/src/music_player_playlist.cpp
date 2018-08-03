#include "music_player_playlist.h"

#include "configuration.h"

#include <fstream>
#include <Windows.h>
#include <filesystem>

#include "md_time.h"
#include "music_player_ui.h"
#include "music_player.h"

#ifdef _WIN32_
#define OUTPUT std::wcout
#else
#define OUTPUT std::cout
#endif

namespace mdEngine
{
namespace MP
{
	namespace Playlist
	{
		PathContainer mdPathContainer;
		SongObject RamLoadedMusic;

		b8 mdNextRequest(false);
		b8 mdPreviousRequest(false);

		f32 mdVolume = 0.5f;

		f32 mdVolumeFadeInValue = 0;
		f32 mdVolumeFadeOutValue = 0;

		b8 mdMusicPaused(false);
		b8 mdVolumeMuted(false);

		b8 mdMPStarted(false);

		b8 mdVolumeFadeIn(false);
		b8 mdVolumeFadeOut(false);

		f32 mdVolumeScrollStep = 2.f;

		u16 mdVolumeFadeTime = 500;

		b8 mdRepeatMusic(false);
		b8 mdShuffleMusic(false);

		b8 mdShuffleMusicSet(false);

		std::vector<u32> mdShuffleMusicPosContainer;
		static s32 mdCurrentShuffleMusicPos = 0;


		void SetActualVolume();

		void RepeatMusicProcedure();

		void ShuffleMusicProcedure();

		void CheckVolumeBounds();

		void CheckMPState();

		b8 check_size(u32);

		b8 check_file();

	}
}

namespace MP
{
	namespace Playlist
	{

		b8 check_size(u32 size)
		{
			if (size > _MAX_SIZE_RAM_LOADED)
				return false;

			return true;
		}

		/* delete this */
		b8 check_file()
		{
			if (BASS_ErrorGetCode() == BASS_ERROR_FILEFORM)
			{
				std::cout << "BASS_ERROR: File of this format is not supported!\n";
				return false;
			}
			if (BASS_ErrorGetCode() == BASS_ERROR_FILEOPEN)
			{
				std::cout << "BASS_ERROR: File could not be opened!";
				return false;
			}

			return true;
		}

		SongObject::SongObject()
		{
			mMusic = NULL;
			mData = NULL;
#ifdef _WIN32_
			mPath = std::wstring();
#else
			mPath = NULL;
#endif
		}

		SongObject::~SongObject()
		{
			mMusic = NULL;
			mData = NULL;
#ifdef _WIN32_
			mPath = std::wstring();
#else
			mPath = NULL;
#endif
		}

#ifdef _WIN32_
		b8 SongObject::init(std::wstring songPath)
		{
			//std::wifstream file(songPath, std::ios::binary | std::ios::ate);
			FILE* file = NULL;
			if (_wfopen_s(&file, songPath.c_str(), L"rb+") != 0)
			{
				OUTPUT << "ERROR: could not open file at path \"" << songPath << "\"\n";
				std::cout << stderr << std::endl;;
				return false;
			}
#else
		b8 SongObject::init(const char* songPath)
		{
			FILE* file = fopen(songPath, "rb+");
			if (file == NULL)
			{
				std::cout << "ERROR: could not open file at path \"" << songPath << "\"\n";
				std::cout << stderr << std::endl;;
				return false;
			}

#endif
			else
			{
				fseek(file, 0L, SEEK_END);
				size_t size = ftell(file); // size of opened file
				fseek(file, 0L, SEEK_SET); // set cursor at the beginning

				mPath = songPath;
				mID = 0;
				mSize = size;


				delete[] mData;
				BASS_StreamFree(mMusic);
				mData = NULL;
				mMusic = NULL;


				if (check_size(mSize) == true)
				{
					mData = new char[size];
					size_t newLen = fread(mData, sizeof(char), mSize, file);
					if (ferror(file) == 0)
					{
						fclose(file);
						Playlist::mdPathContainer.push_back(songPath);
						UI::PlaylistItem * item = new UI::PlaylistItem();
						item->InitFont();
						item->InitItem();


						mMusic = BASS_StreamCreateFile(TRUE, mData, 0, size, BASS_STREAM_AUTOFREE);
						if (check_file() == false)
						{
							return false;
						}
					}
					else
					{
						fclose(file);;
						OUTPUT << "ERROR: could not read file at path \"" << songPath << "\"\n";
						std::cout << stderr << std::endl;;
						return false;
					}
				}
				else
				{
					delete[] mData;
					mData = NULL;
#ifdef _WIN32_
					mMusic = BASS_StreamCreateFile(FALSE, mPath.c_str(), 0, 0, 0);
#else
					mMusic = BASS_StreamCreateFile(FALSE, mPath, 0, 0, 0);
#endif

					if (check_file() == false)
					{
						return false;
					}
				}
			}

			return true;

		}
#ifdef _WIN32_


		b8 SongObject::load(std::wstring songPath, u32 id, MP::Playlist::SongState state)
		{
			FILE* file = NULL;
			if (_wfopen_s(&file, songPath.c_str(), L"rb+") != 0)
			{
				OUTPUT << "ERROR: could not open file at path \"" << songPath << "\"\n";
				std::cout << stderr;
				return false;
			}
#else
		b8 SongObject::load(const char* songPath, u32 id, MP::Playlist::SongState state)
		{
			FILE* file = fopen(songPath, "rb+");
			if (file == NULL)
			{
				std::cout << "ERROR: could not open file at path \"" << songPath << "\"\n";
				std::cerr << stderr << std::endl;
				return false;
			}

#endif
			else
			{
				fseek(file, 0L, SEEK_END);
				size_t size = ftell(file); // size of opened file
				fseek(file, 0L, SEEK_SET); // set cursor at the beginning

				mPath = songPath;
				mID = id;
				mSize = size;

				delete[] mData;
				BASS_StreamFree(mMusic);
				mData = NULL;
				mMusic = NULL;

				if (check_size(mSize) == true)
				{
					mData = new char[size];
					size_t newLen = fread(mData, sizeof(char), mSize, file);
					if (ferror(file) == 0)
					{
						fclose(file);
						

						mMusic = BASS_StreamCreateFile(TRUE, mData, 0, size, BASS_STREAM_AUTOFREE);
						if (check_file() == false)
						{
							//delete mdPathContainer[mID];
							mdPathContainer.erase(mdPathContainer.begin() + mID);

							if (state == MP::Playlist::SongState::mNext)
								return load(mdPathContainer[mID - 1], mID - 1, MP::Playlist::SongState::mNext);

							return load(mdPathContainer[mID + 1], mID + 1, MP::Playlist::SongState::mPrevious);
						}
					}
					else
					{
						fclose(file);;
						OUTPUT << "ERROR: could not read file at path \"" << songPath << "\"\n";
						std::cout << stderr << std::endl;
						return false;
					}
				}
				else
				{
					delete[] mData;
					mData = NULL;
#ifdef _WIN32_
					mMusic = BASS_StreamCreateFile(FALSE, mPath.c_str(), 0, 0, 0);
#else
					mMusic = BASS_StreamCreateFile(FALSE, mPath, 0, 0, 0);
#endif

					if (check_file() == false)
					{
						//delete mdPathContainer[mID];
						mdPathContainer.erase(mdPathContainer.begin() + mID);

						if (state == MP::Playlist::SongState::mNext)
							return load(mdPathContainer[mID - 1], mID - 1, MP::Playlist::SongState::mNext);

						return load(mdPathContainer[mID + 1], mID + 1, MP::Playlist::SongState::mPrevious);;
					}
				}
			}

			return true;
		}


		HMUSIC& SongObject::get()
		{
			return mMusic;
		}

		void Start()
		{

		}

		void UpdatePlaylist()
		{

			if (mdNextRequest)
			{
				PauseMusic();

				PlayMusic();

				mdNextRequest = false;
			}

			if (mdPreviousRequest)
			{
				PauseMusic();

				PlayMusic();

				mdPreviousRequest = false;
			}
		}

		void UpdateMusic()
		{
			float vol = 0;

			if (mdVolumeFadeOut)
			{
				BASS_ChannelGetAttribute(RamLoadedMusic.get(), BASS_ATTRIB_VOL, &vol);
				if (vol <= 0)
				{
					BASS_ChannelPause(RamLoadedMusic.get());
				}
			}
			else if (mdVolumeFadeIn)
			{
				BASS_ChannelGetAttribute(RamLoadedMusic.get(), BASS_ATTRIB_VOL, &vol);
				if (vol >= mdVolume)
				{
					mdVolumeFadeIn = false;
				}
			}
			else
			{
				SetActualVolume();
			}

			CheckMPState();

			CheckVolumeBounds();

			RepeatMusicProcedure();

		}

		void PlayMusic()
		{
			/* Reset booleans that control music state */
			mdVolumeFadeIn = false;
			mdVolumeFadeOut = false;
			mdMusicPaused = false;

			if (RamLoadedMusic.get() != NULL)
			{
				if(IsPlaying() == false)
					RamLoadedMusic.load(mdPathContainer[RamLoadedMusic.mID], RamLoadedMusic.mID, SongState::mCurrent);
				mdMPStarted = true;
			}
			BASS_ChannelPlay(RamLoadedMusic.get(), true);
		}

		void StopMusic()
		{
			if (RamLoadedMusic.get() != NULL)
			{
				BASS_ChannelPause(RamLoadedMusic.get());
				BASS_ChannelSetPosition(RamLoadedMusic.get(), 0, BASS_POS_BYTE);
				mdMPStarted = false;
			}
		}

		void PauseMusic()
		{

			if (BASS_ChannelIsActive(RamLoadedMusic.get()) != BASS_ACTIVE_STOPPED && RamLoadedMusic.get() != NULL)
			{
				mdMusicPaused == false ? (mdMusicPaused = true) : (mdMusicPaused = false);

				if (mdMusicPaused)
				{
					BASS_ChannelSlideAttribute(RamLoadedMusic.get(), BASS_ATTRIB_VOL, 0, mdVolumeFadeTime);
					mdVolumeFadeOut = true;
				}
				else
				{
					mdVolumeFadeOut = false;
					BASS_ChannelPlay(RamLoadedMusic.get(), false);
					BASS_ChannelSlideAttribute(RamLoadedMusic.get(), BASS_ATTRIB_VOL, mdVolume, mdVolumeFadeTime);
					mdVolumeFadeIn = true;
				}
			}

		}

		void NextMusic()
		{
			if ((RamLoadedMusic.get() != NULL) || (BASS_ErrorGetCode() == BASS_ERROR_FILEFORM))
			{
				BASS_ChannelStop(RamLoadedMusic.get());

				/* If shuffle is enabled, try to load next song basing on shuffled positions container.

				If song path exist on next position, load it to the ram,
				else load song from path at position 0 in path's container.

				While loop:
				While path in PathContainer at position (current_path_id + index) is not valid and
				path on next position exist, try to open file at that path position.

				*/
				if (mdShuffleMusic == false)
				{
					if ((RamLoadedMusic.mID + 1) < mdPathContainer.size())
					{
						s32 index = 1;
						while ((RamLoadedMusic.load(mdPathContainer[RamLoadedMusic.mID + index], RamLoadedMusic.mID + index, SongState::mNext) == false)
							&& (RamLoadedMusic.mID + 1 < mdPathContainer.size()))
						{
							index++;
						}
						//assert(RamLoadedSong.load(mdPathContainer[RamLoadedSong.mID + index], RamLoadedSong.mID + index));

						mdNextRequest = true;
					}
					else
					{
						RamLoadedMusic.load(mdPathContainer[0], 0, SongState::mCurrent);
						mdNextRequest = true;
					}
				}
				else
				{
					mdCurrentShuffleMusicPos++;
					if (mdCurrentShuffleMusicPos > mdShuffleMusicPosContainer.size() - 1)
						mdCurrentShuffleMusicPos = 0;
					while (RamLoadedMusic.load(mdPathContainer[mdShuffleMusicPosContainer[mdCurrentShuffleMusicPos]], 
											   mdShuffleMusicPosContainer[mdCurrentShuffleMusicPos], SongState::mNext) == false)
					{
						mdCurrentShuffleMusicPos++;
						if (mdCurrentShuffleMusicPos > mdShuffleMusicPosContainer.size() - 1)
							mdCurrentShuffleMusicPos = 0;
					}

					mdNextRequest = true;
					
				}
			}
		}

		void PreviousMusic()
		{
			if (RamLoadedMusic.get() != NULL)
			{
				BASS_ChannelStop(RamLoadedMusic.get());

				/*	If shuffle is enabled, try to load next song basing on shuffled positions container.
				
				If song path exist on previous position, load it to the ram,
				else load song from path at position `PathContainer.Size() - 1`

				While loop:
				While path in PathContainer at position (current_path_id - index) is not valid and
				path on previous position exist, try to open file at that path position.

				*/
				if (mdShuffleMusic == false)
				{
					if ((RamLoadedMusic.mID - 1) >= 0)
					{
						int index = 1;
						while ((RamLoadedMusic.load(mdPathContainer[RamLoadedMusic.mID - index], RamLoadedMusic.mID - index, SongState::mPrevious) == false)
							&& (RamLoadedMusic.mID - index >= 0))
						{
							index++;
						}

						//assert(RamLoadedSong.load(mdPathContainer[RamLoadedSong.mID - index], RamLoadedSong.mID - index));
						mdPreviousRequest = true;
					}
					else
					{
						RamLoadedMusic.load(mdPathContainer[mdPathContainer.size() - 1], mdPathContainer.size() - 1, SongState::mCurrent);
						mdPreviousRequest = true;
					}
				}
				else
				{
					mdCurrentShuffleMusicPos--;
					if (mdCurrentShuffleMusicPos < 0)
						mdCurrentShuffleMusicPos = mdPathContainer.size() - 1;
					while (RamLoadedMusic.load(mdPathContainer[mdShuffleMusicPosContainer[mdCurrentShuffleMusicPos]], 
											   mdShuffleMusicPosContainer[mdCurrentShuffleMusicPos], SongState::mPrevious) == false)
					{
						mdCurrentShuffleMusicPos--;
						if (mdCurrentShuffleMusicPos < 0)
							mdCurrentShuffleMusicPos = mdPathContainer.size() - 1;
					}

					mdPreviousRequest = true;
					
				}
			}
		}


		void IncreaseVolume(App::InputEvent event)
		{
			switch (event)
			{
			case App::InputEvent::kPressedEvent:
				mdVolume += (Settings::VolumeKeyMultiplier * Time::deltaTime);
				break;
			case App::InputEvent::kScrollEvent:
				mdVolume += (mdVolumeScrollStep / 100.f);
				break;
			}
		}

		void LowerVolume(App::InputEvent event)
		{
			switch (event)
			{
			case App::InputEvent::kPressedEvent:
				mdVolume -= (Settings::VolumeKeyMultiplier * Time::deltaTime);
				break;
			case App::InputEvent::kScrollEvent:
				mdVolume -= (mdVolumeScrollStep / 100.f);
				break;
			};
		}


		void RewindMusic(s32 pos)
		{
			if (RamLoadedMusic.get() != NULL)
			{
				u64 currentPos = BASS_ChannelGetPosition(RamLoadedMusic.get(), BASS_POS_BYTE);
				u64 previousPos = currentPos;
				u64 bytes;
				if (pos < 0)
				{
					bytes = BASS_ChannelSeconds2Bytes(RamLoadedMusic.get(), -(pos));
					currentPos -= bytes;
					if (previousPos < currentPos)
						currentPos = 0;
				}
				else
				{
					u64 length = BASS_ChannelGetLength(RamLoadedMusic.get(), BASS_POS_BYTE);
					bytes = BASS_ChannelSeconds2Bytes(RamLoadedMusic.get(), pos);
					currentPos += bytes;
					if (previousPos >= length)
					{
						PlayMusic();
						currentPos = 0;
					}
				}

				BASS_ChannelSetPosition(RamLoadedMusic.get(), currentPos, BASS_POS_BYTE);
			}
		}

		void RepeatMusicProcedure()
		{
			if (mdRepeatMusic == true)
			{
				if (IsPlaying() == false)
				{
					PlayMusic();
				}
			}
		}

		void ShuffleMusicProcedure()
		{
			/* Fill vector with numbers from 0 to path container's size and then random shuffle 
			   all the numbers, creating a playlist with shuffeled songs.
			*/

			if (mdShuffleMusicPosContainer.size() > 0)
				mdShuffleMusicPosContainer.clear();

			mdShuffleMusicPosContainer.push_back(RamLoadedMusic.mID);
			for (u32 i = 0; i < mdPathContainer.size(); i++)
			{
				if(i != RamLoadedMusic.mID)
					mdShuffleMusicPosContainer.push_back(i);
			}

			std::random_shuffle(mdShuffleMusicPosContainer.begin() + 1, mdShuffleMusicPosContainer.end());
			
		}

		void ReloadMusic(std::wstring path)
		{
			PauseMusic();


			// TODO
		}

		void CheckMPState()
		{
			/* If Music Player has started his first song, next song(next on playlist) will be 
			   played automatically. 
			*/
			if (mdMPStarted == true && IsPlaying() == false && mdRepeatMusic == false)
			{
				NextMusic();
			}
		}

		void CheckVolumeBounds()
		{
			if (mdVolume > 1.0)
				mdVolume = 1.0;

			if (mdVolume < 0.0)
				mdVolume = 0.0;
		}

		b8 IsLoaded()
		{
			return RamLoadedMusic.get() != NULL;
		}

		b8 isPaused()
		{
			return BASS_ChannelIsActive(RamLoadedMusic.get()) == BASS_ACTIVE_PAUSED;
		}

		b8 IsPlaying()
		{
			return BASS_ChannelIsActive(RamLoadedMusic.get());
		}


		std::wstring GetTitle(s32 id)
		{
			std::wstring path;

			if (mdPathContainer.size() > 0 && id >= 0)
			{
#ifdef _WIN32_
				path = std::wstring(mdPathContainer[id].begin(), mdPathContainer[id].end());
#else
				path = mdPathContainer[id];
#endif
				const wchar_t slash = '\\';
				const wchar_t *title = NULL;

				title = wcsrchr(path.c_str(), slash);

				path = std::wstring(title, 1, wcslen(title));
				std::size_t extPos = path.find_last_of(L".");
				path = path.substr(0, extPos);
			}

			return path;
		}

		s32 GetPreviousID()
		{
			s32 id = -1;
			if (mdShuffleMusic)
			{
				if (mdCurrentShuffleMusicPos - 1 < 0)
					id = mdShuffleMusicPosContainer.size() - 1;
				else
					id = mdShuffleMusicPosContainer[mdCurrentShuffleMusicPos - 1];
			}
			else
			{
				if (RamLoadedMusic.mID - 1 < 0)
					id = mdPathContainer.size() - 1;
				else
					id = RamLoadedMusic.mID - 1;
			}

			return id;
		}

		s32 GetNextID()
		{
			s32 id = -1;
			if (mdShuffleMusic)
			{
				if (mdCurrentShuffleMusicPos + 1 > mdShuffleMusicPosContainer.size() - 1)
					id = mdShuffleMusicPosContainer[0];
				else
					id = mdShuffleMusicPosContainer[mdCurrentShuffleMusicPos + 1];
			}
			else
			{
				if (RamLoadedMusic.mID + 1 > mdPathContainer.size() - 1)
					id = 0;
				else
					id = RamLoadedMusic.mID + 1;
			}

			return id;
		}

		std::string GetPositionInString()
		{
			std::string time;

			if (IsPlaying())
			{
				u16 sec = BASS_ChannelBytes2Seconds(
					RamLoadedMusic.get(),
					BASS_ChannelGetPosition(RamLoadedMusic.get(), BASS_POS_BYTE));

				u16 min = 0;
				min = sec / 60;
				if (min < 10)
					time = "0";

				time += std::to_string(min);

				time += ":";

				if ((sec - min * 60) < 10)
					time += "0";

				time += std::to_string(sec - min * 60);
			}
			else
				time = "00:00";

			return time;
		}

		f32 GetPosition()
		{
			if (RamLoadedMusic.get() != NULL)
				return BASS_ChannelBytes2Seconds(RamLoadedMusic.get(),
												 BASS_ChannelGetPosition(RamLoadedMusic.get(), BASS_POS_BYTE));

			return -1;
		}

		void SetActualVolume()
		{
			if(mdVolumeMuted == false)
				BASS_ChannelSetAttribute(RamLoadedMusic.get(), BASS_ATTRIB_VOL, mdVolume);
			else
				BASS_ChannelSetAttribute(RamLoadedMusic.get(), BASS_ATTRIB_VOL, 0.f);
		}

		void SetVolume(f32 vol)
		{
			mdVolumeMuted = false;
			mdVolume = vol;
		}

		f32 GetVolume()
		{
			return mdVolume;
		}

		void MuteVolume(b8 param)
		{
			mdVolumeMuted = param;
		}

		f32 GetMusicLength()
		{
			if(RamLoadedMusic.get() != NULL)
				return BASS_ChannelBytes2Seconds(RamLoadedMusic.get(),
												 BASS_ChannelGetLength(RamLoadedMusic.get(), BASS_POS_BYTE));
			return -1;
		}
#ifdef _DEBUG_
		s32 GetCurrentShufflePos()
		{
			s32 pos = mdCurrentShuffleMusicPos;
			return pos;
		}

		s32 GetShuffleContainerSize()
		{
			return mdShuffleMusicPosContainer.size();
		}

		void PrintShuffledPositions()
		{
			for (u32 i = 0; i < mdShuffleMusicPosContainer.size(); i++)
				std::cout << mdShuffleMusicPosContainer[i] << std::endl;
		}

		void PrintLoadedPaths()
		{
			for (u32 i = 0; i < mdPathContainer.size(); i++)
				std::wcout << mdPathContainer[i] << std::endl;
		}
#endif

		void SetPosition(s32 pos)
		{
			BASS_ChannelSetPosition(
				RamLoadedMusic.get(),
				BASS_ChannelSeconds2Bytes(RamLoadedMusic.get(), pos), 
				BASS_POS_BYTE);
		}

		void RepeatMusic()
		{
			if(IsPlaying() == true)
				mdRepeatMusic = !mdRepeatMusic;
		}

		void ShuffleMusic()
		{
			if(mdPathContainer.size() > 0)
				mdShuffleMusic = !mdShuffleMusic;

			ShuffleMusicProcedure();
		}

		void SetScrollVolumeStep(s8 step)
		{
			mdVolumeScrollStep = step;
		}

		void SetVolumeFadeTime(s32 vol)
		{
			mdVolumeFadeTime = vol;
		}
	}
}
}
