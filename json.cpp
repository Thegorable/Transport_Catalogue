#include "json.h"

using namespace std;

namespace json {

namespace {

    Node LoadArray(istream& input);
    Node LoadDict(istream& input);
    Node LoadString(istream& input);
    Node LoadDigit(istream& input);
    Node LoadBool(istream& input);
    Node LoadNull(istream& input);
    
    void SkipControlChars(istream& input) {
        char ch;
        while (input.get(ch) && ch <= ' ');
        if (input) input.unget();
    }
    
    bool IsClearStream(istream& input) {
        while (input && isspace(input.peek())) {
            input.get();
        }
        return input.eof();
    }
    
    Node LoadNode(istream& input) {
        SkipControlChars(input);
        
        char c = input.peek();
    
        if (isdigit(c) || c == '-') {
            return LoadDigit(input);
        }
    
        switch (c)
        {
        case '[':
            input.get();
            return LoadArray(input);
            break;
        case '{':
            input.get();
            return LoadDict(input);
            break;
        case '"':
            input.get();
            return LoadString(input);
            break;
        case 'n':
            return LoadNull(input);
            break;
        case 'f': case 't':
            return LoadBool(input);
        default:
            throw ParsingError("The wrong start character is provided: "s + c);
            break;
        }
    
        return Node();
    }
    
    Node LoadArray(istream& input) {
        Array result;
        char c;
        input >> c;
    
        while (c != ']') {
            if (input.eof() && c != ']') {
                throw ParsingError ("Wrong Array parsing: the end bracket ] is missed"s);
            }
    
            if (c != ',') {
                input.unget();
            }
            result.push_back(LoadNode(input));    
            input >> c;
        }
    
        return Node(move(result));
    }
    
    Node LoadDict(istream& input) {
        Dict result;
        char c;
        input >> c;
    
        while (c != '}') {
            if (input.eof() && c != '}') {
                throw ParsingError ("Wrong Dict parsing: the end bracket } is missed"s);
            }
    
            if (c == ':' || c == ',') {
                input >> c;
            }
    
            string key = LoadString(input).AsString();
            input >> c;
            result.insert({move(key), LoadNode(input)});
            input >> c;
        }
    
        return Node(move(result));
    }
    
    // Node LoadDigit(istream& input) {
    //     int result_i;
    //     auto start_pos = input.tellg();      
    //     input >> result_i;
    
    //     char cur_char = input.peek();
    //     if (cur_char != '.' && cur_char != 'e' && cur_char != 'E') {
    //         return Node(result_i);
    //     }
    
    //     double result_d;
    //     input.seekg(start_pos, ios_base::beg);
    //     input >> result_d;
    
    //     return Node(result_d);
    // }

    Node LoadDigit(istream& input) {
        int result_i;
        auto start_pos = input.tellg();      
        input >> result_i;
    
        char cur_char = input.peek();
        if (cur_char != '.' && cur_char != 'e' && cur_char != 'E') {
            return Node(result_i);
        }

        string number_str;
        number_str += input.get();
        cur_char = input.peek();

        while (isdigit(cur_char)) {
            number_str += cur_char;
            input.get();
            cur_char = input.peek();
        }
        
        if (cur_char == 'e' || cur_char == 'E') {
            number_str += cur_char;
            input.get();
            cur_char = input.peek();

            if(cur_char == '-' || cur_char == '+') {
                number_str += cur_char;
                input.get();
                cur_char = input.peek();
            }
            
            while (isdigit(cur_char)) {
                number_str += cur_char;
                input.get();
                cur_char = input.peek();
            }
        }

        number_str = to_string(result_i) + number_str;
    
        return Node(stod(number_str));
    }
    
    Node LoadBool(istream& input) {
        std::boolalpha(input);
        bool value;
        input >> value;
        if (input.fail()) {
            throw ParsingError("Wrong input data for bool parsing"s);
        }
        std::noboolalpha(input);
        return value;
    }
    
    Node LoadNull(istream &input) {
        string str("...."s);
        input.read(&str[0], 4);
        if (str != "null"s) {
            throw ParsingError("null parsing in corrupted");
        }
        return Node(nullptr);
    }
    
    Node LoadString(istream& input) {
        string line;
        char cur_char = input.peek();
        while (true) {
            if (input.eof()) {
                throw ParsingError("The end quote \" is missed"s);
            }
    
            cur_char = input.get();
            if (cur_char == '\"') {
                break;
            }
            if (cur_char == '\\') {
                cur_char = input.get();
                switch (cur_char)
                {
                case 'r':
                    line.push_back('\r');
                    break;
                case 'n':
                    line.push_back('\n');
                    break;
                case 't':
                    line.push_back('\t');
                    break;
                case '"': case '\\':
                    line.push_back(cur_char);
                    break;
                default:
                    ParsingError("Wrong escape sequence: "s + cur_char);
                }
            }
            else {
                line.push_back(cur_char);
            }
        }
        
        return Node(line);
    }

