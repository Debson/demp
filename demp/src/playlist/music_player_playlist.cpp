#include "music_player_playlist.h"

#include <fstream>
#include <algorithm>
#include <filesystem>
#include <Windows.h>

#include <boost/filesystem.hpp>

#include "../ui/music_player_ui.h"
#include "../graphics/music_player_graphics_playlist.h"
#include "../audio/mp_audio.h"
#include "../settings/configuration.h"
#include "../settings/music_player_string.h"
#include "../player/music_player.h"
#include "../player/music_player_state.h"
#include "../utility/md_time.h"
#include "../utility/md_parser.h"
#include "../utility/utf8_to_utf16.h"

// TODO: change it
#ifdef _WIN32_
#define OUTPUT std::cout
#endif

namespace fs = boost::filesystem;


namespace mdEngine
{
namespace MP
{
	namespace Playlist
	{
		SongObject RamLoadedMusic;

		Time::Timer lastDeletionTimer;
		Time::Timer playFadeTimer;
		Time::Timer pauseFadeTimer;

		b8 mdNextRequest(false);
		b8 mdPreviousRequest(false);

		f32 mdVolume = 0.5f;
		f32 mdVolumeTemp;

		f32 mdVolumeFadeInValue = 0;
		f32 mdVolumeFadeOutValue = 0;

		b8 mdMusicPaused(false);
		b8 mdVolumeMuted(false);

		b8 mdMPStarted(false);

		b8 mdVolumeFadeIn(false);
		b8 mdVolumeFadeOut(false);
		
		b8 mdStartNewOnEnd(false);

		f32 mdVolumeScrollStep = 2.f;

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

		void CrossfadeSong();

		void PerformCrossfade();

		void AudioChangeInTray();

		b8 StartNextSong();

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
			if (size > Data::_MAX_SIZE_RAM_LOADED * 1000000)
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
			m_MusicStream = NULL;
			m_Data = NULL;
			m_ID = 0;
			m_PreviousID = 0;
			m_MusicSize = 0;
		}

		SongObject::~SongObject()
		{
			BASS_StreamFree(m_MusicStream);
			m_ID = 0;
			m_MusicStream = NULL;
			m_Data = NULL;
		}
   
#ifdef _WIN32_
		b8 SongObject::load(std::shared_ptr<Audio::AudioObject> audioObject)
		{
			BASS_StreamFree(m_MusicStream);
			FILE* file = NULL;
			if (_wfopen_s(&file, audioObject->GetPathUTF8().c_str(), L"rb+") != 0)
			{
				OUTPUT << "ERROR: could not open file at path \"" << audioObject->GetPath() << "\"\n";
				std::cout << stderr;

				if(fs::exists(audioObject->GetPathUTF8()) == false)
					DeleteMusic(&std::vector<s32>(1, audioObject->GetID()));


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
				s32 size = ftell(file); // size of opened file
				fseek(file, 0L, SEEK_SET); // set cursor at the beginning

				m_Path = audioObject->GetPath();
				m_PreviousID = m_ID;
				m_ID = audioObject->GetID();
				m_MusicSize = size;

				if(m_Data != NULL)
					delete[] m_Data;
				m_Data = NULL;
				m_MusicStream = NULL;

				if (check_size(m_MusicSize) == true)
				{
					m_Data = new char[size];
					s32 newLen = fread(m_Data, sizeof(char), m_MusicSize, file);
					if (ferror(file) == 0)
					{
						fclose(file);
						

						m_MusicStream = BASS_StreamCreateFile(TRUE, m_Data, 0, size, BASS_STREAM_AUTOFREE);
						if (check_file() == false)
						{
							DeleteMusic(&std::vector<s32>(1, audioObject->GetID()));
							return load(Audio::Object::GetAudioObject(m_ID + 1));
						}
					}
					else
					{
						fclose(file);;
						OUTPUT << "ERROR: could not read file at path \"" << audioObject->GetPath() << "\"\n";
						std::cout << stderr << std::endl;
						return false;
					}
				}
				else
				{
					fclose(file);
					delete[] m_Data;
					m_Data = NULL;
#ifdef _WIN32_
					std::wstring test = audioObject->GetPathUTF8();
					m_MusicStream = BASS_StreamCreateFile(FALSE, audioObject->GetPathUTF8().c_str(), 0, 0, 0);
#else
					mMusic = BASS_StreamCreateFile(FALSE, mPath, 0, 0, 0);
#endif

					if (check_file() == false && 
						m_ID + 1 < Audio::Object::GetSize())
					{
						DeleteMusic(&std::vector<s32>(1, audioObject->GetID()));
						return load(Audio::Object::GetAudioObject(m_ID + 1));;
					}
				}
			}

			return true;
		}

