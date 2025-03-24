#include "storage.hpp"

// TODO: update gap checking to account for json object being smallest possible size which is 2.

Storage::Storage(const std::string& filename)
	: m_filename(filename + ".jsonl"), m_index(filename) {
	std::ofstream(m_filename, std::ios::app | std::ios::binary).close();
	m_storageStream.open(m_filename, std::ios::binary | std::ios::ate | std::ios::out | std::ios::in);
}

Storage::~Storage() {
	m_storageStream.close();
}

json Storage::getJObject(uint64_t id)
{
	if (!m_index.has(id))
		return {};

	uint32_t offset = m_index.getOffset(id);
	m_storageStream.seekg(offset);

	uint32_t size = 0;
	m_storageStream.read(reinterpret_cast<char*>(&size), sizeof(size));

	std::string line(size, '\0');
	m_storageStream.read(&line[0], size);

	nlohmann::json jObject = nlohmann::json::parse(line);

	return jObject;
}

bool Storage::writeJObject(const nlohmann::json& jObject)
{
	std::string dump = jObject.dump();
	uint32_t size = static_cast<uint32_t>(dump.length());
	uint64_t id = jObject["id"].get<uint64_t>();

	auto bestFit = m_index.getBestFitGap(size);

	if (bestFit.has_value()) {
		m_index.eraseGap(bestFit.value().first);

		uint32_t remaining = bestFit.value().first - size;
		if (remaining > 0) 
			m_index.updateGap(remaining, bestFit.value().second + size);

		m_storageStream.seekp(bestFit.value().second);
	}
	else {
		m_storageStream.seekp(0, std::ios::end);
	}

	m_index.setOffset(id, m_storageStream.tellg());

	m_storageStream.write(reinterpret_cast<char*>(&size), sizeof(size));
	m_storageStream.write(dump.data(), dump.length());	

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
	uint32_t offset = m_index.getOffset(id);
	m_storageStream.seekg(offset);

	uint32_t size = 0;
	m_storageStream.read(reinterpret_cast<char*>(&size), sizeof(size));

	m_index.removeOffset(id, size);

	return true;
	/*std::ofstream fileTemp = std::ofstream(m_filename + ".temp");
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

	return true;*/
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
