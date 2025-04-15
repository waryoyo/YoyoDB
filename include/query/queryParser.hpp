#pragma once

#include <string>
#include <list>
#include <memory>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

/*
Grammer:-

Query -> QueryExpression
QueryExpression -> Condition | Logic
Logic -> Logic | Condition | And | Or
Condition -> Eq | NEq
*/

enum class NodeType {
    LOGICAL,
    CONDITIONAL
};

enum class LogicalNodeType {
    AND,
    OR
};

enum class ConditionalNodeType {
    EQUAL,
    NOT_EQUAL,
    GREATER_THAN,
    LESS_THAN,
    IN,
    LIKE
};


    struct QueryNode {
        virtual ~QueryNode() = default;
        NodeType type;
    };

    struct LogicalNode : public QueryNode {
        LogicalNodeType logicalType;
        std::list<std::unique_ptr<QueryNode>> children;

        LogicalNode(LogicalNodeType type)
            : logicalType(type) {
            this->type = NodeType::LOGICAL;
        }

        void addChild(std::unique_ptr<QueryNode> child) {
            children.push_back(std::move(child));
        }
    };

    struct ConditionalNode : public QueryNode {
        ConditionalNodeType conditionType;
        std::string field;
        json value;

        ConditionalNode(ConditionalNodeType type, const std::string& field, const json& value)
            : conditionType(type), field(field), value(value) {
            this->type = NodeType::CONDITIONAL;
        }
    };


class QueryParser {
public:
    std::unique_ptr<QueryNode> parse(const json& query);
};