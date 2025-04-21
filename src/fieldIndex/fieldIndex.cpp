#include <fieldIndex/FieldIndex.hpp>

FieldIndex::FieldIndex(const std::string& fieldName, const std::string& filename)
    : BaseFieldIndex(fieldName, false, filename)
{
    loadIndex();
}

std::vector<uint64_t> FieldIndex::get(const json& entry) const
{
    std::vector<uint64_t> results;

    auto range = m_indexMap.equal_range(entry);
    for (auto it = range.first; it != range.second; ++it) {
        results.push_back(it->second);
    }

    return results;
}

bool FieldIndex::has(const json& entry) const
{
    return m_indexMap.find(entry) != m_indexMap.end();
}

void FieldIndex::add(const json& entry, uint64_t id)
{
    m_indexMap.emplace(entry, id);

    json j = { {"key", entry}, {"id", id} };
    m_indexStream << j.dump() << '\n';
    m_indexStream.flush();
}

void FieldIndex::remove(const json& entry)
{
    auto range = m_indexMap.equal_range(entry);
    if (range.first == range.second) return;

    for (auto it = range.first; it != range.second; ++it) {
        json tombstone = { {"key", entry}, {"id", nullptr} };
        m_indexStream << tombstone.dump() << '\n';
    }
    m_indexStream.flush();

    m_indexMap.erase(entry);
}

void FieldIndex::remove(const json& entry, uint64_t id)
{
    auto range = m_indexMap.equal_range(entry);
    for (auto it = range.first; it != range.second; ) {
        if (it->second == id) {
            json tombstone = { {"key", entry}, {"id", nullptr} };
            m_indexStream << tombstone.dump() << '\n';
            m_indexStream.flush();

            it = m_indexMap.erase(it);
        }
        else
            it++;
       
    }
}

void FieldIndex::update(const json& entry, uint64_t id)
{
    remove(entry, id);
    add(entry, id);
}


void FieldIndex::compact()
{
    std::ofstream out(m_filename + ".tmp");

    for (const auto& [key, id] : m_indexMap) {
        json j = { {"key", key}, {"id", id} };
        out << j.dump() << '\n';
    }

    out.close();
    std::remove(m_filename.c_str());
    std::rename((m_filename + ".tmp").c_str(), m_filename.c_str());

    if (m_indexStream.is_open()) m_indexStream.close();
    m_indexStream.open(m_filename, std::ios::app);
}

void FieldIndex::loadIndex()
{
    m_indexStream.seekg(0, std::ios::end);
    int length = m_indexStream.tellg();
    if (length <= 0) {
        m_indexStream.clear();
        return;
    }

    m_isNewIndex = false;
    m_indexStream.seekg(0);

    std::string line;
    while (std::getline(m_indexStream, line)) {
        if (line.empty()) continue;

        json j = json::parse(line);
        if (j.contains("key") && j.contains("id")) {
            if (!j["id"].is_null()) {
                m_indexMap.emplace(j["key"], j["id"]);
            }
            else {
                m_indexMap.erase(j["key"]);
            }
        }
    }

    m_indexStream.clear();
    m_indexStream.seekp(0, std::ios::end);
}