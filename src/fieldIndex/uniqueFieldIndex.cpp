#include <fieldIndex/uniqueFieldIndex.hpp>

UniqueFieldIndex::UniqueFieldIndex(const std::string& fieldName, const std::string& filename)
	: BaseFieldIndex(fieldName, true, filename)
{
	loadIndex();
}


bool UniqueFieldIndex::has(const IndexEntry& entry) const
{
	return m_indexMap.contains(entry);
}

void UniqueFieldIndex::add(const IndexEntry& entry, uint64_t id)
{
	if (has(entry))
		return;

	IndexEntry entryCopy = entry;
	if (m_freeSlots.size() == 0) {
		m_indexStream.seekg(0, std::ios::end);
		int length = m_indexStream.tellg();

		if (length > 0)
			m_indexStream.seekp(length);
		else
			m_indexStream.seekp(0, std::ios::end);


		m_indexStream.write(reinterpret_cast<char*>(&id), sizeof(id));
		m_indexStream.write(reinterpret_cast<char*>(&entryCopy.offset), sizeof(entry.offset));
		m_indexStream.write(reinterpret_cast<char*>(&entryCopy.size), sizeof(entry.size));

		m_indexLocationsMap[id] = length;
	}
	else {
		uint32_t front = m_freeSlots.back();
		m_freeSlots.pop_back();

		uint32_t freeOffsetsSize = m_freeSlots.size();

		m_indexStream.seekp(front * (sizeof(uint64_t) + sizeof(uint32_t)) + sizeof(uint32_t));
		m_indexStream.write(reinterpret_cast<char*>(&id), sizeof(id));
		m_indexStream.write(reinterpret_cast<char*>(&entryCopy.offset), sizeof(entry.offset));
		m_indexStream.write(reinterpret_cast<char*>(&entryCopy.size), sizeof(entry.size));

		m_indexLocationsMap[id] = front;
	}

	m_indexMap[entry] = id;
}
void UniqueFieldIndex::remove(const IndexEntry& entry)
{
	if (!has(entry))
		return;

	uint64_t zero = 0;

	auto id = m_indexMap[entry];

	m_indexStream.seekp(m_indexLocationsMap[id] * (sizeof(uint64_t) + sizeof(uint32_t) + sizeof(uint32_t)));
	m_indexStream.write(reinterpret_cast<char*>(&zero), sizeof(uint64_t));
	m_indexStream.write(reinterpret_cast<char*>(&zero), sizeof(uint32_t));
	m_indexStream.write(reinterpret_cast<char*>(&zero), sizeof(uint32_t));

	m_indexMap.erase(entry);
	m_freeSlots.push_back(m_indexLocationsMap[id]);
	m_indexLocationsMap.erase(id);


	m_indexMap.erase(entry);
}
void UniqueFieldIndex::remove(const IndexEntry& entry, uint64_t id)
{
	remove(entry);
}

void UniqueFieldIndex::update(const IndexEntry& entry, uint64_t id)
{
	if (!has(entry))
		return;

	IndexEntry entryCopy = entry;

	m_indexStream.seekp(m_indexLocationsMap[id] * (sizeof(uint64_t) + sizeof(uint32_t)) + sizeof(uint64_t));
	m_indexStream.write(reinterpret_cast<char*>(&entryCopy.offset), sizeof(entry.offset));
	m_indexStream.write(reinterpret_cast<char*>(&entryCopy.size), sizeof(entry.size));

	m_indexMap[entry] = id;
}

void UniqueFieldIndex::loadIndex()
{
	m_indexStream.seekg(0, std::ios::end);
	int length = m_indexStream.tellg();
	if (length <= 0) {
		m_indexStream.clear();
		return;
	}

	m_indexStream.seekg(0);

	uint32_t i = 0;
	while (true) {
		uint64_t id = 0;
		auto entry = IndexEntry();

		m_indexStream.read(reinterpret_cast<char*>(&id), sizeof(uint64_t));
		m_indexStream.read(reinterpret_cast<char*>(&entry.offset), sizeof(uint32_t));
		m_indexStream.read(reinterpret_cast<char*>(&entry.size), sizeof(uint32_t));

		if (m_indexStream.eof() || m_indexStream.fail()) break;

		m_indexMap[entry] = id;
		m_indexLocationsMap[id] = i;

		i++;
	}

	m_indexStream.clear();

}