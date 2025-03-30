#pragma once

#include <string>
#include <map>
#include <vector>

#include <nlohmann/json.hpp>

#include <storage.hpp>
#include <cacheManager.hpp>

class Database {

public:
	Database(const std::string& filename);

	bool createDocument(nlohmann::json& document);
	nlohmann::json readDocument(uint64_t id);
	std::vector<nlohmann::json> readAllDocuments();


	bool updateDocument(uint64_t id, nlohmann::json& document);
	bool deleteDocument(uint64_t id);


private:
	std::string m_filename;
	uint64_t m_counter;
	//std::map<std::string, nlohmann::json> m_documentCache;
	CacheManager m_cacheManager;
	Storage m_storage;

	uint64_t generateId();

	bool saveCache();
	bool loadCache();

	bool loadCounter();
	bool saveCounter() const;


};
