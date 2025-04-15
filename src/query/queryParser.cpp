#include <query/queryParser.hpp>

std::unique_ptr<QueryNode> QueryParser::parse(const json& query)
{
	if (query.is_object() && query.contains("field") && query.contains("operator") && query.contains("value")) {
		ConditionalNodeType type = ConditionalNodeType::EQUAL;

		if (query["operator"] == "eq")
			type = ConditionalNodeType::EQUAL;
		else if (query["operator"] == "neq")
			type = ConditionalNodeType::NOT_EQUAL;

		auto conditionNode = std::make_unique<ConditionalNode>(type, query["field"], query["value"]);
		return conditionNode;
	}
	else if (query.is_object() && query.contains("and") && query["and"].is_array()) {
		auto andNode = std::make_unique<LogicalNode>(LogicalNodeType::AND);
		for (const auto& subQuery : query["and"]) {
			andNode->children.push_back(parse(subQuery));
		}
		return andNode;
	}
	else if (query.is_object() && query.contains("or") && query["or"].is_array()) {
		 auto orNode = std::make_unique<LogicalNode>(LogicalNodeType::OR);
		 for (const auto& subQuery : query["or"]) {
			 orNode->children.push_back(parse(subQuery));
		 }
		 return orNode;
	}

	return nullptr;
}