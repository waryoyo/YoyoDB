#pragma once

#include <string>
#include <map>

#include<nlohmann/json.hpp>

#include <storage.hpp>

class Database {

public:
	Database(const std::string& filename);

	bool createDocument(nlohmann::json& document);
	nlohmann::json readDocument(const long long& id);
	bool updateDocument(const long long& id, nlohmann::json& document);
	bool deleteDocument(const long long& id);


private:
	std::string m_filename;
	long long m_counter;
	std::map<std::string, nlohmann::json> m_documentCache;
	Storage m_storage;

	long long generateId();

	bool saveCache();
	bool loadCache();

	bool loadCounter();
	bool saveCounter();


};
