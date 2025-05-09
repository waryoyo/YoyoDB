#include "database.hpp"

Database::Database(const std::string& filename)
	: m_filename(filename + ".txt"), m_index(filename), m_storage(filename, m_index) {
	std::ofstream(m_filename, std::ios::app | std::ios::binary).close();
	loadCounter();
	loadFieldIndices();
}

std::vector<uint64_t> Database::getAllIds()
{
	return m_storage.getAllJObjectIds();
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

std::vector<nlohmann::json> Database::readAllDocuments()
{
	auto idsList = m_storage.getAllJObjectIds();
	std::vector<nlohmann::json> finalResult;
	finalResult.reserve(idsList.size());

	for (const auto& id : idsList) {
		auto item = m_cacheManager.get(id);
		if (item.has_value()) {
			finalResult.push_back(item.value());
			continue;
		}

		auto jObject = m_storage.getJObject(id);
		if (jObject.contains("id")) {
			finalResult.push_back(jObject);
			m_cacheManager.put(jObject["id"], jObject);
		}

	}

	return finalResult;
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

bool Database::createFieldIndex(const std::string& fieldName, bool unique)
{
	if (hasIndex(fieldName))
		return false;

	if (!fs::exists("fields") || !fs::is_directory("fields")) {
		fs::create_directory("fields");
	}

	auto idsList = getAllIds();
	auto index = std::make_unique<UniqueFieldIndex>(fieldName, fieldName + ".bin");
	populateFieldIndex(index.get(), idsList);

	m_fieldIndicesMap[fieldName] = std::move(index);
	return true;
}

bool Database::hasIndex(const std::string& fieldName)
{
	return m_fieldIndicesMap.contains(fieldName);
}

uint64_t Database::generateId()
{
	m_counter++;
	saveCounter();
	return m_counter;
}

std::vector<nlohmann::json> Database::readFromIndex(const std::string& fieldName, const nlohmann::json& value)
{
	std::vector<json> results;

	if (!hasIndex(fieldName))
		return results;

	auto& index = m_fieldIndicesMap[fieldName];
	if (index->has(value)) {
		auto ids = index->get(value);
		for (const auto& id : ids) {
			auto doc = readDocument(id);

			if (!doc.is_null()) 
				results.push_back(doc);
			
		}
	}
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

void Database::loadFieldIndices()
{
	if (fs::exists("fields") && fs::is_directory("fields")) {
		for (const auto& entry : fs::directory_iterator("fields")) {
			std::string pathName = entry.path().filename().string();
			std::string fieldName = pathName.substr(0, pathName.find("."));
			createFieldIndex(fieldName, true);
			/*auto index = std::make_unique<UniqueFieldIndex>(fieldName, pathName);

			if (index->isNew())
				populateFieldIndex(index.get(), idsList);
			

			m_fieldIndicesMap[entry.path().string()] = std::move(index);*/
		}
	}

}

void Database::populateFieldIndex(BaseFieldIndex* index, std::vector<size_t> ids)
{
	for (const auto& id : ids) {
		auto doc = readDocument(id);
		if (!doc.contains(index->getFieldName()))
			continue;
		index->add(doc[index->getFieldName()], id);
	}
}
