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

	std::vector<uint64_t> getAllIds();

	bool createDocument(nlohmann::json& document);
	nlohmann::json readDocument(uint64_t id);

	std::vector<nlohmann::json> readAllDocuments();

	bool updateDocument(uint64_t id, nlohmann::json& document);
	bool deleteDocument(uint64_t id);

	bool createFieldIndex(const std::string& fieldName, bool unique = false);
	std::vector<nlohmann::json> readFromIndex(const std::string& fieldName, const nlohmann::json& value);
	bool hasIndex(const std::string& fieldName);



private:
	std::string m_filename;
	uint64_t m_counter;
	CacheManager m_cacheManager;
	Storage m_storage;
	StorageIndex m_index;

	std::unordered_map<std::string, std::unique_ptr<BaseFieldIndex>> m_fieldIndicesMap;
	//std::unordered_map<std::string, std::vector<std::unique_ptr<BaseFieldIndex>>> m_fieldIndicesMap;
	uint64_t generateId();

	bool loadCounter();
	bool saveCounter() const;

	void loadFieldIndices();
	void populateFieldIndex(BaseFieldIndex* index, std::vector<size_t> ids);


};
