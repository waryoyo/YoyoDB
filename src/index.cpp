#include "index.hpp"

Index::Index(const std::string& filename)
	: m_filename(filename)
{
	m_indexStream.open(m_filename, std::ios::binary | std::ios::ate | std::ios::out | std::ios::in);
	if (!m_indexStream.is_open()) {
		throw std::runtime_error("Error opening index file: " + m_filename);
	}
	loadIndex();
}

Index::~Index()
{
	m_indexStream.close();
}

void Index::setOffset(uint64_t id, uint32_t offset)
{
	if (m_indexMap.count(id) != 0) {
		m_indexStream.seekp(sizeof(uint32_t) + m_indexLocationsMap[id] * (sizeof(uint64_t) + sizeof(uint32_t)) + sizeof(uint64_t));
		m_indexStream.write(reinterpret_cast<char*>(&offset), sizeof(uint32_t));
	}
	else if (m_freeOffsets.size() == 0) {
		m_indexStream.seekg(0, std::ios::end);
		int length = m_indexStream.tellg();

		m_indexStream.seekg(length);
		m_indexStream.write(reinterpret_cast<char*>(&id), sizeof(uint64_t));
		m_indexStream.write(reinterpret_cast<char*>(&offset), sizeof(uint32_t));
	}
	else {
		uint32_t front = m_freeOffsets.back();
		m_freeOffsets.pop_back();

		m_indexStream.seekp(0);
		m_indexStream.write(reinterpret_cast<char*>(m_freeOffsets.size()), sizeof(uint32_t));

		m_indexStream.seekp(sizeof(uint32_t) + front * (sizeof(uint64_t) + sizeof(uint32_t)) + sizeof(uint32_t));
		m_indexStream.write(reinterpret_cast<char*>(&id), sizeof(uint64_t));
		m_indexStream.write(reinterpret_cast<char*>(&offset), sizeof(uint32_t));
	}

	m_indexMap[id] = offset;

	m_indexStream.seekp(0);
}

void Index::removeOffset(uint64_t id)
{
	if (m_indexMap.count(id) == 0)
		return;

	uint64_t zeroId = 0;
	uint32_t zeroOffset = 0;

	m_indexStream.seekp(sizeof(uint32_t) + m_indexLocationsMap[id] * (sizeof(uint64_t) + sizeof(uint32_t)));
	m_indexStream.write(reinterpret_cast<char*>(&zeroId), sizeof(uint64_t));
	m_indexStream.write(reinterpret_cast<char*>(&zeroOffset), sizeof(uint32_t));

	m_indexMap.erase(id);
	m_freeOffsets.push_back(m_indexLocationsMap[id]);
	m_indexLocationsMap.erase(id);

	m_indexStream.seekp(0);

	uint32_t freeOffsetSize = static_cast<uint32_t>(m_freeOffsets.size());
	m_indexStream.write(reinterpret_cast<char*>(&freeOffsetSize), sizeof(uint32_t));

}

uint32_t Index::getOffset(uint64_t id)
{
	if(m_indexMap.count(id) == 0)
		throw std::runtime_error("ID not found in index.");

	return m_indexMap[id];
}

void Index::loadIndex() {

	if (m_indexStream.peek() == std::ifstream::traits_type::eof()) {
		createIndex();
		return;
	}

	uint32_t freeOffsetsCount = 0;
	m_indexStream.read(reinterpret_cast<char*>(&freeOffsetsCount), sizeof(uint32_t));
	m_freeOffsets.reserve(std::max(freeOffsetsCount, 1u));

	m_indexStream.seekg(0, std::ios::end);
	int length = m_indexStream.tellg();

	std::vector<uint32_t> usedBlocks;
	usedBlocks.reserve(floor(length/2));
	m_indexStream.seekg(sizeof(uint32_t));

	uint32_t i = 0;
	while (m_indexStream.peek() != std::ifstream::traits_type::eof()) {
		uint64_t id = 0;
		uint32_t offset = 0;

		m_indexStream.read(reinterpret_cast<char*>(&id), sizeof(uint64_t));
		m_indexStream.read(reinterpret_cast<char*>(&offset), sizeof(uint32_t));

		if (id == 0 && offset == 0 && m_freeOffsets.size() <= freeOffsetsCount) {
			m_freeOffsets.push_back(i);
			continue;
		}

		usedBlocks.push_back(offset);

		m_indexMap[id] = offset;
		m_indexLocationsMap[id] = i;

		i++;
	}

	sort(usedBlocks.begin(), usedBlocks.end());
	for (size_t i = 1; i < usedBlocks.size(); i++) {
		uint32_t distance = usedBlocks[i] - usedBlocks[i - 1];
		if (distance > 1)
			m_emptyGaps[distance] = usedBlocks[i - 1];
	}

	m_indexStream.seekp(0);
}

void Index::createIndex()
{
	uint32_t zeroSize = 0;
	m_indexStream.write(reinterpret_cast<char*>(&zeroSize), sizeof(uint32_t));

	m_indexStream.seekp(0);
}


