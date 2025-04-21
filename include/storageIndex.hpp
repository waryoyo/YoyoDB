#pragma once

#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>
#include <optional>
#include <list>
#include <filesystem>


#include <indexEntry.hpp>
#include <fieldIndex/baseFieldIndex.hpp>
#include <fieldIndex/uniqueFieldIndex.hpp>

namespace fs = std::filesystem;

class StorageIndex {
public:


	StorageIndex(const std::string& filename);
	~StorageIndex();

	bool has(uint64_t id);

	std::vector<uint64_t> listAllIDs();

	void setEntry(uint64_t id, IndexEntry& entry);
	void removeEntry(uint64_t id);
	IndexEntry getEntry(uint64_t id);

	std::optional<std::pair<uint32_t, uint32_t>> getBestFitGap(uint32_t size);
	void eraseGap(uint32_t size);
	void updateGap(uint32_t size, uint32_t offset);

private:
	static const int recordSize = sizeof(uint64_t) + sizeof(uint32_t);
	
	std::unordered_map<uint64_t, IndexEntry> m_indexMap; // <id, offset>
	std::unordered_map<uint32_t, uint32_t> m_indexLocationsMap; // <id, location_in_index_file>
	//std::vector<uint32_t> m_freeOffsets;
	std::list<uint32_t> m_freeOffsets;

	std::multimap<uint32_t, uint32_t> m_emptyGaps; // <size, startpos>

	std::string m_filename;
	std::fstream m_indexStream;

	void loadIndex();

};