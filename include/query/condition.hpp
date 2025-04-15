#pragma once

#include <nlohmann/json.hpp>

using json = nlohmann::json;

class Condition {
public:
	virtual bool evaluate(const json& document) const = 0;
};

class ComparatorCondition : public Condition {
public:
    ComparatorCondition(const std::string& field, const json& value);
    virtual bool evaluate(const json& document) const = 0;
protected:
    std::string m_field;
    json m_value;
};

class EqualCondition : public ComparatorCondition {
public:
    EqualCondition(const std::string& field, const json& value)
        : ComparatorCondition(field, value) {}
    bool evaluate(const json& document) const;
};

class NotEqualCondition : public ComparatorCondition {
public:
    NotEqualCondition(const std::string& field, const json& value)
        : ComparatorCondition(field, value) {}
    bool evaluate(const json& document) const;
};

class GreaterCondition : public ComparatorCondition {
public:
    bool evaluate(const json& document) const;
};

class SmallerCondition : public ComparatorCondition {
public:
    bool evaluate(const json& document) const;
};

class InCondition : public ComparatorCondition {
public:
    bool evaluate(const json& document) const;
};

class LikeCondition : public ComparatorCondition {
public:
    bool evaluate(const json& document) const;
};