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

#include <entt/core/hashed_string.hpp>
#include <entt/core/type_info.hpp>
#include <entt/core/type_traits.hpp>

//! Project Headers
#include <atomicdex/config/coins.cfg.hpp>

namespace atomic_dex
{
    using mm2_started           = entt::tag<"mm2_started"_hs>;
    using post_login            = entt::tag<"post_login"_hs>;
    using gui_enter_trading     = entt::tag<"gui_enter_trading"_hs>;
    using gui_leave_trading     = entt::tag<"gui_leave_trading"_hs>;
    using mm2_initialized       = entt::tag<"mm2_running_and_enabling"_hs>;
    //using tx_fetch_finished     = entt::tag<"gui_tx_fetch_finished"_hs>;
    using default_coins_enabled = entt::tag<"default_coins_enabled"_hs>;
    // using process_swaps_and_orders_finished = entt::tag<"process_swaps_and_orders_finished"_hs>;
    using band_oracle_refreshed     = entt::tag<"band_oracle_refreshed"_hs>;
    using current_currency_changed  = entt::tag<"update_orders_and_swap_values"_hs>;
    using force_update_providers    = entt::tag<"force_update_providers"_hs>;
    using download_release_finished = entt::tag<"download_release_finished"_hs>;

    struct tx_fetch_finished
    {
        bool with_error{false};
    };

    struct process_swaps_and_orders_finished
    {
        bool after_manual_reset{false};
    };

    struct enabling_coin_failed
    {
        std::string coin;
        std::string reason;
    };

    struct batch_failed
    {
        std::string from;
        std::string reason;
    };

    struct mismatch_configuration_custom_coin
    {
        std::string coin;
    };

    struct endpoint_nonreacheable
    {
        std::string base_uri;
    };

    struct update_portfolio_values
    {
        bool with_update_model{true};
    };

    struct process_orderbook_finished
    {
        bool is_a_reset;
    };

    struct refresh_ohlc_needed
    {
        bool is_a_reset;
    };

    struct start_fetching_new_ohlc_data
    {
        bool is_a_reset;
    };

    struct ticker_balance_updated
    {
        std::vector<std::string> tickers;
    };

    struct fiat_rate_updated
    {
        std::string ticker;
    };

    struct coin_enabled
    {
        std::vector<std::string> tickers;
    };

    //! Event when gecko fetch all the data of this specific coin
    struct coin_fully_initialized
    {
        std::vector<std::string> tickers;
    };

    struct coin_disabled
    {
        std::string ticker;
    };

    struct orderbook_refresh
    {
        std::string base;
        std::string rel;
    };

    struct coin_cfg_parsed
    {
        std::vector<atomic_dex::coin_config> cfg;
    };

    struct fatal_notification
    {
        std::string message;
    };
} // namespace atomic_dex