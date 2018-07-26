#include "music_player_playlist.h"

#include <fstream>
#include <filesystem>

#include "md_time.h"


namespace mdEngine
{
namespace MP
{
	namespace Playlist
	{
		PathContainer mdPathContainer;
		SongObject RamLoadedSong;

		b8 mdNextRequest(false);
		b8 mdPreviousRequest(false);

		f64 mdVolume = 0.5;

		f32 mdVolumeFadeInValue = 0;
		f32 mdVolumeFadeOutValue = 0;

		b8 mdMusicPaused(false);

		b8 mdVolumeFadeIn(false);
		b8 mdVolumeFadeOut(false);


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
			mPath = NULL;
		}

		SongObject::~SongObject()
		{
			mMusic = NULL;
			mPath = NULL;
		}

		b8 SongObject::init(const char* songPath)
		{
			std::ifstream file(songPath, std::ios::binary | std::ios::ate);

			if (!file)
			{
				std::cerr << "ERROR::FILE_LOAD: code: " << strerror(errno)
					<< "\nPath: \"" << songPath << "\"\n!";
				return false;
			}
			else
			{
				std::streamsize size = file.tellg(); // size of opened file
				file.seekg(0, std::ios::beg); // set cursor at the beginning

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
					if (file.read(mData, mSize))
					{
						file.close();
						mMusic = BASS_StreamCreateFile(TRUE, mData, 0, size, BASS_STREAM_AUTOFREE);
						if (check_file() == false)
						{
							return false;
						}
					}
					else
					{
						file.close();
						std::cerr << "ERROR::FILE_READ: code: " << strerror(errno)
							<< "\nPath: \"" << songPath << "\"\n!";;
						return false;
					}
				}
				else
				{
					delete[] mData;
					mData = NULL;
					mMusic = BASS_StreamCreateFile(FALSE, mPath, 0, 0, 0);

					if (check_file() == false)
					{
						return false;
					}
				}
			}

			return true;

		}

		b8 SongObject::load(const char* songPath, u32 id, MP::Playlist::SongState state)
		{
			std::ifstream file(songPath, std::ios::binary | std::ios::ate);

			if (!file)
			{
				std::cerr << "ERROR::FILE_LOAD: code: " << strerror(errno) << std::endl;
				return false;
			}
			else
			{

				std::streamsize size = file.tellg(); // size of opened file
				file.seekg(0, std::ios::beg); // set cursor at the beginning

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
					if (file.read(mData, mSize))
					{
						file.close();
						mMusic = BASS_StreamCreateFile(TRUE, mData, 0, size, BASS_STREAM_AUTOFREE);
						if (check_file() == false)
						{
							delete mdPathContainer[mID];
							mdPathContainer.erase(mdPathContainer.begin() + mID);

							if (state == MP::Playlist::SongState::mNext)
								return load(mdPathContainer[mID - 1], mID - 1, MP::Playlist::SongState::mNext);

							return load(mdPathContainer[mID + 1], mID + 1, MP::Playlist::SongState::mPrevious);
						}
					}
					else
					{
						file.close();
						std::cerr << "ERROR::FILE_READ: code: " << strerror(errno) << std::endl;
						return false;
					}
				}
				else
				{
					delete[] mData;
					mData = NULL;
					mMusic = BASS_StreamCreateFile(FALSE, mPath, 0, 0, 0);

					if (check_file() == false)
					{
						delete mdPathContainer[mID];
						mdPathContainer.erase(mdPathContainer.begin() + mID);

						if (state == MP::Playlist::SongState::mNext)
							return load(mdPathContainer[mID - 1], mID - 1, MP::Playlist::SongState::mNext);

						return load(mdPathContainer[mID + 1], mID + 1, MP::Playlist::SongState::mPrevious);;
					}
				}
			}

