#include <query/queryExecutor.hpp>

std::vector<json> QueryExecutor::execute(const QueryNode* root, Database& database)
{
	std::vector<json> results;
	auto documents = database.readAllDocuments();

	for (const auto& document : documents) {
		if (root && evaluateNode(root, document)) {
			results.push_back(document);
		}
	}

	return results;
}

bool QueryExecutor::evaluateNode(const QueryNode* node, const json& document) {
	if (!node)
		return NULL;

	switch (node->type)
	{
	case NodeType::LOGICAL: {
		const LogicalNode* logicalNode = dynamic_cast<const LogicalNode*>(node);
		return evaluateLogic(logicalNode, document);
		break;
	}
	case NodeType::CONDITIONAL: {
		const ConditionalNode* conditonalNode = dynamic_cast<const ConditionalNode*>(node);
		return evaluateCondition(conditonalNode, document);
		break;
	}
	}
	return NULL;
}

bool QueryExecutor::evaluateLogic(const LogicalNode* node, const json& document)
{
	bool result = true;
	switch (node->logicalType)
	{
	case LogicalNodeType::AND:
		for (const auto& childNode : node->children) {
			result &= evaluateNode(childNode.get(), document);
			if (!result)
				break;
		}
		break;
	case LogicalNodeType::OR:
		result = false;
		for (const auto& childNode : node->children) {
			result |= evaluateNode(childNode.get(), document);
			if (result)
				break;
		}
		break;
	default:
		break;
	}

	return result;
}

bool QueryExecutor::evaluateCondition(const ConditionalNode* node, const json& document)
{
	std::unique_ptr<Condition> condition;
	switch (node->conditionType)
	{
	case ConditionalNodeType::EQUAL:
		condition = std::make_unique<EqualCondition>(node->field, node->value);
		break;
	case ConditionalNodeType::NOT_EQUAL:
		condition = std::make_unique<NotEqualCondition>(node->field, node->value);
		break;
	default:
		break;
	}
	return condition->evaluate(document);

}

bool QueryExecutor::canUseIndex(const ConditionalNode* node)
{
	if (node->field == "id")
		return true;

	return false;
}