		void SongObject::OnFileDeletion()
		{
			m_Path = std::string();
			m_ID = -1;
		}

		void SongObject::Free()
		{
			if (m_Data != NULL)
				delete m_Data;
			m_Data = NULL;
			m_MusicSize = 0;
			if(m_MusicStream != NULL)
				BASS_StreamFree(m_MusicStream);
			m_MusicStream = NULL;
			m_ID = 0;
		}

		HMUSIC& SongObject::get()
		{
			return m_MusicStream;
		}

		void Playlist::InitializeConfig()
		{

			std::string file = Strings::_SETTINGS_FILE;

			Data::VolumeLevel = Parser::GetFloat(file, Strings::_VOLUME);
			if (Data::VolumeLevel < 0)
				Data::VolumeLevel = 0;
			if (Data::VolumeLevel > 1.f)
				Data::VolumeLevel = 1.f;
			mdVolume = Data::VolumeLevel;
			State::SetState(State::VolumeChanged);
			
			/*s32 songID = Parser::GetInt(file, Strings::_CURRENT_SONG_ID);
			RamLoadedMusic.load(Audio::Object::GetAudioObject(songID));*/

			Parser::GetInt(file, Strings::_SHUFFLE_STATE) == 1 ? State::SetState(State::ShuffleAfterLoad) : (void)0;
			mdShuffleMusic = Parser::GetInt(file, Strings::_SHUFFLE_STATE);
			Parser::GetInt(file, Strings::_REPEAT_STATE)	== 1 ? Playlist::RepeatMusic()	: (void)0;

			Data::VolumeScrollStep		= Parser::GetInt(file, Strings::_VOLUME_SCROLL_STEP);
			if (Data::VolumeScrollStep < 0)
				Data::VolumeScrollStep = 0;
			if (Data::VolumeScrollStep > Data::VolumeScrollStepMAX)
				Data::VolumeScrollStep = Data::VolumeScrollStepMAX;

			Data::PlaylistScrollStep	= Parser::GetInt(file, Strings::_PLAYLIST_SCROLL_STEP);
			if (Data::PlaylistScrollStep < 0)
				Data::PlaylistScrollStep = 0;
			if (Data::PlaylistScrollStep > Data::PlaylistScrollStepMAX)
				Data::PlaylistScrollStep = Data::PlaylistScrollStepMAX;

			Data::_MAX_SIZE_RAM_LOADED	= Parser::GetInt(file, Strings::_MAX_RAM_LOADED_SIZE);
			if (Data::_MAX_SIZE_RAM_LOADED < 0)
				Data::_MAX_SIZE_RAM_LOADED = 0;
			if (Data::_MAX_SIZE_RAM_LOADED > Data::_MAX_SIZE_RAM_LOADED_MAX)
				Data::_MAX_SIZE_RAM_LOADED = Data::_MAX_SIZE_RAM_LOADED_MAX;

			Data::PauseFadeTime	= Parser::GetInt(file, Strings::_PAUSE_FADE_TIME);
			if (Data::PauseFadeTime < 0)
				Data::PauseFadeTime = 0;
			if (Data::PauseFadeTime > Data::PauseFadeTimeMAX)
				Data::PauseFadeTime = Data::PauseFadeTimeMAX;

			Parser::GetInt(file, Strings::_ON_EXIT_MINIMIZE_TO_TRAY) == 1 ? State::SetState(State::OnExitMinimizeToTray) : (void)0;

			// App was closed durning some audio file playback, go back exactly to that audio file and it's position
			Parser::GetInt(file, Strings::_SONG_POSITION) > 0 ? State::SetState(State::LoadMusicOnFileLoad) : (void)0;
		}

		void Playlist::Start()
		{
			playFadeTimer = Time::Timer(Data::StartMusicFadeTime);
			pauseFadeTimer = Time::Timer(Data::PauseFadeTime);
		}

