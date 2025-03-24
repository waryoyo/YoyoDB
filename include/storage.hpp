#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include <index.hpp>

using json = nlohmann::json;

class Storage
{
public:
	Storage(const std::string& filename);
	~Storage();

	class JsonLineIterator { // will be removed
	public:
		JsonLineIterator(const std::string& filename);
		~JsonLineIterator();

		bool hasNext() const;
		nlohmann::json next();
		void close();
	private:
		std::string m_filename;
		std::ifstream m_file;
		std::string m_currentLine;

		bool m_hasNext = false;
		void advance();
		
	};

	json getJObject(uint64_t id);
	bool writeJObject(const nlohmann::json& jObject);
	bool updateJObject(uint64_t id, const nlohmann::json& jObject);
	bool deleteJObject(uint64_t id);

	JsonLineIterator getIterator(); // to be removed
	//std::vector<nlohmann::json> readAll();

private:
	std::string m_filename;
	std::fstream m_storageStream;
	Index m_index;

};
