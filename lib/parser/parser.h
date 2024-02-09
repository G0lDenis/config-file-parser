#pragma once

#include "../parameter/parameter.h"

#include <filesystem>
#include <fstream>


namespace omfl {

    class Parser {
    private:
        param::Parameter data_;
        bool valid_{false};
    public:
        [[nodiscard]] bool valid() const;

        void SetValidity();

        [[nodiscard]] const param::Parameter& GetRoot() const;

        [[nodiscard]] const param::Parameter& Get(const std::string&) const;

        static param::Parameter& AddVariable(param::Parameter&, const char**);

        param::Parameter& AddSection(const char**);

        param::Parameter& ParseLine(param::Parameter*, const std::string&);

        Parser();

        static PARAMETER_VALUE ParseIntOrFloat(const char** sign);

        static bool ParseBool(const char** sign);

        static std::string ParseString(const char** sign);

        static std::vector<param::Parameter>& ParseArray(const char** sign);
    };

    bool IsAllowedCharacter(char);

    const Parser& parse(const std::filesystem::path& path);

    const Parser& parse(const std::string& str);

}
