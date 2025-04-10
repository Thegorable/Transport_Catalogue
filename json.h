#pragma once
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <variant>

namespace json {

class Node;
using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

class ParsingError : public std::runtime_error {
public:
    using runtime_error::runtime_error;
};

struct NodePrinter {
    NodePrinter(std::ostream& o, int nested = 0);

    void operator () (std::nullptr_t);
    void operator () (int value);
    void operator () (double value);
    void operator () (const std::string& value);
    void operator () (bool value);
    void operator () (const Array& value);
    void operator () (const Dict& value);

private:
    std::ostream& out;
    int nested_ = 0;
};

using JsonValue = std::variant<std::nullptr_t, int, double, std::string, bool, Array, Dict>;

class Node {
public:
    Node() = default;
    Node(Array array);
    Node(Dict map);
    Node(int value);
    Node(double value);
    Node(const std::string& value);
    Node(const char* value);
    Node(bool value);
    Node(std::nullptr_t);

    bool IsInt() const;
    bool IsDouble() const;
    bool IsPureDouble() const;
    bool IsBool() const;
    bool IsString() const;
    bool IsNull() const;
    bool IsArray() const;
    bool IsMap() const;

    const Array& AsArray() const;
    const Dict& AsMap() const;
    bool AsBool() const;
    int AsInt() const;
    double AsDouble() const;
    const std::string& AsString() const;
    std::nullptr_t AsNull() const;

    const JsonValue& GetValue() const;

    bool operator ==(const Node& node) const;
    bool operator !=(const Node& node) const;

    void SetValue(const JsonValue& value);
    void SetValue(JsonValue&& value);

private:
    JsonValue value_;
};

class Document {
    public:
        explicit Document(Node root);
    
        const Node& GetRoot() const;
    
        bool operator ==(const Document& other) const;
    
    private:
        Node root_;
    };

Document Load(std::istream& input);

void PrintNode(const Document& doc, std::ostream& output);

}