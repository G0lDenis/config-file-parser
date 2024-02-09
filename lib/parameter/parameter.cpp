#include "parameter.h"


int param::Parameter::AsInt() const {
    if (data_.index() != 0) {
        throw std::logic_error("Parameter is not int type");
    }

    return std::get<0>(data_);
}

int param::Parameter::AsIntOrDefault(int default_value) const {
    if (data_.index() != 0 || !real_) {
        return default_value;
    }

    return std::get<0>(data_);
}

bool param::Parameter::IsInt() const {
    if (data_.index() != 0) {
        return false;
    }

    return true;
}

float param::Parameter::AsFloat() const {
    if (data_.index() != 1) {
        throw std::logic_error("Parameter is not float type");
    }

    return std::get<1>(data_);
}

float param::Parameter::AsFloatOrDefault(float default_value) const {
    if (data_.index() != 1 || !real_) {
        return default_value;
    }

    return std::get<1>(data_);
}

bool param::Parameter::IsFloat() const {
    if (data_.index() != 1) {
        return false;
    }

    return true;
}

bool param::Parameter::AsBool() const {
    if (data_.index() != 2) {
        throw std::logic_error("Parameter is not bool type");
    }

    return std::get<2>(data_);
}

[[maybe_unused]] bool param::Parameter::AsBoolOrDefault(bool default_value) const {
    if (data_.index() != 2 || !real_) {
        return default_value;
    }

    return std::get<2>(data_);
}

bool param::Parameter::IsBool() const {
    if (data_.index() != 2) {
        return false;
    }

    return true;
}

const std::string& param::Parameter::AsString() const {
    if (data_.index() != 3) {
        throw std::logic_error("Parameter is not string type");
    }

    return std::get<3>(data_);
}

const std::string& param::Parameter::AsStringOrDefault(const std::string& default_value) const {
    if (data_.index() != 3 || !real_) {
        return default_value;
    }

    return std::get<3>(data_);
}

bool param::Parameter::IsString() const {
    if (data_.index() != 3) {
        return false;
    }

    return true;
}

const param::Parameter& param::Parameter::Get(const std::string& key) const {
    auto* param = const_cast<param::Parameter*>(this);
    size_t pos;
    size_t prev_pos = 0;
    while ((pos = key.find('.', prev_pos)) != std::string::npos) {
        param = const_cast<param::Parameter*>(&param->GetSectionValue(key.substr(prev_pos, pos - prev_pos)));
        prev_pos = pos + 1;
    }
    param = const_cast<param::Parameter*>(&param->GetSectionValue(key.substr(prev_pos, key.length() - prev_pos)));

    return const_cast<const param::Parameter&>(*param);
}

const param::Parameter& param::Parameter::GetSectionValue(const std::string& key) {
    if (data_.index() != 5) {
        throw std::logic_error("Parameter is not section");
    }
    const std::map<std::string, Parameter>* param = &std::get<5>(data_);
    if (!(*param).contains(key)) {
        throw std::runtime_error("No parameter with such key found");
    }

    return param->at(key);
}

param::Parameter& param::Parameter::SetValue(const std::string& key, const PARAMETER_VALUE& value) {
    if (data_.index() != 5)throw std::logic_error("Parameter is not section");
    if (value.index() != 5 && std::get<5>(data_).contains(key)) throw std::runtime_error("Variable already exists");
    switch (value.index()) {
        case 0:
            std::get<5>(data_).insert({key, Parameter(std::get<0>(value))});
            return std::get<5>(data_).at(key);
        case 1:
            std::get<5>(data_).insert({key, Parameter(std::get<1>(value))});
            return std::get<5>(data_).at(key);
        case 2:
            std::get<5>(data_).insert({key, Parameter(std::get<2>(value))});
            return std::get<5>(data_).at(key);
        case 3:
            std::get<5>(data_).insert({key, Parameter(std::get<3>(value))});
            return std::get<5>(data_).at(key);
        case 4:
            std::get<5>(data_).insert({key, Parameter(std::get<4>(value))});
            return std::get<5>(data_).at(key);
        default:
            std::map<std::string, Parameter>* param = &std::get<5>(data_);
            if (!(*param).contains(key)) {
                auto* new_parameter = new Parameter(std::map<std::string, Parameter>());
                param->insert({key, *new_parameter});
            }

            return param->at(key);
    }
}

bool param::Parameter::IsArray() const {
    if (data_.index() != 4) {
        return false;
    }

    return true;
}

const param::Parameter& param::Parameter::operator[](size_t key) const {
    if (data_.index() != 4) {
        throw std::logic_error("Parameter is not array");
    }
    if (std::get<4>(data_).size() <= key) return param::invalid_param;

    return std::get<4>(data_)[key];
}

param::Parameter::Parameter(const PARAMETER_VALUE& value) {
    data_ = value;
}

param::Parameter::Parameter() = default;

param::Parameter::Parameter(bool validity, const PARAMETER_VALUE& value) {
    real_ = validity;
    data_ = value;
}

param::Parameter& param::Parameter::operator=(const PARAMETER_VALUE& value) {
    data_ = value;
    return *this;
}

param::Parameter& param::Parameter::operator=(const int& value) {
    data_ = value;
    return *this;
}

param::Parameter& param::Parameter::operator=(const float& value) {
    data_ = value;
    return *this;
}

param::Parameter& param::Parameter::operator=(const bool& value) {
    data_ = value;
    return *this;
}

param::Parameter& param::Parameter::operator=(const std::string& value) {
    data_ = value;
    return *this;
}

param::Parameter& param::Parameter::operator=(const std::vector<param::Parameter>& value) {
    data_ = value;
    return *this;
}
