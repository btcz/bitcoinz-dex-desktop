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

#include <nlohmann/json.hpp>

//! Project Headers
#include "atomicdex/api/mm2/rpc.setprice.hpp"

namespace mm2::api
{
    void
    to_json(nlohmann::json& j, const setprice_request& request)
    {
        j["base"]            = request.base;
        j["price"]           = request.price;
        j["rel"]             = request.rel;
        j["volume"]          = request.volume;
        j["cancel_previous"] = request.cancel_previous;
        j["max"]             = request.max;
        if (request.base_nota.has_value())
        {
            j["base_nota"] = request.base_nota.value();
        }
        if (request.base_confs.has_value())
        {
            j["base_confs"] = request.base_confs.value();
        }
        if (request.rel_nota.has_value())
        {
            j["rel_nota"] = request.rel_nota.value();
        }
        if (request.rel_confs.has_value())
        {
            j["rel_confs"] = request.rel_confs.value();
        }
        if (request.min_volume.has_value())
        {
            j["min_volume"] = request.min_volume.value();
        }
    }
}