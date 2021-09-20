//! Deps
#include <nlohmann/json.hpp>

//! Project Headers
#include "atomicdex/api/mm2/generics.hpp"
#include "atomicdex/api/mm2/rpc.min.volume.hpp"

namespace mm2::api
{
    void
    to_json(nlohmann::json& j, const min_volume_request& cfg)
    {
        j["coin"] = cfg.coin;
    }

    void
    from_json(const nlohmann::json& j, min_volume_answer_success& cfg)
    {
        j.at("min_trading_vol").get_to(cfg.min_trading_vol);
        j.at("coin").get_to(cfg.coin);

        //SPDLOG_INFO("coin {} have min_trading_vol: {}", cfg.coin, cfg.min_trading_vol);
    }

    void
    from_json(const nlohmann::json& j, min_volume_answer& answer)
    {
        extract_rpc_json_answer<min_volume_answer_success>(j, answer);
    }
} // namespace mm2::api