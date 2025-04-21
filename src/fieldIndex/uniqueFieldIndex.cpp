#include <fieldIndex/uniqueFieldIndex.hpp>

UniqueFieldIndex::UniqueFieldIndex(const std::string& fieldName, const std::string& filename)
	: BaseFieldIndex(fieldName, true, filename)
{
	loadIndex();
}


std::vector<uint64_t> UniqueFieldIndex::get(const json& entry) const
{
	if (!has(entry))
		return {};

	return { m_indexMap.at(entry) };
}

bool UniqueFieldIndex::has(const json& entry) const
{
	return m_indexMap.contains(entry);
}

void UniqueFieldIndex::add(const json& entry, uint64_t id)
{
	if (has(entry))
		return;

	json j = { {"key", entry}, {"id", id} };
	m_indexStream << j.dump() << '\n';
	m_indexStream.flush();

	m_indexMap[entry] = id;
}
void UniqueFieldIndex::remove(const json& entry)
{
	if (!has(entry))
		return;

	json tombstone = { {"key", entry}, {"id", nullptr} };
	m_indexStream << tombstone.dump() << '\n';
	m_indexStream.flush();
}
void UniqueFieldIndex::remove(const json& entry, uint64_t id)
{
	remove(entry);
}

void UniqueFieldIndex::update(const json& entry, uint64_t id)
{
	if (!has(entry))
		return;

	add(entry, id);
}

void UniqueFieldIndex::compact()
{
	std::ofstream out(m_filename + ".tmp");

	for (const auto& [key, id] : m_indexMap) {
		json j = { {"key", key}, {"id", id} };
		out << j.dump() << '\n';
	}

	out.close();
	std::remove(m_filename.c_str());
	std::rename((m_filename + ".tmp").c_str(), m_filename.c_str());

	if (m_indexStream.is_open()) m_indexStream.close();
	m_indexStream.open(m_filename, std::ios::app);
}

void UniqueFieldIndex::loadIndex()
{
	m_indexStream.seekg(0, std::ios::end);
	int length = m_indexStream.tellg();
	if (length <= 0) {
		m_indexStream.clear();
		return;
	}

	m_isNewIndex = false;
	m_indexStream.seekg(0);

	std::string line;
	while (std::getline(m_indexStream, line)) {
		if (line.empty()) continue;

		json j = json::parse(line);
		if (j.contains("key") && j.contains("id") && !j["id"].is_null()) {
			m_indexMap[j["key"]] = j["id"];
		}
		else if (j.contains("key") && j["id"].is_null()) {
			m_indexMap.erase(j["key"]);
		}
	}

	m_indexStream.clear();

}