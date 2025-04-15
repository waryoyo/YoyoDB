#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <list>

#include <nlohmann/json.hpp>

#include <database.hpp>

using json = nlohmann::json;

class DatabaseSystem {
	DatabaseSystem(int limit);

public:
	json findOne(uint64_t id, const std::string& databaseName);
	bool insertOne(const json& document, const std::string& databaseName);
	bool deleteOne(uint64_t id, const std::string& databaseName);
	bool updateOne(uint64_t id, const json& document, const std::string& databaseName);

private:
	std::unordered_map<std::string, Database> m_databasesLoaded;
	std::list<std::string> m_lru;
	int m_lruLimit;

	void loadDatabase(const std::string& databaseName);

};