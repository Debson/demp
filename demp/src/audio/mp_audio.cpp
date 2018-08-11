#include "mp_audio.h"

#include <filesystem>
#include <thread>

#include "../utf8_to_utf16.h"
#include "../sqlite/md_sqlite.h"

namespace fs = std::experimental::filesystem::v1;

namespace Audio
{
	static std::vector<AudioItem*> m_AudioItemContainer;
	static s32 itemCount = 0;

	b8 AddAudioItem(const std::wstring path);

}

b8 Audio::PushToPlaylist(const std::wstring path)
{
	if (AddAudioItem(path) == false)
	{
		for (auto & i : fs::directory_iterator(path))
		{
			std::wstring dirPath(i.path().wstring());

			PushToPlaylist(dirPath);
		}
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

	std::thread infoT(Info::GetInfo, &item->info, path);
	infoT.detach();

	//Info::GetInfo(&item->info, path);




	return true;
}



void Audio::GetItemsInfo()
{
	if (m_AudioItemContainer.size() < 3800)
		return;

	for (s32 i = 0; i < m_AudioItemContainer.size(); i++)
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

	//std::cout << m_AudioItemContainer.size() << std::endl;
}

