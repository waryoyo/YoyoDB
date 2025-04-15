#include "query/condition.hpp"

ComparatorCondition::ComparatorCondition(const std::string& field, const json& value)
	: m_field(field), m_value(value)
{
}

bool EqualCondition::evaluate(const json& document) const
{
	if (!document.contains(m_field))
		return false;

	return document[m_field] == m_value;
}


bool NotEqualCondition::evaluate(const json& document) const
{
	if (!document.contains(m_field))
		return true;

	return document[m_field] != m_value;
}

bool GreaterCondition::evaluate(const json& document) const
{
	return document[m_field] > m_value;
}

bool SmallerCondition::evaluate(const json& document) const
{
	return document[m_field] < m_value;

}

bool InCondition::evaluate(const json& document) const
{
	if (document[m_field] != NULL and document[m_field].is_array()) {
		auto result = std::find(document[m_field].begin(), document[m_field].end(), m_value);
		if (result != document[m_field].end())
			return true;
	}

	return false;
}

bool LikeCondition::evaluate(const json& document) const
{
	if (document[m_field] == NULL or not document[m_field].is_string())
		return true;

	if (m_value == NULL or not m_value.is_string())
		return true;

	// Use asserts here instead
	if (document[m_field] != NULL and document[m_field].is_string()) {
		std::string item = document[m_field];
		std::string value = m_value;

		auto result = item.find(value);

		if (result != std::string::npos)
			return true;

	}

	return false;
}
