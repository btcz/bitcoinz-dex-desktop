#pragma once

//! STD
#include <optional>
#include <string>

//! Deps
#include <nlohmann/json_fwd.hpp>

namespace mm2::api
{
    struct update_maker_order_request
    {
        std::string                uuid;
        std::string                new_price;
        std::optional<std::string> volume_delta;
        bool                       max{false};
        std::optional<std::string> min_volume;
        std::optional<bool>        base_nota;
        std::optional<std::size_t> base_confs;
        std::optional<bool>        rel_nota;
        std::optional<std::size_t> rel_confs;
    };

    void to_json(nlohmann::json& j, const update_maker_order_request& request);
} // namespace mm2::api

namespace atomic_dex
{
    using t_update_maker_order_request = ::mm2::api::update_maker_order_request;
}