#include "databaseSystem.hpp"

json DatabaseSystem::findOneId(uint64_t id, const std::string& databaseName)
{
    if (m_databasesLoaded.count(databaseName) == 0)
        loadDatabase(databaseName);

    Database db = m_databasesLoaded[databaseName];
    return db.readDocument(id);
}

bool DatabaseSystem::insertOne(const json& document, const std::string& databaseName) {
    if (m_databasesLoaded.count(databaseName) == 0)
        loadDatabase(databaseName);

    Database db = m_databasesLoaded[databaseName];
    db.createDocument(document);
    return true;
}

bool DatabaseSystem::deleteOne(uint64_t id, const std::string& databaseName)
{
    if (m_databasesLoaded.count(databaseName) == 0)
        loadDatabase(databaseName);

    Database db = m_databasesLoaded[databaseName];
    db.deleteDocument(id);
    return true;
}

bool DatabaseSystem::updateOne(uint64_t id, const json& document, const std::string& databaseName)
{
    if (m_databasesLoaded.count(databaseName) == 0)
        loadDatabase(databaseName);

    Database db = m_databasesLoaded[databaseName];
    db.updateDocument(id, document);
    return true;
}

void DatabaseSystem::loadDatabase(const std::string& databaseName)
{
    if (m_databasesLoaded.size() == m_lruLimit) {
        std::string last = m_lru.back();
        m_databasesLoaded.erase(last);
        m_lru.pop_back();
    }

    m_databasesLoaded[databaseName] = Database(databaseName);
    m_lru.push_back(databaseName);
}
