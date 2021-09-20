#pragma once

//! STD Headers
#include <string>

//! Deps
#include <nlohmann/json.hpp>

namespace mm2::api
{
    struct generic_answer_error
    {
        std::string    error;
        std::string    error_path;
        std::string    error_trace;
        std::string    error_type;
        nlohmann::json error_data;
    };

    void from_json(const nlohmann::json& j, generic_answer_error& res);
} // namespace mm2::api