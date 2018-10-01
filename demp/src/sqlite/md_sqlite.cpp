#include "md_sqlite.h"

#include <filesystem>
#include <sqlite3.h>

#include "../settings/music_player_string.h"
#include "../utility/md_util.h"
#include "../utility/utf8_to_utf16.h"


namespace fs = std::experimental::filesystem::v1;

namespace mdEngine
{
	namespace Database
	{
		sqlite3 *db;
		s32 rc;

		static int callback(void *NotUsed, int argc, char **argv, char **azColName);

		b8 Connect();
		b8 Disconnect();
		b8 CreateTable(Audio::AudioProperties* audioItem);
		b8 Insert(Audio::AudioProperties* audioItem);
		b8 Select(std::string table);
		void Delete(std::string item);

	}

	b8 Database::OpenDB()
	{
		return Connect();
	}

	b8 Database::PushToDatabase(Audio::AudioProperties* audioItem)
	{
		//Connect();

		CreateTable(audioItem);

		Insert(audioItem);
		
		return true;

		//return Disconnect();
	}

	b8 Database::CloseDB()
	{
		return Disconnect();
	}

	void Database::GetItemsInfo()
	{
		//Connect();
		for (u32 i = 0; i < Audio::Object::GetSize(); i++)
		{
			Select(Audio::Object::GetAudioObject(i)->GetFolderPath());
			std::cout << "\n\n";
		}
		//Disconnect();
	}

	b8 Database::Connect()
	{
		char *zErrMsg = 0;

		rc = sqlite3_open(Strings::_DATABASE_FILE.c_str(), &db);

		if (rc) 
		{
			//fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
			return false;
		}
		else 
		{
			//fprintf(stderr, "Database was opened successfully\n");
			return true;
		}

		return true;
	}

	b8 Database::Disconnect()
	{
		if (rc)
		{
			sqlite3_close(db);
			//MD_LOG("Database was not opened!");
			return true;
		}
		else
		{
			//MD_ERROR("Disconnected from database.");
			return false;
		}

		return true;
	}

	b8 Database::CreateTable(Audio::AudioProperties* item)
	{
		char *errMsg = 0;
		std::string sql;

		// ID(key), path UNIQUE, folder, name, ext, freq, bitrate, size, length
		sql = "CREATE TABLE [";
		sql += item->folder;
		sql += "](";
		sql += "ID INTEGER PRIMARY KEY NOT NULL UNIQUE,";
		sql += "PATH TEXT NOT NULL UNIQUE,";
		sql += "FOLDER TEXT NOT NULL,";
		sql += "NAME TEXT NOT NULL,";
		sql += "EXT TEXT NOT NULL,";
		sql += "FREQ REAL,";
		sql += "BITRATE REAL,";
		sql += "SIZE REAL NOT NULL,";
		sql += "LENGTH REAL NOT NULL";
		sql += ");";
		std::cout << sql << std::endl;

		rc = sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg);


		if (rc != SQLITE_OK) {
			//fprintf(stderr, "SQL error: %s\n", errMsg);
			sqlite3_free(errMsg);
			return false;
		}
		else {
			//fprintf(stdout, "Table created successfully\n");
			return true;
		}

		return true;
	}

	b8 Database::Insert(Audio::AudioProperties* item)
	{
		char *errMsg = 0;
		std::string sql;

	/*	sql = "INSERT INTO [";
		sql += utf16_to_utf8(item->folder);
		sql += "] (ID, PATH, FOLDER, NAME, EXT, FREQ, BITRATE, SIZE, LENGTH) ";
		sql += "VALUES (";
		sql += std::to_string(item->id)				+ ", ";
		sql += "'" + utf16_to_utf8(item->path)		+ "', ";
		sql += "'" + utf16_to_utf8(item->folder)	+ "', ";
		sql += "'" + utf16_to_utf8(item->title)		+ "', ";
		sql += "'" + utf16_to_utf8(item->info.ext)	+ "', ";
		sql += std::to_string(item->info.freq)		+ ", ";
		sql += std::to_string(item->info.bitrate)	+ ", ";
		sql += std::to_string(item->info.size)		+ ", ";
		sql += std::to_string(item->info.length);
		sql += "); ";*/

		rc = sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg);

		if (rc != SQLITE_OK) 
		{
			//fprintf(stderr, "SQL error: %s\n", errMsg);
			sqlite3_free(errMsg);
			return false;
		}
		else 
		{
			//fprintf(stdout, "Records created successfully\n");
			return true;
		}

	}

	b8 Database::Select(std::string table)
	{
		char *errMsg = 0;
		std::string sql;

		sql = "SELECT * from [";
		sql += table;
		sql += "]";

		rc = sqlite3_exec(db, sql.c_str(), callback, 0, &errMsg);

		if (rc != SQLITE_OK) 
		{
			//fprintf(stderr, "SQL error: %s\n", &errMsg);
			sqlite3_free(errMsg);
			return false;
		}
		else 
		{
			//fprintf(stdout, "Operation done successfully\n");
			return true;
		}

		return true;
	}

	void Database::Delete(std::string item)
	{

	}

	static int Database::callback(void *NotUsed, int argc, char **argv, char **azColName) {
		int i;
		for (i = 0; i<argc; i++) {
			printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		}
		printf("\n");
		return 0;
	}

}