		void Playlist::UpdatePlaylist()
		{
			if (lastDeletionTimer.GetTicksStart() > LAST_EVENT_TIME)
			{
				lastDeletionTimer.Stop();
				State::SetState(State::DeletionInProgress);
			}

			if (mdNextRequest)
			{
				//if(RamLoadedMusic.m_ID - 1 >= 0)
				//Audio::Object::GetAudioObject(RamLoadedMusic.m_ID - 1);

				PauseMusic();

				PlayMusic();

				mdNextRequest = false;
			}

			if (mdPreviousRequest)
			{
				//if(RamLoadedMusic.m_ID + 1 < Audio::Object::GetSize())
				//Audio::Object::GetAudioObject(RamLoadedMusic.m_ID + 1);

				PauseMusic();

				PlayMusic();

				mdPreviousRequest = false;
			}

			// Music fade out on pause, after fade out finished, stop channel
			if (pauseFadeTimer.finished == true)
			{
				if (mdMusicPaused == true)
				{
					BASS_ChannelPause(RamLoadedMusic.get());
				}

				pauseFadeTimer.finished = false;
			}
		

			if (State::CheckState(State::DeletionInProgress) == true)
			{
				ShuffleMusicProcedure();
				//ShuffleMusic();
			}
		}

		void UpdateMusic()
		{
			// If mdVolume was changed, update channel volume with mdVolume and update global VolumeLevel(visual volume)
			if(State::CheckState(State::VolumeChanged) == true && 
			   playFadeTimer.finished == true)
			{
				SetActualVolume();
				Data::VolumeLevel = mdVolume;
				State::ResetState(State::VolumeChanged);
			}
			else if (State::CheckState(State::VolumeChanged) == true)
			{// Channel fading in, update global volume anyway(visual volume)
				Data::VolumeLevel = mdVolume;
			}

			/*	Example: Song started, so the channel is fading in to mdVolume, but during that time mdVolume 
						 was changed and is lower than channel volume, imediately stop fade.
			*/
			if ((playFadeTimer.started == true && playFadeTimer.finished == false) ||
				(pauseFadeTimer.started == true && pauseFadeTimer.finished == false))
			{
				f32 vol;
				BASS_ChannelGetAttribute(RamLoadedMusic.get(), BASS_ATTRIB_VOL, &vol);

				if (vol > mdVolume)
					BASS_ChannelSlideAttribute(RamLoadedMusic.get(), BASS_ATTRIB_VOL | BASS_SLIDE_LOG, mdVolume, 1);
			}

			CheckMPState();

			RepeatMusicProcedure();

			CrossfadeSong();

			AudioChangeInTray();

			playFadeTimer.Update();
			pauseFadeTimer.Update();
		}

		void PlayMusic()
		{
			if (Audio::Object::GetSize() == 0)
				return;


			/* Reset booleans that control music state */
			PauseMusic();


			/* If current's song position is greater than 0, it means that song's
			   position was loaded from file. */
			s32 pos = 0;
			if (GetPosition() > 0)
				pos = GetPosition();

			if (Audio::Object::GetSize() < 1)
				return;

			if (RamLoadedMusic.get() != NULL) {
				if (IsPlaying() == false) {//&& RamLoadedMusic.load(Audio::Object::GetAudioObject(RamLoadedMusic.m_ID)))
					mdMPStarted = true;
				}
				else {
					mdMPStarted = false;
					return;
				}
			}

			mdMusicPaused = false;
			mdStartNewOnEnd = true;

			Graphics::MP::GetPlaylistObject()->SetPlayingID(Audio::Object::GetAudioObject(RamLoadedMusic.m_ID)->GetID());
			Graphics::MP::GetPlaylistObject()->SetSelectedID(RamLoadedMusic.m_ID);

			if (Audio::Object::GetAudioObject(RamLoadedMusic.m_PreviousID) != nullptr) {
				Audio::Object::GetAudioObject(RamLoadedMusic.m_PreviousID)->DeleteAlbumImageTexture();
			}
			Audio::Object::GetAudioObject(RamLoadedMusic.m_ID)->LoadAlbumImage();

			State::SetState(State::RequestForInfoLoad);

			if (Audio::Object::GetAudioObject(RamLoadedMusic.m_ID)->IsInfoLoaded() == false) {
				Audio::Info::GetInfo(Audio::Object::GetAudioObject(RamLoadedMusic.m_ID));
			}

			State::ResetState(State::RequestForInfoLoad);


			State::SetState(State::AudioChosen);

			BASS_ChannelPlay(RamLoadedMusic.get(), true);
			playFadeTimer.Start();
			SetPosition(pos);

			BASS_ChannelSetAttribute(RamLoadedMusic.get(), BASS_ATTRIB_VOL, 0.f);
			BASS_ChannelSlideAttribute(RamLoadedMusic.get(), BASS_ATTRIB_VOL | BASS_SLIDE_LOG, mdVolume, Data::StartMusicFadeTime);
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
				mdMusicPaused = !mdMusicPaused;
				pauseFadeTimer.Start();

				if (mdMusicPaused == true)
				{
					BASS_ChannelSlideAttribute(RamLoadedMusic.get(), BASS_ATTRIB_VOL, 0, Data::PauseFadeTime);
				}
				else
				{
					BASS_ChannelPlay(RamLoadedMusic.get(), false);
					BASS_ChannelSlideAttribute(RamLoadedMusic.get(), BASS_ATTRIB_VOL, mdVolume, Data::PauseFadeTime);
				}
			}
		}

