//
// Created by Sztergbaum Roman on 27/03/2021.
//

#include <nlohmann/json.hpp>

//!
#include "atomicdex/api/mm2/generics.hpp"
#include "atomicdex/api/mm2/rpc.disable.hpp"

namespace mm2::api
{
    //! Serialization
    void
    to_json(nlohmann::json& j, const disable_coin_request& req)
    {
        j["coin"] = req.coin;
    }

    //! Deserialization
    void
    from_json(const nlohmann::json& j, disable_coin_answer_success& resp)
    {
        j.at("coin").get_to(resp.coin);
    }

    void
    from_json(const nlohmann::json& j, disable_coin_answer& resp)
    {
        extract_rpc_json_answer<disable_coin_answer_success>(j, resp);
    }
} // namespace mm2::api