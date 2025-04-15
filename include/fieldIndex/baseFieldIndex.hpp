#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <fstream>
#include <vector>
#include <iostream>

#include <indexEntry.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

class BaseFieldIndex {
public:

	BaseFieldIndex(const std::string& fieldName, bool unique, const std::string& filename);
	virtual ~BaseFieldIndex() = default;

	std::string getFieldName() const;
	bool isUnique() const;

	virtual void loadIndex() = 0;

	virtual bool has(const IndexEntry& entry) const = 0;
	virtual void add(const IndexEntry& entry, uint64_t id) = 0;
	virtual void update(const IndexEntry& entry, uint64_t id) = 0;
	virtual void remove(const IndexEntry& entry) = 0;
	virtual void remove(const IndexEntry& entry, uint64_t id) = 0;



protected:
	std::string m_fieldName;
	bool m_unique;

	std::unordered_map<uint64_t, uint32_t> m_indexLocationsMap; // <id, offset>
	std::list<uint32_t> m_freeSlots;
	std::string m_filename;
	std::fstream m_indexStream;
};