		void NextMusic()
		{
			if ((RamLoadedMusic.get() != NULL || BASS_ErrorGetCode() == BASS_ERROR_FILEFORM) &&
				Audio::Object::GetSize() > 0)
			{
				f32 pos = GetPosition();
				BASS_ChannelStop(RamLoadedMusic.get());

				if (State::CheckState(State::CurrentlyPlayingDeleted) == true)
				{
					RamLoadedMusic.m_ID--;
					State::ResetState(State::CurrentlyPlayingDeleted);
				}

				if (RamLoadedMusic.m_ID > Audio::Object::GetSize())
					RamLoadedMusic.m_ID = -1;

				/* If shuffle is enabled, try to load next song basing on shuffled positions container.

				If song path exist on next position, load it to the ram,
				else load song from path at position 0 in path's container.

				While loop:
				While path in PathContainer at position (current_path_id + index) is not valid and
				path on next position exist, try to open file at that path position.

				*/
				if (mdShuffleMusic == false)
				{
					if ((RamLoadedMusic.m_ID + 1) < Audio::Object::GetSize())
					{
						s32 index = 1;
						while ((RamLoadedMusic.load(Audio::Object::GetAudioObject(RamLoadedMusic.m_ID + index)) == false)
							&& (RamLoadedMusic.m_ID + 1 < Audio::Object::GetSize()))
						{
							index++;
						}

						if(pos != 0)
							mdNextRequest = true;
					}
					else
					{
						/*	Example: Audio file is already playing and the same audio file was opened in different player, so
									 so it doesn't have access to load that file
						*/
						if (RamLoadedMusic.load(Audio::Object::GetAudioObject(0)) == true)
						{
							if (pos != 0)
								mdNextRequest = true;
						}
						else
						{
							mdNextRequest = false;
							return;
						}

					}
				}
				else
				{
					mdCurrentShuffleMusicPos++;
					if (mdCurrentShuffleMusicPos > mdShuffleMusicPosContainer.size() - 1)
						mdCurrentShuffleMusicPos = 0;
					while (RamLoadedMusic.load(Audio::Object::GetAudioObject(mdShuffleMusicPosContainer[mdCurrentShuffleMusicPos])) == false)
					{
						mdCurrentShuffleMusicPos++;
						if (mdCurrentShuffleMusicPos > mdShuffleMusicPosContainer.size() - 1)
							mdCurrentShuffleMusicPos = 0;
					}

					mdNextRequest = true;
					
				}
			}

			State::SetState(State::AudioChanged);

		}

