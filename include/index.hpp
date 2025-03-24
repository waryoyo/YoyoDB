#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>
#include <algorithm>

class Index {
public:
	Index(const std::string& filename);
	~Index();
	void setOffset(uint64_t id, uint32_t offset);
	void removeOffset(uint64_t id);
	uint32_t getOffset(uint64_t id);
private:
	static const int recordSize = sizeof(uint64_t) + sizeof(uint32_t);
	
	std::unordered_map<uint64_t, uint32_t> m_indexMap; // <id, offset>
	std::unordered_map<uint32_t, uint32_t> m_indexLocationsMap; // <id, location_in_index_file>
	std::vector<uint32_t> m_freeOffsets;

	std::map<uint32_t, uint32_t> m_emptyGaps; // <size, startpos>

	std::string m_filename;
	std::fstream m_indexStream;

	void loadIndex();
	void createIndex();
	


};