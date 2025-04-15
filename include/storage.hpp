#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include <storageIndex.hpp>

using json = nlohmann::json;

class Storage
{
public:
	Storage(const std::string& filename);
	~Storage();

	std::vector<uint64_t> getAllJObjectIds();

	json getJObject(uint64_t id);
	bool writeJObject(const nlohmann::json& jObject);
	bool updateJObject(uint64_t id, const nlohmann::json& jObject);
	bool deleteJObject(uint64_t id);

	bool createFIndex(const std::string& field, bool unique);

	//std::vector<nlohmann::json> readAll();

private:
	std::string m_filename;
	std::fstream m_storageStream;
	StorageIndex m_index;

};
