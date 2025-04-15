#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include <fstream>
#include <vector>
#include <iostream>

#include <fieldIndex/baseFieldIndex.hpp>
#include <indexEntry.hpp>

#include <nlohmann/json.hpp>

using json = nlohmann::json;


class UniqueFieldIndex : public BaseFieldIndex {
public:
	UniqueFieldIndex(const std::string& fieldName, const std::string& filename);
	~UniqueFieldIndex() override = default;


	bool has(const IndexEntry& entry) const override;
	void add(const IndexEntry& entry, uint64_t id) override;
	void remove(const IndexEntry& entry) override;
	void remove(const IndexEntry& entry, uint64_t id) override;
	void update(const IndexEntry& entry, uint64_t id) override;

private:
	std::unordered_map<IndexEntry, uint64_t> m_indexMap;

	void loadIndex();
};