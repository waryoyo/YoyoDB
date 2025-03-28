#include "database.hpp"

Database::Database(const std::string& filename)
	: m_filename(filename + ".txt"), m_storage(filename) {
	std::ofstream(m_filename, std::ios::app | std::ios::binary).close();
	loadCounter();
}

bool Database::createDocument(nlohmann::json& document)
{
	document["id"] = generateId();
	bool result = m_storage.writeJObject(document);
	m_cacheManager.put(document["id"], document);
	return result;
}

nlohmann::json Database::readDocument(uint64_t id)
{
	auto item = m_cacheManager.get(id);
	if (item.has_value())
		return item.value();


	auto jObject = m_storage.getJObject(id);
	if (jObject.contains("id"))
		m_cacheManager.put(jObject["id"], jObject);

	return jObject;

}

bool Database::updateDocument(uint64_t id, nlohmann::json& document)
{
	document["id"] = id;
	m_storage.updateJObject(id, document);
	m_cacheManager.put(id, document);
	return true;
}

bool Database::deleteDocument(uint64_t id)
{
	m_storage.deleteJObject(id);
	m_cacheManager.remove(id);
	return true;
}

uint64_t Database::generateId()
{
	m_counter++;
	saveCounter();
	return m_counter;
}

bool Database::saveCache()
{
	return false;
}

bool Database::loadCache()
{
	return false;
}

bool Database::loadCounter()
{
	std::ifstream file = std::ifstream(m_filename);
	m_counter = 0;
	file >> m_counter;
	file.close();
	return true;
}

bool Database::saveCounter() const
{
	std::ofstream file = std::ofstream(m_filename);
	file << m_counter;
	file.close();
	return true;
}
