#pragma once

#include <map>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>

#define PARAMETER_VALUE std::variant<int, float, bool, std::string, std::vector<param::Parameter>, std::map<std::string, param::Parameter>>

namespace param {
    class Parameter {
    private:
        bool real_ = true;
        PARAMETER_VALUE data_;
    public:
        [[nodiscard]] int AsInt() const;

        [[nodiscard]] int AsIntOrDefault(int) const;

        [[nodiscard]] bool IsInt() const;

        [[nodiscard]] float AsFloat() const;

        [[nodiscard]] float AsFloatOrDefault(float) const;

        [[nodiscard]] bool IsFloat() const;

        [[nodiscard]] bool AsBool() const;

        [[maybe_unused]] [[nodiscard]] bool AsBoolOrDefault(bool) const;

        [[nodiscard]] bool IsBool() const;

        [[nodiscard]] const std::string& AsString() const;

        [[nodiscard]] const std::string& AsStringOrDefault(const std::string&) const;

        [[nodiscard]] bool IsString() const;

        [[nodiscard]] const Parameter& Get(const std::string&) const;

        Parameter& SetValue(const std::string&, const PARAMETER_VALUE&);

        [[nodiscard]] bool IsArray() const;

        const Parameter& operator[](size_t) const;

        explicit Parameter(const PARAMETER_VALUE&);

        explicit Parameter();

        explicit Parameter(bool, const PARAMETER_VALUE&);

        Parameter& operator=(const PARAMETER_VALUE&);

        Parameter& operator=(const int&);

        Parameter& operator=(const float&);

        Parameter& operator=(const bool&);

        Parameter& operator=(const std::string&);

        Parameter& operator=(const std::vector<param::Parameter>&);

        const Parameter& GetSectionValue(const std::string&);
    } static invalid_param{false, 0};
}
