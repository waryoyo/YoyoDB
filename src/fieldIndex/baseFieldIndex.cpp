#include <fieldIndex/BaseFieldIndex.hpp>

BaseFieldIndex::BaseFieldIndex(const std::string& fieldName, bool unique, const std::string& filename)
	: m_fieldName(fieldName), m_unique(unique), m_filename(filename)
{
	std::ofstream(m_filename, std::ios::app | std::ios::binary).close();
	m_indexStream.open(m_filename, std::ios::binary | std::ios::out | std::ios::in | std::ios::ate);
	if (!m_indexStream.is_open()) {
		throw std::runtime_error("Error opening index file: " + m_filename);
	}
}

std::string BaseFieldIndex::getFieldName() const
{
	return m_fieldName;
}

bool BaseFieldIndex::isUnique() const
{
	return m_unique;
}