			return true;
		}

		b8 SongObject::update(const char* songPath)
		{

			return true;
		}

		HMUSIC& SongObject::get()
		{
			return mMusic;
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
			BASS_ChannelGetAttribute(RamLoadedSong.get(), BASS_ATTRIB_VOL, &vol);

			if (mdVolumeFadeOut)
			{
				if (vol <= 0)
				{
					BASS_ChannelPause(RamLoadedSong.get());
				}
			}
			else if (mdVolumeFadeIn)
			{
				if (vol >= mdVolume)
				{
					mdVolumeFadeIn = false;
				}
			}
			else
			{
				SetMusicVolume(mdVolume);
			}

			if (MP::Input::IsKeyPressed(MP::KeyCode::I))
			{
				std::cout << BASS_ChannelGetLength(RamLoadedSong.get(), BASS_POS_BYTE) << std::endl;
			}

			//std::cout << (BASS_ChannelIsActive(RamLoadedSong.get()) == (BASS_ACTIVE_STOPPED)) << std::endl;
		}

		void PlayMusic()
		{
			/* Reset booleans that control music state */
			mdVolumeFadeIn = false;
			mdVolumeFadeOut = false;
			mdMusicPaused = false;

			if (RamLoadedSong.get() != NULL)
			{
				BASS_ChannelPlay(RamLoadedSong.get(), true);
			}

		}

		void StopMusic()
		{
			if (RamLoadedSong.get() != NULL)
			{
				BASS_ChannelPause(RamLoadedSong.get());
				BASS_ChannelSetPosition(RamLoadedSong.get(), 0, BASS_POS_BYTE);
			}
		}

		void PauseMusic()
		{

			if (BASS_ChannelIsActive(RamLoadedSong.get()) != BASS_ACTIVE_STOPPED && RamLoadedSong.get() != NULL)
			{
				mdMusicPaused == false ? (mdMusicPaused = true) : (mdMusicPaused = false);

				if (mdMusicPaused)
				{
					BASS_ChannelSlideAttribute(RamLoadedSong.get(), BASS_ATTRIB_VOL, 0, Settings::VolumeFadeTime);
					mdVolumeFadeOut = true;
				}
				else
				{
					mdVolumeFadeOut = false;
					BASS_ChannelPlay(RamLoadedSong.get(), false);
					BASS_ChannelSlideAttribute(RamLoadedSong.get(), BASS_ATTRIB_VOL, mdVolume, Settings::VolumeFadeTime);
					mdVolumeFadeIn = true;
				}
			}

		}

		void NextMusic()
		{
			if ((RamLoadedSong.get() != NULL) || (BASS_ErrorGetCode() == BASS_ERROR_FILEFORM))
			{
				BASS_ChannelStop(RamLoadedSong.get());

				/*	If song path exist on next position, load it to the ram,
				else load song from path at position 0 in path's container.

				While loop:
				While path in PathContainer at position (current_path_id + index) is not valid and
				path on next position exist, try to open file at that path position.

				*/
				std::cout << RamLoadedSong.mID << std::endl;
				if ((RamLoadedSong.mID + 1) < mdPathContainer.size())
				{
					int index = 1;
					while ((RamLoadedSong.load(mdPathContainer[RamLoadedSong.mID + index], RamLoadedSong.mID + index, SongState::mNext) == false)
						&& (RamLoadedSong.mID + 1 < mdPathContainer.size()))
					{
						index++;
					}
					//assert(RamLoadedSong.load(mdPathContainer[RamLoadedSong.mID + index], RamLoadedSong.mID + index));

					mdNextRequest = true;
				}
				else
				{
					RamLoadedSong.load(mdPathContainer[0], 0, SongState::mCurrent);
					mdNextRequest = true;
				}
			}
		}

		void PreviousMusic()
		{
			if (RamLoadedSong.get() != NULL)
			{
				BASS_ChannelStop(RamLoadedSong.get());

				/*	If song path exist on previous position, load it to the ram,
				else load song from path at position `PathContainer.Size() - 1`

				While loop:
				While path in PathContainer at position (current_path_id - index) is not valid and
				path on previous position exist, try to open file at that path position

				*/
				std::cout << RamLoadedSong.mID << std::endl;
				if ((RamLoadedSong.mID - 1) >= 0)
				{
					int index = 1;
					while ((RamLoadedSong.load(mdPathContainer[RamLoadedSong.mID - index], RamLoadedSong.mID - index, SongState::mPrevious) == false)
						&& (RamLoadedSong.mID - index >= 0))
					{
						index++;
					}

					//assert(RamLoadedSong.load(mdPathContainer[RamLoadedSong.mID - index], RamLoadedSong.mID - index));
					mdPreviousRequest = true;
				}
				else
				{
					RamLoadedSong.load(mdPathContainer[mdPathContainer.size() - 1], mdPathContainer.size() - 1, SongState::mCurrent);
					mdPreviousRequest = true;
				}
			}
		}


		void IncreaseVolume(MP::InputEvent event)
		{
			if (mdVolume > 1.0)
			{
				mdVolume = 1.0;
			}
			else
			{
				switch (event)
				{
				case MP::InputEvent::kPressedEvent:
					mdVolume += (Settings::VolumeKeyMultiplier * Time::deltaTime);
					break;
				case MP::InputEvent::kScrollEvent:
					mdVolume += (Settings::VolumeScrollStep / 100.f);
					break;
				}
			}

			std::cout << "Volume: " << mdVolume << std::endl;
		}

		void LowerVolume(MP::InputEvent event)
		{
			if (mdVolume < 0)
			{
				mdVolume = 0;
			}
			else
			{
				switch (event)
				{
				case MP::InputEvent::kPressedEvent:
					mdVolume -= (Settings::VolumeKeyMultiplier * Time::deltaTime);
					break;
				case MP::InputEvent::kScrollEvent:
					mdVolume -= (Settings::VolumeScrollStep / 100.f);
					break;
				};
			}

			std::cout << "Volume: " << mdVolume << std::endl;
		}

		void SetMusicVolume(f32 vol)
		{
			BASS_ChannelSetAttribute(RamLoadedSong.get(), BASS_ATTRIB_VOL, vol);
		}

		void RewindMusic(s32 pos)
		{
			if (RamLoadedSong.get() != NULL)
			{
				u64 currentPos = BASS_ChannelGetPosition(RamLoadedSong.get(), BASS_POS_BYTE);
				u64 previousPos = currentPos;
				u64 bytes;
				if (pos < 0)
				{
					bytes = BASS_ChannelSeconds2Bytes(RamLoadedSong.get(), -(pos));
					currentPos -= bytes;
					if (previousPos < currentPos)
						currentPos = 0;
				}
				else
				{
					u64 length = BASS_ChannelGetLength(RamLoadedSong.get(), BASS_POS_BYTE);
					bytes = BASS_ChannelSeconds2Bytes(RamLoadedSong.get(), pos);
					currentPos += bytes;
					if (previousPos >= length)
					{
						PlayMusic();
						currentPos = 0;
					}
				}

				std::cout << "Music pos: " << BASS_ChannelBytes2Seconds(RamLoadedSong.get(), currentPos) << std::endl;
				BASS_ChannelSetPosition(RamLoadedSong.get(), currentPos, BASS_POS_BYTE);
			}
		}
	}
}
}
