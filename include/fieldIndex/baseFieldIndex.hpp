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
	bool isNew() const;

	virtual void loadIndex() = 0;

	virtual std::vector<uint64_t> get(const json& entry) const = 0;
	virtual bool has(const json& entry) const = 0;
	virtual void add(const json& entry, uint64_t id) = 0;
	virtual void update(const json& entry, uint64_t id) = 0;
	virtual void remove(const json& entry) = 0;
	virtual void remove(const json& entry, uint64_t id) = 0;

	virtual void compact() = 0;


protected:
	std::string m_fieldName;
	bool m_unique;
	bool m_isNewIndex = true;

	std::string m_filename;
	std::fstream m_indexStream;
};