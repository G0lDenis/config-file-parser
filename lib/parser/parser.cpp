#include "parser.h"

#include <iostream>


bool omfl::Parser::valid() const {
    return valid_;
}

void omfl::Parser::SetValidity() {
    valid_ = true;
}

const param::Parameter& omfl::Parser::GetRoot() const {
    return data_;
}

omfl::Parser::Parser() :
    data_(param::Parameter{PARAMETER_VALUE(std::map<std::string, param::Parameter>())}) {}

const param::Parameter& omfl::Parser::Get(const std::string& key) const {
    return data_.Get(key);
}

const omfl::Parser& omfl::parse(const std::filesystem::path& path) {
    std::ifstream ifs(path.c_str(), std::ios::in | std::ios::binary | std::ios::ate);

    std::ifstream::pos_type fileSize = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    std::vector<char> bytes(fileSize);
    ifs.read(&bytes[0], fileSize);

    return omfl::parse(std::string(&bytes[0], fileSize));

}

bool omfl::IsAllowedCharacter(char sign) {
    if (isalpha(sign) || isdigit(sign) || sign == '_' || sign == '-') return true;

    return false;
}

PARAMETER_VALUE omfl::Parser::ParseIntOrFloat(const char** sign) {
    std::string value_string_notation;
    PARAMETER_VALUE value = int(0);
    bool completeness = false;
    if (**sign == '-') {
        value_string_notation += '-';
        (*sign)++;
    } else if (**sign == '+') {
        (*sign)++;
    }
    while (**sign && **sign != '#') {
        if (isdigit((**sign))) {
            completeness = true;
            value_string_notation += **sign;
        } else if (**sign == '.') {
            if (!completeness) throw std::runtime_error("Error during parsing float");
            value = 0.f;
            value_string_notation += **sign;
            completeness = false;
        } else break;
        (*sign)++;
    }
    if (!completeness) throw std::runtime_error("Error during parsing int/float");
    switch (value.index()) {
        case 0:
            value = std::stoi(value_string_notation);
            break;
        default:
            value = std::stof(value_string_notation);
    }

    return value;
}

bool omfl::Parser::ParseBool(const char** sign) {
    std::string value_string_notation;
    while (**sign && **sign != '#') {
        if (!isalpha((**sign))) break;
        value_string_notation += **sign;
        (*sign)++;
    }
    if (value_string_notation == "true") return true;
    if (value_string_notation == "false") return false;
    throw std::runtime_error("Bool parameter implementation is incorrect");
}

std::string omfl::Parser::ParseString(const char** sign) {
    std::string value_string_notation;
    (*sign)++;
    while (**sign && **sign != '#') {
        if (**sign == '"') {
            (*sign)++;
            return value_string_notation;
        }
        value_string_notation += **sign;
        (*sign)++;
    }
    throw std::runtime_error("String parameter implementation is incorrect");
}

std::vector<param::Parameter>& omfl::Parser::ParseArray(const char** sign) {
    std::string value_string_notation;
    param::Parameter param;
    auto head = new std::vector<param::Parameter>{};
    bool completeness = false;
    (*sign)++;

    while (**sign && **sign != '#') {
        while (**sign == ' ' || **sign == '\t') {
            (*sign)++;
        }
        PARAMETER_VALUE main_value;
        if (**sign == '"') main_value = ParseString(sign);
        else if (**sign == '[') main_value = ParseArray(sign);
        else if (isdigit((**sign)) || **sign == '-' || **sign == '+') main_value = ParseIntOrFloat(sign);
        else if (**sign == ']') {
            completeness = true;
            (*sign)++;
            break;
        } else main_value = ParseBool(sign);
        head->emplace_back(main_value);
        while (**sign == ' ' || **sign == '\t') {
            (*sign)++;
        }
        if (**sign == ',') (*sign)++;
        else if (**sign != ']') throw std::runtime_error("Error during parsing array");
    }
    if (!completeness) throw std::runtime_error("Error during parsing array");

    return *head;
}

param::Parameter& omfl::Parser::AddVariable(param::Parameter& root, const char** sign) {
    std::string variable;
    while (**sign && **sign != ' ' && **sign != '\t' && **sign != '=') {
        if (!IsAllowedCharacter(**sign))
            throw std::logic_error("Unexpected symbol in variable declaration");
        variable += **sign;
        (*sign)++;
    }
    if (variable.empty()) throw std::runtime_error("Variable name is empty");
    if (!**sign) throw std::runtime_error("Variable without implementation");
    while (**sign == ' ' || **sign == '\t') {
        (*sign)++;
    }
    if (**sign != '=') throw std::runtime_error("Variable without implementation");
    (*sign)++;
    while (**sign == ' ' || **sign == '\t') {
        (*sign)++;
    }
    if (!**sign) throw std::runtime_error("Variable without implementation");

    PARAMETER_VALUE main_value;

    if (**sign == '"') main_value = ParseString(sign);
    else if (**sign == '[') main_value = ParseArray(sign);
    else if (isdigit((**sign)) || **sign == '-' || **sign == '+') main_value = ParseIntOrFloat(sign);
    else main_value = ParseBool(sign);

    while (**sign && **sign != '#') {
        if (**sign != ' ') throw std::runtime_error("Error during parsing variable");
        (*sign)++;
    }
    if (**sign == '#') (*sign)--;
    root.SetValue(variable, main_value);

    return root;
}

param::Parameter& omfl::Parser::AddSection(const char** sign) {
    std::string section;
    param::Parameter* root = &data_;
    while (**sign) {
        if (**sign != '[') {
            if (**sign == '.') {
                if (section.empty()) throw std::runtime_error("Error during parsing section");
                root = &(root->SetValue(section, std::map<std::string, param::Parameter>()));
                section = "";
            } else if (**sign == ']') {
                if (section.empty()) throw std::runtime_error("Error during parsing section");
                (*sign)++;
                root = &(root->SetValue(section, std::map<std::string, param::Parameter>()));

                return *root;
            } else if (IsAllowedCharacter(**sign)) {
                section += **sign;
            } else {
                throw std::runtime_error("Unexpected symbol in section declaration");
            }
        }
        (*sign)++;
    }

    throw std::runtime_error("Error during parsing section");
}

param::Parameter& omfl::Parser::ParseLine(param::Parameter* section, const std::string& str) {
    bool completeness = false;
    const char* sign = str.c_str();
    while (*sign) {
        if (*sign == '#') return *section;
        if (*sign == ' ' || *sign == '\t') {
            sign++;
            continue;
        }
        if (completeness) throw std::runtime_error("Error during parsing line");
        if (*sign == '[') {
            section = &(this->AddSection(&sign));
        } else {
            this->AddVariable(*section, &sign);
        }
        completeness = true;
    }

    return *section;
}

const omfl::Parser& omfl::parse(const std::string& str) {
    auto* parser = new Parser{};
    std::string line;
    std::stringstream stream(str);
    auto* root = const_cast<param::Parameter*>(&parser->GetRoot());

    while (std::getline(stream, line, '\n')) {
        try {
            root = &parser->ParseLine(root, line);
        }
        catch (const std::exception& ex) {
            std::cout << ex.what();
            return *parser;
        }
    }
    parser->SetValidity();

    return *parser;
}
