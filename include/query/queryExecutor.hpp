#pragma once

#include <vector>

#include <nlohmann/json.hpp>

#include <query/queryParser.hpp>
#include <query/condition.hpp>
#include <database.hpp>


using json = nlohmann::json;

class QueryExecutor {
public:
	std::vector<json> execute(const QueryNode* root, Database& database);
private:
	bool evaluateNode(const QueryNode* node, const json& document);
	bool evaluateLogic(const LogicalNode* node, const json& document);
	bool evaluateCondition(const ConditionalNode* node, const json& document);

	bool canUseIndex(const ConditionalNode* node);

};