#include "storageIndex.hpp"

StorageIndex::StorageIndex(const std::string& filename)
	: m_filename(filename + ".bin")
{
	std::ofstream(m_filename, std::ios::app | std::ios::binary).close();
	m_indexStream.open(m_filename, std::ios::binary | std::ios::out | std::ios::in | std::ios::ate);
	if (!m_indexStream.is_open()) {
		throw std::runtime_error("Error opening index file: " + m_filename);
	}
	loadIndex();
}

StorageIndex::~StorageIndex()
{
	m_indexStream.close();
}

bool StorageIndex::has(uint64_t id)
{
	return m_indexMap.contains(id);
}

std::vector<uint64_t> StorageIndex::listAllIDs()
{
	std::vector<uint64_t> documentIDs;
	documentIDs.reserve(m_indexMap.size());

	for (auto it = m_indexMap.begin(); it != m_indexMap.end(); ++it)
		documentIDs.push_back(it->first);
	
	return documentIDs;
}

void StorageIndex::setEntry(uint64_t id, IndexEntry& entry)
{
	if (m_indexMap.count(id) != 0) {
		m_indexStream.seekp(m_indexLocationsMap[id] * (sizeof(uint64_t) + sizeof(uint32_t)) + sizeof(uint64_t));
		m_indexStream.write(reinterpret_cast<char*>(&entry.offset), sizeof(entry.offset));
		m_indexStream.write(reinterpret_cast<char*>(&entry.size), sizeof(entry.size));
	}
	else if (m_freeOffsets.size() == 0) {
		m_indexStream.seekg(0, std::ios::end);
		int length = m_indexStream.tellg();

		if (length > 0)
			m_indexStream.seekp(length);
		else
			m_indexStream.seekp(0, std::ios::end);

		m_indexStream.write(reinterpret_cast<char*>(&id), sizeof(id));
		m_indexStream.write(reinterpret_cast<char*>(&entry.offset), sizeof(entry.offset));
		m_indexStream.write(reinterpret_cast<char*>(&entry.size), sizeof(entry.size));

		m_indexLocationsMap[id] = length;
	}
	else {
		uint32_t front = m_freeOffsets.back();
		m_freeOffsets.pop_back();

		uint32_t freeOffsetsSize = m_freeOffsets.size();

		m_indexStream.seekp(front * (sizeof(uint64_t) + sizeof(uint32_t)) + sizeof(uint32_t));
		m_indexStream.write(reinterpret_cast<char*>(&id), sizeof(id));
		m_indexStream.write(reinterpret_cast<char*>(&entry.offset), sizeof(entry.offset));
		m_indexStream.write(reinterpret_cast<char*>(&entry.size), sizeof(entry.size));

		m_indexLocationsMap[id] = front;
	}

	m_indexMap[id] = entry;

}

void StorageIndex::removeEntry(uint64_t id)
{
	if (m_indexMap.count(id) == 0)
		return;

	uint64_t zero = 0;

	m_indexStream.seekp(m_indexLocationsMap[id] * (sizeof(uint64_t) + sizeof(uint32_t) + sizeof(uint32_t)));
	m_indexStream.write(reinterpret_cast<char*>(&zero), sizeof(uint64_t));
	m_indexStream.write(reinterpret_cast<char*>(&zero), sizeof(uint32_t));
	m_indexStream.write(reinterpret_cast<char*>(&zero), sizeof(uint32_t));

	m_emptyGaps.insert({ m_indexMap[id].size, m_indexMap[id].offset });
	m_indexMap.erase(id);
	m_freeOffsets.push_back(m_indexLocationsMap[id]);
	m_indexLocationsMap.erase(id);

}

IndexEntry StorageIndex::getEntry(uint64_t id)
{
	if(m_indexMap.count(id) == 0)
		throw std::runtime_error("ID not found in index.");

	return m_indexMap[id];
}

std::optional<std::pair<uint32_t, uint32_t>> StorageIndex::getBestFitGap(uint32_t size)
{
	auto lower = m_emptyGaps.lower_bound(size);

	if (lower == m_emptyGaps.end()) {
		return std::nullopt;
	}

	return std::pair(lower->first, lower->second);
}

void StorageIndex::eraseGap(uint32_t size)
{
	if(m_emptyGaps.contains(size))
		m_emptyGaps.erase(size);
}

void StorageIndex::updateGap(uint32_t size, uint32_t offset)
{
	m_emptyGaps.insert({ size, offset });
}

void StorageIndex::loadIndex() {

	m_indexStream.seekg(0, std::ios::end);
	int length = m_indexStream.tellg();
	if (length <= 0) {
		m_indexStream.clear();
		return;
	}

	m_indexStream.seekg(0);

	std::vector<IndexEntry> usedBlocks;
	usedBlocks.reserve(floor(std::max(length/16, 2)));

	uint32_t i = 0;
	while (true) {
		uint64_t id = 0;
		auto entry = IndexEntry();

		m_indexStream.read(reinterpret_cast<char*>(&id), sizeof(uint64_t));
		m_indexStream.read(reinterpret_cast<char*>(&entry.offset), sizeof(uint32_t));
		m_indexStream.read(reinterpret_cast<char*>(&entry.size), sizeof(uint32_t));

		if (m_indexStream.eof() || m_indexStream.fail()) break;

		if (id == 0 && entry.offset == 0) {
			m_freeOffsets.push_back(i);
			continue;
		}

		usedBlocks.push_back(entry);

		m_indexMap[id] = entry;
		m_indexLocationsMap[id] = i;

		i++;
	}

	m_indexStream.clear();

	sort(usedBlocks.begin(), usedBlocks.end(), [](const IndexEntry& first, const IndexEntry& second) {
			return first.offset < second.offset;
	});
	for (size_t i = 1; i < usedBlocks.size(); i++) {
		uint32_t distance = usedBlocks[i - 1].offset + usedBlocks[i-1].size - usedBlocks[i].offset;
		if (distance > 1)
 			m_emptyGaps.insert({ distance, usedBlocks[i - 1].offset });
	}

}

