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

	std::vector<uint64_t> get(const json& entry) const override;
	bool has(const json& entry) const override;
	void add(const json& entry, uint64_t id) override;
	void remove(const json& entry) override;
	void remove(const json& entry, uint64_t id) override;
	void update(const json& entry, uint64_t id) override;

	void compact() override;


private:
	std::unordered_map<json, uint64_t> m_indexMap;
	void loadIndex();
};