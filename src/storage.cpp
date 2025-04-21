#include "storage.hpp"

// TODO: update gap checking to account for json object being smallest possible size which is 2.

Storage::Storage(const std::string& filename, StorageIndex& index)
	: m_filename(filename + ".jsonl"), m_index(index) {
	std::ofstream(m_filename, std::ios::app | std::ios::binary).close();
	m_storageStream.open(m_filename, std::ios::binary | std::ios::ate | std::ios::out | std::ios::in);
}

Storage::~Storage() {
	m_storageStream.close();
}

std::vector<uint64_t> Storage::getAllJObjectIds()
{
	return m_index.listAllIDs();
}

json Storage::getJObject(uint64_t id)
{
	if (!m_index.has(id))
		return {};

	auto entry = m_index.getEntry(id);
	m_storageStream.seekg(entry.offset);

	std::string line(entry.size, '\0');
	m_storageStream.read(&line[0], entry.size);
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

	auto entry = IndexEntry(m_storageStream.tellg(), size);
	m_index.setEntry(id, entry);

	m_storageStream.write(dump.data(), dump.length());	
	return true;
}

bool Storage::updateJObject(uint64_t id, const nlohmann::json& jObject)
{
	// reimplement update later to be more better
	deleteJObject(id);
	writeJObject(jObject);
	return true;
}

bool Storage::deleteJObject(uint64_t id)
{
	m_index.removeEntry(id);
	return true;
}
