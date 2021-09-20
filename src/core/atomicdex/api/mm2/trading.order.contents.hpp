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

//! STD
#include <string>

//! Deps
#include <nlohmann/json_fwd.hpp>

namespace mm2::api
{
    struct trading_order_contents
    {
        std::string action;
        std::string base;
        std::string base_amount;
        std::string dest_pub_key;
        std::string method;
        std::string rel;
        std::string rel_amount;
        std::string sender_pubkey;
        std::string uuid;
    };

    void from_json(const nlohmann::json& j, trading_order_contents& contents);
}