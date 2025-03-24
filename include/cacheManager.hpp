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

	std::optional<json> get(uint64_t key);
	void put(uint64_t key, json document);
	void remove(uint64_t key);
	void clear();

private:
	std::unordered_map<uint64_t, std::pair<std::list<uint64_t>::iterator, json>> m_map;
	std::list<uint64_t> m_accessOrder;
	size_t m_capacity;
};