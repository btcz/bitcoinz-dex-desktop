//
// Created by Sztergbaum Roman on 10/04/2021.
//

#pragma once

//! STD
#include <optional>
#include <string>

//! Deps
#include <nlohmann/json_fwd.hpp>

#include "atomicdex/api/mm2/rpc.min.volume.hpp"

namespace mm2::api
{
    struct min_volume_request
    {
        std::string coin;
    };

    void to_json(nlohmann::json& j, const min_volume_request& cfg);

        struct min_volume_answer_success
    {
        std::string min_trading_vol;
        std::string coin;
    };

    void from_json(const nlohmann::json& j, min_volume_answer_success& cfg);

    struct min_volume_answer
    {
        std::optional<min_volume_answer_success> result;
        std::optional<std::string>               error;
        int                                      rpc_result_code;
        std::string                              raw_result;
    };

    void from_json(const nlohmann::json& j, min_volume_answer& answer);
} // namespace mm2::api

namespace atomic_dex
{
    using t_min_volume_request        = ::mm2::api::min_volume_request;
    using t_min_volume_answer         = ::mm2::api::min_volume_answer;
    using t_min_volume_answer_success = ::mm2::api::min_volume_answer_success;
} // namespace atomic_dex