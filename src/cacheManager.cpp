#include "cacheManager.hpp"

CacheManager::CacheManager()
	: m_capacity(100) {

}

CacheManager::CacheManager(size_t capacity)
	: m_capacity(capacity) {
}

std::optional<json> CacheManager::get(const long long& key)
{
	if (m_map.count(key) != 0) {
		m_accessOrder.erase(m_map[key].first);
		m_accessOrder.push_front(key);
		m_map[key].first = m_accessOrder.begin();
		return m_map[key].second;
	}

	return std::nullopt;
}

void CacheManager::put(const long long& key, json document)
{	
	if (m_map.count(key) != 0) {
		m_accessOrder.erase(m_map[key].first);
	}

	// Later on make sure this handles multiple threads so removes all extra before adding and etc...
	if (m_accessOrder.size() == m_capacity) {
		m_map.erase(m_accessOrder.back());
		m_accessOrder.pop_back();
	}

	m_accessOrder.push_front(key);
	m_map[key] = { m_accessOrder.begin(), document };

}

void CacheManager::remove(const long long& key)
{
	if (m_map.count(key) != 0) {
		m_accessOrder.erase(m_map[key].first);
		m_map.erase(key);
	}
}

void CacheManager::clear()
{
	m_map.clear();
	m_accessOrder.clear();
}
