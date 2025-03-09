#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

class Storage
{
public:
	Storage(const std::string& filename);
	
	class JsonLineIterator {
	public:
		JsonLineIterator(const std::string& filename);
		~JsonLineIterator();

		bool hasNext();
		nlohmann::json next();
	private:
		std::string m_filename;
		std::ifstream m_file;
		std::string m_currentLine;

		bool m_hasNext = false;
		void advance();
		
	};

	bool writeJObject(const nlohmann::json& jObject);
	JsonLineIterator getIterator();
	//std::vector<nlohmann::json> readAll();

private:
	std::string m_filename;

};