		void PreviousMusic()
		{
			if ((RamLoadedMusic.get() != NULL || BASS_ErrorGetCode() == BASS_ERROR_FILEFORM) &&
				Audio::Object::GetSize() > 0)
			{
				f32 pos = GetPosition();

				BASS_ChannelStop(RamLoadedMusic.get());

				State::SetState(State::AudioChanged);
				State::ResetState(State::CurrentlyPlayingDeleted);

				if (RamLoadedMusic.m_ID > Audio::Object::GetSize())
					RamLoadedMusic.m_ID = -1;

				/*	If shuffle is enabled, try to load next song basing on shuffled positions container.
				
				If song path exist on previous position, load it to the ram,
				else load song from path at position `PathContainer.Size() - 1`

				While loop:
				While path in PathContainer at position (current_path_id - index) is not valid and
				path on previous position exist, try to open file at that path position.

				*/
				if (mdShuffleMusic == false)
				{
					if ((RamLoadedMusic.m_ID - 1) >= 0)
					{
						int index = 1;
						while ((RamLoadedMusic.load(Audio::Object::GetAudioObject(RamLoadedMusic.m_ID - index)) == false)
							&& (RamLoadedMusic.m_ID - index >= 0))
						{
							index++;
						}

						if (pos != 0)
							mdPreviousRequest = true;
					}
					else
					{
						if (RamLoadedMusic.load(Audio::Object::GetAudioObject(Audio::Object::GetSize() - 1)) == true)
						{
							if (pos != 0)
								mdPreviousRequest = true;
						}
						else
						{
							mdPreviousRequest = false;
							return;
						}
					}
				}
				else
				{
					mdCurrentShuffleMusicPos--;
					if (mdCurrentShuffleMusicPos < 0)
						mdCurrentShuffleMusicPos = Audio::Object::GetSize() - 1;
					while (RamLoadedMusic.load(Audio::Object::GetAudioObject(mdShuffleMusicPosContainer[mdCurrentShuffleMusicPos])) == false)
					{
						mdCurrentShuffleMusicPos--;
						if (mdCurrentShuffleMusicPos < 0)
							mdCurrentShuffleMusicPos = Audio::Object::GetSize() - 1;
					}

					mdPreviousRequest = true;
					
				}
			}
		}

		void IncreaseVolume(App::InputEvent event)
		{
			State::SetState(State::VolumeChanged);
			MuteVolume(false);

			switch (event)
			{
			case App::InputEvent::kPressedEvent:
				mdVolume += (Data::VolumeKeyMultiplier * Time::deltaTime);
				break;
			case App::InputEvent::kScrollEvent:
				mdVolume += (Data::VolumeScrollStep / 100.f);
				break;
			}

			if (mdVolume > 1.f)
				mdVolume = 1.f;
			if (mdVolume < 0)
				mdVolume = 0.f;
		}