    void PrintNode(const Node& node, ostream& out) {
        visit(NodePrinter(out), node.GetValue());
    }
    
}  // namespace

NodePrinter::NodePrinter(ostream& o) : out(o) {}

void NodePrinter::operator () (nullptr_t) {
    out << "null"s;
}

void NodePrinter::operator () (int value) {
    out << value;
}

void NodePrinter::operator () (double value) {
    out << value;
}

void NodePrinter::operator () (const string& value) {
    out << '\"';
    for (char cur_char : value) {
        switch (cur_char)
        {
        case '\n':
            out << "\\n";
            break;
        case '\r':
            out << "\\r";
            break;
        case '\t':
            out << "\\t";
            break;
        case '\\':
            out << "\\\\";
            break;
        case '"':
            out << "\\\"";
            break;
        default:
            out << cur_char;
        }
    }
    out << '\"';
}

void NodePrinter::operator () (bool value) {
    out << boolalpha << value;
    noboolalpha(out);
}

void NodePrinter::operator () (const Array& value) {
    out << "[\n";

    bool first = true;
    for (auto& node : value) {
        if (!first) {
            out << ",\n";
        }
        out << "    ";
        PrintNode(node, out);
        first = false;
    }
    out << "\n]";
}

void NodePrinter::operator () (const Dict& value) {
    out << "{";
    
    bool first = true;
    for (auto& [key, node_in] : value) {
        if (!first) {
            out << ", ";
        }
        out << '"' << key << "\" : ";
        PrintNode(node_in, out);
        first = false;
    }

    out << "}";
}

Node::Node(Array array)
    : value_(move(array)) {
}

Node::Node(Dict map)
    : value_(move(map)) {
}

Node::Node(int value)
    : value_(value) {
}

Node::Node(double value)
    : value_(value) {
}

Node::Node(const string& value) 
    : value_(value) {
}

Node::Node(const char *value) 
    : value_(value) {
}

Node::Node(bool value)
    : value_(value) {
}

Node::Node(std::nullptr_t)
    : value_(nullptr) {
}

const Array& Node::AsArray() const {
    if (!std::holds_alternative<Array>(value_)) {
        throw std::logic_error("Doesn't contain Array"s);
    }
    return std::get<Array>(value_);
}

const Dict& Node::AsMap() const {
    if (!std::holds_alternative<Dict>(value_)) {
        throw std::logic_error("Doesn't contain Dict"s);
    }
    return std::get<Dict>(value_);
}

bool Node::AsBool() const {
    if (!std::holds_alternative<bool>(value_)) {
        throw std::logic_error("Doesn't contain bool"s);
    }
    return std::get<bool>(value_);
}

int Node::AsInt() const {
    if (!std::holds_alternative<int>(value_)) {
        throw std::logic_error("Doesn't contain int");
    }
    return std::get<int>(value_);
}

double Node::AsDouble() const {
    if (std::holds_alternative<double>(value_)) {
        return std::get<double>(value_);
    }
    if (!std::holds_alternative<int>(value_)) {
        throw std::logic_error("Doesn't contain double"s);
    }
    return static_cast<double> (std::get<int>(value_));
}

const std::string& Node::AsString() const {
    if (!std::holds_alternative<std::string>(value_)) {
        throw std::logic_error("Doesn't contain string"s);
    }
    return std::get<std::string>(value_);
}

nullptr_t Node::AsNull() const {
    if (!std::holds_alternative<nullptr_t>(value_)) {
        throw std::logic_error("Doesn't contain nullptr");
    }
    return nullptr;
}

bool Node::IsInt() const {
    return std::holds_alternative<int>(value_);
}

bool Node::IsDouble() const {
    return std::holds_alternative<double>(value_) || std::holds_alternative<int>(value_);
}

bool Node::IsPureDouble() const {
    return std::holds_alternative<double>(value_);
}

bool Node::IsBool() const {
    return std::holds_alternative<bool>(value_);
}

bool Node::IsString() const {
    return std::holds_alternative<std::string>(value_);
}

bool Node::IsNull() const {
    return std::holds_alternative<std::nullptr_t>(value_);
}

bool Node::IsArray() const {
    return std::holds_alternative<Array>(value_);
}

bool Node::IsMap() const {
    return std::holds_alternative<Dict>(value_);
}

const JsonValue& Node::GetValue() const {
    return value_;
}

bool Node::operator==(const Node& node) const {
    return value_ == node.value_;
}

bool Node::operator!=(const Node& node) const {
    return !(*this == node);
}

void Node::SetValue(const JsonValue &value) {
    value_ = value;
}

void Node::SetValue(JsonValue &&value) {
    value_ = move(value);
}

Document::Document(Node root)
    : root_(move(root)) {
}

const Node& Document::GetRoot() const {
    return root_;
}

bool Document::operator==(const Document &other) const {
    return root_ == other.root_;
}

Document Load(istream& input) {
    Document doc = Document(LoadNode(input));
    if (!IsClearStream(input)) {
        throw ParsingError("The json is not parsed fully. Possible reason: wrong final syntax"s);
    }
    return doc;
}

void PrintNode(const Document& doc, std::ostream& output) {
    PrintNode(doc.GetRoot(), output);
}

} // namespace json