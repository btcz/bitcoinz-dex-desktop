//
// Created by Sztergbaum Roman on 22/05/2021.
//

//! Deps
#include <nlohmann/json.hpp>

//! Project Headers
#include "atomicdex/api/mm2/rpc.convertaddress.hpp"
#include "atomicdex/api/mm2/generics.hpp"

namespace mm2::api
{
    void
    to_json(nlohmann::json& j, const mm2::api::convert_address_request& req)
    {
        j["coin"]              = req.coin;
        j["from"]              = req.from;
        j["to_address_format"] = req.to_address_format;
    }

    void
    from_json(const nlohmann::json& j, convert_address_answer_success& answer)
    {
        j.at("address").get_to(answer.address);
    }

    void
    from_json(const nlohmann::json& j, convert_address_answer& answer)
    {
        extract_rpc_json_answer<convert_address_answer_success>(j, answer);
    }
} // namespace mm2::api