		void LowerVolume(App::InputEvent event)
		{
			State::SetState(State::VolumeChanged);

			switch (event)
			{
			case App::InputEvent::kPressedEvent:
				mdVolume -= (Data::VolumeKeyMultiplier * Time::deltaTime);
				break;
			case App::InputEvent::kScrollEvent:
				mdVolume -= (Data::VolumeScrollStep / 100.f);
				md_log(mdVolume);
				break;
			};

			if (mdVolume > 1.f)
				mdVolume = 1.f;
			if (mdVolume < 0)
				mdVolume = 0.f;
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
				if (IsPlaying() == false && mdMPStarted == true)
				{
					SetPosition(0);
					PlayMusic();
				}
			}
		}

		void ShuffleMusicProcedure()
		{
			/* Fill vector with numbers from 0 to path container's size and then random shuffle 
			   all the numbers, creating a playlist with shuffeled songs.
			*/

			if (mdShuffleMusicPosContainer.empty() == false)
			{
				mdShuffleMusicPosContainer.clear();
				mdCurrentShuffleMusicPos = 0;
			}
			
			if (RamLoadedMusic.m_ID < Audio::Object::GetSize())
				mdShuffleMusicPosContainer.push_back(RamLoadedMusic.m_ID);

			for (u32 i = 0; i < Audio::Object::GetSize(); i++)
			{
				if(i != RamLoadedMusic.m_ID)
					mdShuffleMusicPosContainer.push_back(i);
			}

			if(mdShuffleMusicPosContainer.empty() == false)
				std::random_shuffle(mdShuffleMusicPosContainer.begin() + 1, mdShuffleMusicPosContainer.end());
			
		}

		void ReloadMusic(std::string path)
		{
			PauseMusic();


			// TODO
		}

		void CheckMPState()
		{
			/* If Music Player has started his first song, next song(next on playlist) will be 
			   played automatically. Check if next music was selected by user 
			*/
			if (StartNextSong() == true)
			{
				NextMusic();
			}

			/* Playlist is empty and  */
			if (Audio::Object::GetSize() == 0 && 
				State::CheckState(State::InitialLoadFromFile) == false &&
				IsPlaying() == false)
			{
				mdStartNewOnEnd = false;
				mdCurrentShuffleMusicPos = 0;
				//Graphics::MP::GetPlaylistObject()->SetPlayingID(-1);
				RamLoadedMusic.Free();
			}

			if (State::CheckState(State::ShuffleAfterAddition) &&
				mdShuffleMusic == true)
			{
				ShuffleMusic();
				//ShuffleMusicProcedure();
				State::ResetState(State::ShuffleAfterAddition);
			}

			if (State::CheckState(State::FilesLoaded) == true &&
				State::CheckState(State::ShuffleAfterLoad) == true &&
				mdShuffleMusic == true)
			{
				ShuffleMusic();
				//ShuffleMusicProcedure();
				State::ResetState(State::ShuffleAfterLoad);
			}
			else
			{
				State::ResetState(State::ShuffleAfterLoad);
			}

			if (Audio::Object::GetSize() == 0)
			{
				mdShuffleMusicPosContainer.clear();
			}

			/* Load, basing on file output, last song that was played before exiting application */
			if (Audio::Object::GetSize() > 0 &&
				State::CheckState(State::LoadMusicOnFileLoad) == true)
			{
				std::string file = Strings::_SETTINGS_FILE;
				s32 songID = Parser::GetInt(file, Strings::_CURRENT_SONG_ID);
				if (songID < Audio::Object::GetSize() && songID >= 0)
				{
					md_log(songID);
					RamLoadedMusic.load(Audio::Object::GetAudioObject(songID));
					Playlist::SetPosition((s32)Parser::GetFloat(file, Strings::_SONG_POSITION));
					Graphics::MP::GetPlaylistObject()->SetPlayingID(Audio::Object::GetAudioObject(songID)->GetID());
					State::SetState(State::InitialLoadFromFile);
				}
				State::ResetState(State::LoadMusicOnFileLoad);
			}
		}

		void DeleteMusic(const std::vector<s32>* indexes)
		{
			if (State::CheckState(State::FilesDroppedNotLoaded) == true)
			{
				return;
			}

			RamLoadedMusic.OnFileDeletion();
			//State::SetState(State::AudioDeleted);

			auto sepCont = Interface::Separator::GetContainer();
			State::SetState(State::DeletionInProgress);
			//Interface::Separator::GetContainer()->clear();

			// If deleted file is smaller than 10% of the playlist size, use different algorithm
			b8 smallDeletion = (indexes->size() < (0.1f * float(Audio::Object::GetSize())) || Audio::Object::GetSize() < 100) ? true : false;

			/*	Note: Useful only where files are being processed and deletion occurs.
				Save audio container's size before deletion(if deletion is a large deletion of files
				and some files are still left, it will terminate threads that were working on extracting
				info with old audio container data)
			*/
			Audio::AudioContainerSizeBeforeDeletion = 0.1f * float(Audio::Object::GetSize());

			for (auto i : *indexes)
			{
				if (i < 0 || i > Audio::Object::GetSize() - 1)
					return;
				State::SetState(State::AudioDeleted);
				// BUG: Can't erase pos in playlist button container

				Audio::PerformDeletion(i, smallDeletion);

				if (Graphics::MP::GetPlaylistObject()->GetPlayingID() == i)
					State::SetState(State::CurrentlyPlayingDeleted);

				if (Graphics::MP::GetPlaylistObject()->GetSelectedID() > Audio::Object::GetSize() - 1)
					Graphics::MP::GetPlaylistObject()->SetSelectedID(Audio::Object::GetSize() - 1);

			}

			/*	If there is large deletion of files and some items are left after deletion
				set folders rep again(because separator's vector was cleared) and load info 
				for left items
			*/
			if (smallDeletion == false && 
				Audio::Object::GetSize() > 0)
			{
				Audio::PerformSetFoldersRep();
				Audio::LoadFilesInfo();
			}

			Audio::AudioContainerSizeBeforeDeletion = 0;
		}

		void CrossfadeSong()
		{
			if (GetMusicLength() - GetPosition() < 10)
			{
				//PerformCrossfade();
			}
		}

		void PerformCrossfade()
		{

		}

		void AudioChangeInTray()
		{
			// Increase fps when audio is about to change
			// If it is last 2 seconds of currently playing song
			if (GetMusicLength() - GetPosition() < 2.f)
			{
				State::SetState(State::AudioChangedInTray);
			}
			// If it is first 2-3 seconds of the song, reset that state.
			if (GetPosition() > 2.f && GetPosition() - 3.f <= 0)
			{
				State::ResetState(State::AudioChangedInTray);
			}
		}

		b8 StartNextSong()
		{

			/*	Uncomment so music won't repeat after it reaches last song on the playlist or 
				last song in the shuffle container
			*/

			/*if (RamLoadedMusic.m_ID + 1 >= Audio::Object::GetSize() &&
				mdShuffleMusic == false)
			{
				mdStartNewOnEnd = false;
			}

			if (mdCurrentShuffleMusicPos + 1 >= mdShuffleMusicPosContainer.size() && 
				mdShuffleMusic == true)
			{
				mdStartNewOnEnd = false;
			}*/

			/*if (mdRepeatMusic == false)
				return false;

			if (mdMPStarted == false)
				return false;


			if (State::CheckState(State::FilesLoading) == true)
				return false;

			if (mdStartNewOnEnd == false)
				return false;

			if (IsPlaying() == true)
				return false;

			return true;*/

			if (Playlist::IsPlaying() == true)
			{
				return false;
			}

			if (Audio::Object::GetSize() > 1 && mdRepeatMusic == false && mdMPStarted == true)
				return true;

			// Free loaded song
			if (Audio::Object::GetSize() <= 1)
			{
				//Graphics::MP::GetPlaylistObject()->SetPlayingID(-1);
				RamLoadedMusic.Free();
				mdMPStarted = false;
			}

			return false;
		}

		b8 IsLoaded()
		{
			return RamLoadedMusic.get() != NULL;
		}

		b8 IsPaused()
		{
			return BASS_ChannelIsActive(RamLoadedMusic.get()) == BASS_ACTIVE_PAUSED;
		}

		b8 IsPlaying()
		{
			return BASS_ChannelIsActive(RamLoadedMusic.get());
		}

		b8 IsChannelPlaying()
		{
			if (mdMusicPaused == true)
				return !mdMusicPaused;

			return BASS_ChannelIsActive(RamLoadedMusic.get()) == BASS_ACTIVE_PLAYING;
		}

		b8 IsShuffleEnabled()
		{
			return mdShuffleMusic;
		}

		b8 IsRepeatEnabled()
		{
			return mdRepeatMusic;
		}


		std::string GetTitle(s32 id)
		{
			std::string path;

			/*if (Audio::Object::GetSize() > 0 && id >= 0)
			{
#ifdef _WIN32_
				path = std::string(Audio::Object::GetAudioObject(id)->GetPath());
#else
				path = mdPathContainer[id];
#endif
				const wchar_t slash = '\\';
				const wchar_t *title = NULL;


				title = wcsrchr(path.c_str(), slash);

				path = std::string(title, 1, wcslen(title));
				std::size_t extPos = path.find_last_of(L".");
				path = path.substr(0, extPos);
			}*/

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
				if (RamLoadedMusic.m_ID - 1 < 0)
					id = Audio::Object::GetSize() - 1;
				else
					id = RamLoadedMusic.m_ID - 1;
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
				if (RamLoadedMusic.m_ID + 1 > Audio::Object::GetSize() - 1)
					id = 0;
				else
					id = RamLoadedMusic.m_ID + 1;
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
			{
				BASS_ChannelSetAttribute(RamLoadedMusic.get(), BASS_ATTRIB_VOL, 0.f);
				Data::VolumeLevel = 0.f;
			}
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

		void GetBitrate(f32* bitrate)
		{
			BASS_ChannelGetAttribute(RamLoadedMusic.get(), BASS_ATTRIB_BITRATE, bitrate);
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

		void SetPosition(s32 pos)
		{
			BASS_ChannelSetPosition(
				RamLoadedMusic.get(),
				BASS_ChannelSeconds2Bytes(RamLoadedMusic.get(), pos), 
				BASS_POS_BYTE);
		}

		void RepeatMusic()
		{
			//if(IsPlaying() == true)
				mdRepeatMusic = !mdRepeatMusic;
		}

		void ShuffleMusic()
		{
			if(Audio::Object::GetSize() > 0)
				mdShuffleMusic = !mdShuffleMusic;

			if(mdShuffleMusic == true)
				ShuffleMusicProcedure();
		}

		void SetScrollVolumeStep(s8 step)
		{
			mdVolumeScrollStep = step;
		}

		void SetVolumeFadeTime(s32 vol)
		{
			
			Data::PauseFadeTime = vol;
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
			for (u32 i = 0; i < Audio::Object::GetSize(); i++)
				std::cout << Audio::Object::GetAudioObject(i)->GetPath() << std::endl;
		}
#endif
	}
}
}
