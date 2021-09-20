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

#pragma once

//! Deps
#include <nlohmann/json_fwd.hpp>

//! Project Headers
#include "atomicdex/config/electrum.cfg.hpp"
#include "atomicdex/constants/qt.coins.enums.hpp"

namespace mm2::api
{
    struct electrum_request
    {
        std::string                              coin_name;
        std::vector<atomic_dex::electrum_server> servers;
        CoinType                                 coin_type;
        bool                                     is_testnet{false};
        bool                                     with_tx_history{true};
        const std::string                        testnet_qrc_swap_contract_address{"0xba8b71f3544b93e2f681f996da519a98ace0107a"};
        const std::string                        testnet_fallback_qrc_swap_contract_address{testnet_qrc_swap_contract_address};
        const std::string                        mainnet_qrc_swap_contract_address{"0x2f754733acd6d753731c00fee32cb484551cc15d"};
        const std::string                        mainnet_fallback_qrc_swap_contract_address{mainnet_qrc_swap_contract_address};
        std::optional<nlohmann::json>            address_format;
    };

    struct electrum_answer
    {
        std::string address;
        std::string balance;
        std::string result;
        int         rpc_result_code;
        std::string raw_result;
    };

    void to_json(nlohmann::json& j, const electrum_request& cfg);

    void from_json(const nlohmann::json& j, electrum_answer& answer);
} // namespace mm2::api

namespace atomic_dex
{
    using t_electrum_request = ::mm2::api::electrum_request;
    using t_electrum_answer  = ::mm2::api::electrum_answer;
} // namespace atomic_dex