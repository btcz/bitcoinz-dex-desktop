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
#include <vector>

//! Deps
#include <date/date.h>           ///< date::format
#include <date/tz.h>             ///< date::make_zoned
#include <entt/core/attribute.h> ///< ENTT_API

#include "fs.prerequisites.hpp"
#include "safe.float.hpp"
#include "atomicdex/config/coins.cfg.hpp"
#include "log.prerequisites.hpp"

namespace atomic_dex::utils
{
    //! Float numbers helpers
    std::string get_formated_float(t_float_50 value);
    std::string adjust_precision(const std::string& current);
    std::string format_float(t_float_50 value);
    std::string extract_large_float(const std::string& current);

    //! Fs helpers
    bool        create_if_doesnt_exist(const fs::path& path);
    std::string u8string(const fs::path& path);
    //std::string u8string(const std::wstring& p);
    //std::string wstring_to_utf8(const std::wstring& str);
    //std::string to_utf8(const wchar_t* w);

    double determine_balance_factor(bool with_pin_cfg);

    template <typename TimeFormat = std::chrono::milliseconds>
    inline std::string
    to_human_date(std::size_t timestamp, std::string format)
    {
        using namespace date;

        const sys_time<TimeFormat> tp{TimeFormat{timestamp}};

        try
        {
            const auto tp_zoned = date::make_zoned(current_zone(), tp);
            return date::format(std::move(format), tp_zoned);
        }
        catch (const std::exception& error)
        {
            return date::format(std::move(format), tp);
        }
    }

    ENTT_API fs::path get_atomic_dex_data_folder();

    /// \brief  Gets the path where addressbook configs are stored.
    /// \return An fs::path object.
    [[nodiscard]] fs::path get_atomic_dex_addressbook_folder();

    fs::path get_runtime_coins_path() ;

    fs::path get_atomic_dex_logs_folder() ;

    ENTT_API fs::path get_atomic_dex_current_log_file();
    ENTT_API std::shared_ptr<spdlog::logger> register_logger();

    ENTT_API fs::path get_current_configs_path();

    fs::path get_mm2_atomic_dex_current_log_file();

    fs::path get_atomic_dex_config_folder();

    //std::string minimal_trade_amount_str();

    //const t_float_50 minimal_trade_amount();

    fs::path get_atomic_dex_export_folder();

    fs::path get_atomic_dex_current_export_recent_swaps_file();

    ENTT_API fs::path get_themes_path();
    ENTT_API fs::path get_logo_path();

    std::string retrieve_main_ticker(const std::string& ticker);

    void to_eth_checksum(std::string& address);

    std::vector<std::string> coin_cfg_to_ticker_cfg(std::vector<coin_config> in);

} // namespace atomic_dex::utils
