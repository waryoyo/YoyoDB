#include "index.hpp"

Index::Index(const std::string& filename)
	: m_filename(filename + ".bin")
{
	std::ofstream(m_filename, std::ios::app | std::ios::binary).close();
	m_indexStream.open(m_filename, std::ios::binary | std::ios::out | std::ios::in | std::ios::ate);
	if (!m_indexStream.is_open()) {
		throw std::runtime_error("Error opening index file: " + m_filename);
	}
	loadIndex();
}

Index::~Index()
{
	m_indexStream.close();
}

bool Index::has(uint64_t id)
{
	return m_indexMap.contains(id);
}

void Index::setOffset(uint64_t id, uint32_t offset)
{
	if (m_indexMap.count(id) != 0) {
		m_indexStream.seekp(sizeof(uint32_t) + m_indexLocationsMap[id] * (sizeof(uint64_t) + sizeof(uint32_t)) + sizeof(uint64_t));
		m_indexStream.write(reinterpret_cast<char*>(&offset), sizeof(offset));
	}
	else if (m_freeOffsets.size() == 0) {
		m_indexStream.seekg(0, std::ios::end);
		int length = m_indexStream.tellg();

		if (length > 0)
			m_indexStream.seekp(length);
		else
			m_indexStream.seekp(0, std::ios::end);

		m_indexStream.write(reinterpret_cast<char*>(&id), sizeof(id));
		m_indexStream.write(reinterpret_cast<char*>(&offset), sizeof(offset));
	}
	else {
		uint32_t front = m_freeOffsets.back();
		m_freeOffsets.pop_back();

		uint32_t freeOffsetsSize = m_freeOffsets.size();

		m_indexStream.seekp(0);
		m_indexStream.write(reinterpret_cast<char*>(&freeOffsetsSize), sizeof(freeOffsetsSize));

		m_indexStream.seekp(sizeof(uint32_t) + front * (sizeof(uint64_t) + sizeof(uint32_t)) + sizeof(uint32_t));
		m_indexStream.write(reinterpret_cast<char*>(&id), sizeof(id));
		m_indexStream.write(reinterpret_cast<char*>(&offset), sizeof(offset));
	}

	m_indexMap[id] = offset;

}

void Index::removeOffset(uint64_t id, uint32_t size)
{
	if (m_indexMap.count(id) == 0)
		return;

	uint64_t zeroId = 0;
	uint32_t zeroOffset = 0;

	m_indexStream.seekp(sizeof(uint32_t) + m_indexLocationsMap[id] * (sizeof(uint64_t) + sizeof(uint32_t)));
	m_indexStream.write(reinterpret_cast<char*>(&zeroId), sizeof(zeroId));
	m_indexStream.write(reinterpret_cast<char*>(&zeroOffset), sizeof(zeroOffset));

	m_emptyGaps[size] = m_indexMap[id];
	m_indexMap.erase(id);
	m_freeOffsets.push_back(m_indexLocationsMap[id]);
	m_indexLocationsMap.erase(id);

	m_indexStream.seekp(0);
	uint32_t freeOffsetSize = static_cast<uint32_t>(m_freeOffsets.size());
	m_indexStream.write(reinterpret_cast<char*>(&freeOffsetSize), sizeof(freeOffsetSize));

}

uint32_t Index::getOffset(uint64_t id)
{
	if(m_indexMap.count(id) == 0)
		throw std::runtime_error("ID not found in index.");

	return m_indexMap[id];
}

std::optional<std::pair<uint32_t, uint32_t>> Index::getBestFitGap(uint32_t size)
{
	auto it = std::find_if(m_emptyGaps.begin(), m_emptyGaps.end(),
		[&](const auto& gap) { return gap.second >= size; });
	
	if (it != m_emptyGaps.end())
		return std::pair(it->first, it->second);

	return std::nullopt;
}

void Index::eraseGap(uint32_t size)
{
	if(m_emptyGaps.contains(size))
		m_emptyGaps.erase(size);
}

void Index::updateGap(uint32_t size, uint32_t offset)
{
	m_emptyGaps[size] = offset;
}

void Index::loadIndex() {

	m_indexStream.seekg(0, std::ios::end);
	int length = m_indexStream.tellg();
	if (length <= 0) {
		m_indexStream.clear();
		createIndex();
		return;
	}

	m_indexStream.seekg(0);
	uint32_t freeOffsetsCount = 0;
	m_indexStream.read(reinterpret_cast<char*>(&freeOffsetsCount), sizeof(uint32_t));
	m_freeOffsets.reserve(std::max(freeOffsetsCount, 1u));

	std::vector<uint32_t> usedBlocks;
	usedBlocks.reserve(floor(std::max(length, 2)/2));
	m_indexStream.seekg(sizeof(uint32_t));

	uint32_t i = 0;
	while (true) {
		uint64_t id = 0;
		uint32_t offset = 0;

		m_indexStream.read(reinterpret_cast<char*>(&id), sizeof(uint64_t));
		m_indexStream.read(reinterpret_cast<char*>(&offset), sizeof(uint32_t));

		if (m_indexStream.eof() || m_indexStream.fail()) break;

		if (id == 0 && offset == 0 && m_freeOffsets.size() <= freeOffsetsCount) {
			m_freeOffsets.push_back(i);
			continue;
		}

		usedBlocks.push_back(offset);

		m_indexMap[id] = offset;
		m_indexLocationsMap[id] = i;

		i++;
	}

	m_indexStream.clear();

	sort(usedBlocks.begin(), usedBlocks.end());
	for (size_t i = 1; i < usedBlocks.size(); i++) {
		uint32_t distance = usedBlocks[i] - usedBlocks[i - 1];
		if (distance > 1)
			m_emptyGaps[distance] = usedBlocks[i - 1];
	}

}

void Index::createIndex()
{
	m_indexStream.seekp(0, std::ios::end);
	uint32_t zeroSize = 0;
	m_indexStream.write(reinterpret_cast<char*>(&zeroSize), sizeof(zeroSize));
	//m_indexStream.flush();
}


