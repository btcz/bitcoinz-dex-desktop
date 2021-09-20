/******************************************************************************
 * Copyright © 2013-2021 The Komodo Platform Developers.                      *
 *                                                                            *
 * See the AUTHORS, DEVELOPER-AGREEMENT and LICENSE files at                  *
 * the top-level directory of this distribution for the individual copyright  *
 * holder information and the developer policies on copyright and licensing.  *
 *                                                                            *
 * Unless otherwise agreed in a custom licensing agreement, no part of the    *
 * Komodo Platform software, including this file may be copied, modified,     *
 * propagated or distributed except according to the terms contained in the   *
 * LICENSE file                                                               *
 *                                                                            *
 * Removal or modification of this copyright notice is prohibited.            *
 *                                                                            *
 ******************************************************************************/

//! Deps
#include <nlohmann/json.hpp>

//! Project Deps
#include "atomicdex/api/mm2/rpc.electrum.hpp"

//! Implementation RPC [electrum]
namespace mm2::api
{
    //! Serialization
    void
    to_json(nlohmann::json& j, const electrum_request& cfg)
    {
        j["coin"]       = cfg.coin_name;
        j["servers"]    = cfg.servers;
        j["tx_history"] = cfg.with_tx_history;
        if (cfg.coin_type == CoinType::QRC20)
        {
            j["swap_contract_address"] = cfg.is_testnet ? cfg.testnet_qrc_swap_contract_address : cfg.mainnet_qrc_swap_contract_address;
            j["fallback_swap_contract"] = cfg.is_testnet ? cfg.testnet_fallback_qrc_swap_contract_address : cfg.mainnet_fallback_qrc_swap_contract_address;
        }
        if (cfg.address_format.has_value()) {
            j["address_format"] = cfg.address_format.value();
        }
        //SPDLOG_INFO("electrum: {}", j.dump());
    }

    //! Deserialization
    void
    from_json(const nlohmann::json& j, electrum_answer& cfg)
    {
        j.at("address").get_to(cfg.address);
        j.at("balance").get_to(cfg.balance);
        j.at("result").get_to(cfg.result);
    }
} // namespace mm2::api