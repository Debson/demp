#include "mp_audio.h"

#include <thread>
#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include "../utf8_to_utf16.h"
#include "../sqlite/md_sqlite.h"
#include "../md_util.h"
#include "../md_time.h"

namespace fs = boost::filesystem;

namespace Audio
{
	static std::vector<AudioItem*> m_AudioItemContainer;
	static s32 itemCount = 0;

	b8 loadingPaths(false);
	b8 firstEnter(false);
	s32 lastVecSize = 0;


	b8 AddAudioItem(const std::wstring path);

	void RetrieveInfo();

}

void Audio::Test(std::wstring pa)
{
	

}


b8 Audio::PushToPlaylist(const std::wstring path)
{
	if (fs::exists(path))
	{
		if (fs::is_directory(path))
		{
			loadingPaths = true;
			for (auto & i : fs::directory_iterator(path))
			{
				AddAudioItem(i.path().wstring());
				if (fs::is_directory(i.path().wstring()))
					PushToPlaylist(i.path().wstring());
			}
		}
	}
	else
	{
		MD_ERROR("File at path \"" + utf16_to_utf8(path) + "\" does not exist!");
	}

	return true;
}

b8 Audio::DeallocateAudioItems()
{
	for (s32 i = 0; i < m_AudioItemContainer.size(); i++)
	{
		delete m_AudioItemContainer[i];
	}

	m_AudioItemContainer.clear();

	return true;
}

void Audio::UpdateAudioLogic()
{
	if (firstEnter == false)
	{
		lastVecSize = m_AudioItemContainer.size();
		firstEnter = true;
	}

	if (lastVecSize != m_AudioItemContainer.size())
	{
		lastVecSize = m_AudioItemContainer.size();
		loadingPaths = false;
	}
	
	if (loadingPaths)
	{
		std::thread t(RetrieveInfo);
		t.detach();
	}
}

void Audio::RetrieveInfo()
{
	for (s32 i = 0; i < m_AudioItemContainer.size(); i++)
	{
		Info::GetInfo(&m_AudioItemContainer[i]->info, m_AudioItemContainer[i]->path);
	}
	std::cout << Time::GetTicks() << std::endl;
}

Audio::AudioItem* Audio::Items::GetItem(s32 id)
{
	if (m_AudioItemContainer.size() > 0 &&
		m_AudioItemContainer.size() > id &&
		id >= 0)
	{
		return m_AudioItemContainer.at(id);
	}

	return NULL;
}

u32 Audio::Items::GetSize()
{
	return m_AudioItemContainer.size();
}

b8 Audio::AddAudioItem(const std::wstring path)
{
	if (Info::CheckIfAudio(path) == false)
		return false;

	AudioItem * item = new AudioItem();
	item->id = itemCount;
	item->path = path;
	item->folder = Info::GetFolder(path);
	item->name = Info::GetName(path);
	itemCount++;
	m_AudioItemContainer.push_back(item);

	return true;
}



void Audio::GetItemsInfo()
{
	
	for (s32 i = 3200; i < m_AudioItemContainer.size(); i++)
	{
		std::cout << std::endl;
		std::cout << m_AudioItemContainer[i]->id << std::endl;
		std::cout << utf16_to_utf8(m_AudioItemContainer[i]->path) << std::endl;
		std::cout << utf16_to_utf8(m_AudioItemContainer[i]->folder) << std::endl;
		std::cout << utf16_to_utf8(m_AudioItemContainer[i]->name) << std::endl;
		std::wcout << m_AudioItemContainer[i]->info.ext << std::endl;
		std::cout << m_AudioItemContainer[i]->info.freq << std::endl;
		std::cout << m_AudioItemContainer[i]->info.bitrate << std::endl;
		std::cout << m_AudioItemContainer[i]->info.size << std::endl;
		std::cout << m_AudioItemContainer[i]->info.length << "s" << std::endl;
	}

	std::cout << m_AudioItemContainer.size() << std::endl;
}

