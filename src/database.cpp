#include "database.hpp"

Database::Database(const std::string& filename)
	: m_filename(filename), m_storage(filename) {
	loadCounter();
}

bool Database::createDocument(nlohmann::json& document)
{
	document["id"] = generateId();
	bool result = m_storage.writeJObject(document);
	return result;
}

nlohmann::json Database::readDocument(const long long& id)
{
	auto iterator = m_storage.getIterator();

	while (iterator.hasNext()) {
		nlohmann::json jObject = iterator.next();
		if (not jObject.count("id"))
			continue;
		if (jObject["id"].get<long long>() == id)
			return jObject;
	}
	
	return {};
}

bool Database::updateDocument(const long long& id, nlohmann::json& document)
{
	return false;
}

bool Database::deleteDocument(const long long& id)
{
	return false;
}

long long Database::generateId()
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
	std::ifstream file = std::ifstream(m_filename + ".txt");
	m_counter = 0;
	file >> m_counter;
	file.close();
	return true;
}

bool Database::saveCounter()
{
	std::ofstream file = std::ofstream(m_filename + ".txt");
	file << m_counter;
	file.close();
	return true;
}
