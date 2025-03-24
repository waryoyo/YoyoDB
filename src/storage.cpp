#include "storage.hpp"

Storage::Storage(const std::string& filename)
	: m_filename(filename), m_index(filename) {

}

bool Storage::writeJObject(const nlohmann::json& jObject)
{
	std::string dump = jObject.dump();
	uint32_t size = static_cast<uint32_t>(dump.length());
	std::ofstream file = std::ofstream(m_filename, std::ios::app | std::ios::binary);

	file.write(reinterpret_cast<char*>(&size), sizeof(size));
	file.write(dump.data(), dump.length());
	file.close();
		
	return true;
}

bool Storage::updateJObject(uint64_t id, const nlohmann::json& jObject)
{
	std::ofstream fileTemp = std::ofstream(m_filename + ".temp");
	JsonLineIterator iterator(m_filename);
	while (iterator.hasNext()) {
		nlohmann::json currentJObject = iterator.next();
		if (currentJObject["id"].get<uint64_t>() == id) {
			fileTemp << jObject.dump() << std::endl;
		}
		else {
			fileTemp << currentJObject.dump() << std::endl;
		}
	}
	iterator.close();
	fileTemp.close();

	std::filesystem::remove(m_filename);
	std::filesystem::rename(m_filename + ".temp", m_filename);

	return true;
}

bool Storage::deleteJObject(uint64_t id)
{
	std::ofstream fileTemp = std::ofstream(m_filename + ".temp");
	JsonLineIterator iterator(m_filename);
	while (iterator.hasNext()) {
		nlohmann::json currentJObject = iterator.next();
		if (currentJObject["id"].get<uint64_t>() != id) {
			fileTemp << currentJObject.dump() << std::endl;
		}
	}

	iterator.close();
	fileTemp.close();

	std::filesystem::remove(m_filename);
	std::filesystem::rename(m_filename + ".temp", m_filename);

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
	close();
}

bool Storage::JsonLineIterator::hasNext() const
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

void Storage::JsonLineIterator::close()
{
	m_file.close();
}

void Storage::JsonLineIterator::advance()
{
	if (std::getline(m_file, m_currentLine)) 
		m_hasNext = true;
	else
		m_hasNext = false;
}
