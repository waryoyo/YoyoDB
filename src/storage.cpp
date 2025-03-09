#include "storage.hpp"

Storage::Storage(const std::string& filename)
	: m_filename(filename) {

}

bool Storage::writeJObject(const nlohmann::json& jObject)
{
	std::ofstream file = std::ofstream(m_filename, std::ios::app);
	file << jObject.dump() << std::endl;
	file.close();
		
	return true;
}

Storage::JsonLineIterator Storage::getIterator()
{
	return Storage::JsonLineIterator(m_filename);
}

Storage::JsonLineIterator::JsonLineIterator(const std::string& filename)
	: m_filename(filename), m_file(filename)
{
	advance();
}

Storage::JsonLineIterator::~JsonLineIterator()
{
	if (m_file.is_open()) {
		m_file.close();
	}
}

bool Storage::JsonLineIterator::hasNext()
{
	return m_hasNext;
}

nlohmann::json Storage::JsonLineIterator::next()
{
	if (!m_hasNext)
		return {};
	
	nlohmann::json line = nlohmann::json::parse(m_currentLine);
	advance();
	return line;
}

void Storage::JsonLineIterator::advance()
{
	if (std::getline(m_file, m_currentLine)) 
		m_hasNext = true;
	else
		m_hasNext = false;
}
