#include<unordered_map>
#include<nlohmann/json.hpp>
#include<queue>
#include<list>
#include<utility>

using json = nlohmann::json;


class CacheManager {
public:
	CacheManager();
	CacheManager(size_t capacity);

	std::optional<json> get(const long long& key);
	void put(const long long& key, json document);
	void remove(const long long& key);
	void clear();

private:
	std::unordered_map<long long, std::pair<std::list<long long>::iterator, json>> m_map;
	std::list<long long> m_accessOrder;
	size_t m_capacity;
};