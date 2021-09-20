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

//! QT
#include <QDebug>
#include <QFile>
#include <QJsonDocument>
#include <QLocale>
#include <QSettings>

//! Deps
#include <boost/algorithm/string/case_conv.hpp>

//! Project Headers
#include "atomicdex/events/events.hpp"
#include "atomicdex/managers/qt.wallet.manager.hpp"
#include "atomicdex/models/qt.global.coins.cfg.model.hpp"
#include "atomicdex/pages/qt.portfolio.page.hpp"
#include "atomicdex/pages/qt.settings.page.hpp"
#include "atomicdex/services/mm2/mm2.service.hpp"
#include "atomicdex/services/price/coingecko/coingecko.wallet.charts.hpp"
#include "atomicdex/services/price/global.provider.hpp"
#include "atomicdex/utilities/global.utilities.hpp"
#include "atomicdex/utilities/qt.utilities.hpp"

namespace
{
    void
    copy_icon(const QString icon_filepath, const QString icons_path_directory, const std::string& ticker)
    {
        SPDLOG_INFO("copying icon");
        if (not icon_filepath.isEmpty())
        {
            const fs::path& suffix = fs::path(icon_filepath.toStdString()).extension();
            fs::copy_file(
                icon_filepath.toStdString(), fs::path(icons_path_directory.toStdString()) / (boost::algorithm::to_lower_copy(ticker) + suffix.string()),
                get_override_options());
        }
        SPDLOG_INFO("copying icon finished");
    }
} // namespace

//! Constructo destructor
namespace atomic_dex
{
    settings_page::settings_page(entt::registry& registry, ag::ecs::system_manager& system_manager, std::shared_ptr<QApplication> app, QObject* parent) :
        QObject(parent), system(registry), m_system_manager(system_manager), m_app(app)
    {
        SPDLOG_INFO("settings_page created");
    }
} // namespace atomic_dex

//! Override
namespace atomic_dex
{
    void
    settings_page::update()
    {
    }
} // namespace atomic_dex

//! Properties
namespace atomic_dex
{
    QString
    settings_page::get_current_lang() const
    {
        QSettings& settings = entity_registry_.ctx<QSettings>();
        return settings.value("CurrentLang").toString();
    }

    void
    atomic_dex::settings_page::set_current_lang(QString new_lang)
    {
        const std::string new_lang_std = new_lang.toStdString();
        QSettings&        settings     = entity_registry_.ctx<QSettings>();
        settings.setValue("CurrentLang", new_lang);
        settings.sync();

        auto get_locale = [](const std::string& current_lang)
        {
            if (current_lang == "tr")
            {
                return QLocale::Language::Turkish;
            }
            if (current_lang == "en")
            {
                return QLocale::Language::English;
            }
            if (current_lang == "fr")
            {
                return QLocale::Language::French;
            }
            if (current_lang == "ru")
            {
                return QLocale::Language::Russian;
            }
            return QLocale::Language::AnyLanguage;
        };

        SPDLOG_INFO("Locale before parsing AtomicDEX settings: {}", QLocale().name().toStdString());
        QLocale::setDefault(get_locale(new_lang.toStdString()));
        SPDLOG_INFO("Locale after parsing AtomicDEX settings: {}", QLocale().name().toStdString());
        [[maybe_unused]] auto res = this->m_translator.load("atomic_defi_" + new_lang, QLatin1String(":/atomic_defi_design/assets/languages"));
        assert(res);
        this->m_app->installTranslator(&m_translator);
        this->m_qml_engine->retranslate();
        emit onLangChanged();
        SPDLOG_INFO("Post lang changed");
    }

    bool
    atomic_dex::settings_page::is_notification_enabled() const
    {
        return m_config.notification_enabled;
    }

    void
    settings_page::set_notification_enabled(bool is_enabled)
    {
        if (m_config.notification_enabled != is_enabled)
        {
            change_notification_status(m_config, is_enabled);
            emit onNotificationEnabledChanged();
        }
    }

    QString
    settings_page::get_current_currency_sign() const
    {
        return QString::fromStdString(this->m_config.current_currency_sign);
    }

    QString
    settings_page::get_current_fiat_sign() const
    {
        return QString::fromStdString(this->m_config.current_fiat_sign);
    }

    QString
    settings_page::get_current_currency() const
    {
        return QString::fromStdString(this->m_config.current_currency);
    }

    void
    settings_page::set_current_currency(const QString& current_currency)
    {
        bool        can_proceed = true;
        std::string reason      = "";
        if (atomic_dex::is_this_currency_a_fiat(m_config, current_currency.toStdString()))
        {
            if (!m_system_manager.get_system<global_price_service>().is_fiat_available(current_currency.toStdString()))
            {
                can_proceed = false;
                reason      = "rate for fiat: " + current_currency.toStdString() + " not available";
            }
        }
        else
        {
            if (!m_system_manager.get_system<global_price_service>().is_currency_available(current_currency.toStdString()))
            {
                can_proceed = false;
                reason      = "rate for currency " + current_currency.toStdString() + " not available";
            }
        }


        if (current_currency.toStdString() != m_config.current_currency && can_proceed)
        {
            SPDLOG_INFO("change currency {} to {}", m_config.current_currency, current_currency.toStdString());
            atomic_dex::change_currency(m_config, current_currency.toStdString());

            // this->dispatcher_.trigger<force_update_providers>();
            this->dispatcher_.trigger<update_portfolio_values>();
            this->dispatcher_.trigger<current_currency_changed>();
            emit onCurrencyChanged();
            emit onCurrencySignChanged();
            emit onFiatSignChanged();
        }
        else
        {
            if (!reason.empty())
            {
                SPDLOG_ERROR("cannot change currency for reason: {}", reason);
            }
        }
    }

    QString
    settings_page::get_current_fiat() const
    {
        return QString::fromStdString(this->m_config.current_fiat);
    }

    void
    settings_page::set_current_fiat(const QString& current_fiat)
    {
        if (m_system_manager.get_system<global_price_service>().is_fiat_available(current_fiat.toStdString()))
        {
            if (current_fiat.toStdString() != m_config.current_fiat)
            {
                SPDLOG_INFO("change fiat {} to {}", m_config.current_fiat, current_fiat.toStdString());
                atomic_dex::change_fiat(m_config, current_fiat.toStdString());
                m_system_manager.get_system<coingecko_wallet_charts_service>().manual_refresh("set_current_fiat");
                emit onFiatChanged();
            }
        }
        else
        {
            SPDLOG_ERROR("Cannot change fiat, because other rates are not available");
        }
    }
} // namespace atomic_dex

//! Public API
namespace atomic_dex
{
    atomic_dex::cfg&
    settings_page::get_cfg()
    {
        return m_config;
    }

    const atomic_dex::cfg&
    settings_page::get_cfg() const
    {
        return m_config;
    }

    void
    settings_page::init_lang()
    {
        set_current_lang(get_current_lang());
    }
} // namespace atomic_dex

//! QML API
namespace atomic_dex
{
    QStringList
    settings_page::get_available_langs() const
    {
        QSettings& settings = entity_registry_.ctx<QSettings>();
        return settings.value("AvailableLang").toStringList();
    }

    QStringList
    settings_page::get_available_fiats() const
    {
        QStringList out;
        out.reserve(m_config.available_fiat.size());
        for (auto&& cur_fiat: m_config.available_fiat) { out.push_back(QString::fromStdString(cur_fiat)); }
        out.sort();
        return out;
    }

    QStringList
    settings_page::get_recommended_fiats() const
    {
        static const auto nb_recommended = 6;
        QStringList       out;
        out.reserve(nb_recommended);
        for (auto&& it = m_config.available_fiat.begin(); it != m_config.available_fiat.end() && it < m_config.available_fiat.begin() + nb_recommended; it++)
        {
            out.push_back(QString::fromStdString(*it));
        }
        return out;
    }

    QStringList
    settings_page::get_available_currencies() const
    {
        QStringList out;
        out.reserve(m_config.possible_currencies.size());
        for (auto&& cur_currency: m_config.possible_currencies) { out.push_back(QString::fromStdString(cur_currency)); }
        return out;
    }

    bool
    settings_page::is_this_ticker_present_in_raw_cfg(const QString& ticker) const
    {
        return m_system_manager.get_system<mm2_service>().is_this_ticker_present_in_raw_cfg(ticker.toStdString());
    }

    bool
    settings_page::is_this_ticker_present_in_normal_cfg(const QString& ticker) const
    {
        return m_system_manager.get_system<mm2_service>().is_this_ticker_present_in_normal_cfg(ticker.toStdString());
    }

    QString
    settings_page::get_custom_coins_icons_path() const
    {
        return std_path_to_qstring(utils::get_runtime_coins_path());
    }

    void
    settings_page::process_qrc_20_token_add(const QString& contract_address, const QString& coingecko_id, const QString& icon_filepath)
    {
        this->set_fetching_custom_token_data_busy(true);
        using namespace std::string_literals;
        std::string url            = "/contract/"s + contract_address.toStdString();
        auto        answer_functor = [this, contract_address, coingecko_id, icon_filepath](web::http::http_response resp)
        {
            std::string    body = TO_STD_STR(resp.extract_string(true).get());
            nlohmann::json out  = nlohmann::json::object();
            out["mm2_cfg"]      = nlohmann::json::object();
            out["adex_cfg"]     = nlohmann::json::object();
            if (resp.status_code() == 200)
            {
                nlohmann::json body_json      = nlohmann::json::parse(body);
                const auto     ticker         = body_json.at("qrc20").at("symbol").get<std::string>() + "-QRC20";
                const auto     name_lowercase = boost::algorithm::to_lower_copy(body_json.at("qrc20").at("name").get<std::string>());
                out["ticker"]                 = ticker;
                out["name"]                   = name_lowercase;
                copy_icon(icon_filepath, get_custom_coins_icons_path(), atomic_dex::utils::retrieve_main_ticker(ticker));
                const auto&    mm2      = this->m_system_manager.get_system<mm2_service>();
                nlohmann::json qtum_cfg = mm2.get_raw_mm2_ticker_cfg("QTUM");
                if (not is_this_ticker_present_in_raw_cfg(QString::fromStdString(ticker)))
                {
                    out["mm2_cfg"]["protocol"]                                      = nlohmann::json::object();
                    out["mm2_cfg"]["protocol"]["type"]                              = "QRC20";
                    out["mm2_cfg"]["protocol"]["protocol_data"]                     = nlohmann::json::object();
                    out["mm2_cfg"]["protocol"]["protocol_data"]["platform"]         = "QTUM";
                    std::string out_address                                         = "0x" + contract_address.toStdString();
                    out["mm2_cfg"]["protocol"]["protocol_data"]["contract_address"] = out_address;
                    out["mm2_cfg"]["coin"]                                          = ticker;
                    // out["mm2_cfg"]["gui_coin"]                                      = ticker;
                    out["mm2_cfg"]["mm2"] = 1;
                    if (body_json.at("qrc20").contains("decimals"))
                    {
                        out["mm2_cfg"]["decimals"] = body_json.at("qrc20").at("decimals").get<int>();
                    }
                    out["mm2_cfg"]["txfee"]                  = qtum_cfg["txfee"];
                    out["mm2_cfg"]["pubtype"]                = qtum_cfg["pubtype"];
                    out["mm2_cfg"]["p2shtype"]               = qtum_cfg["p2shtype"];
                    out["mm2_cfg"]["wiftype"]                = qtum_cfg["wiftype"];
                    out["mm2_cfg"]["name"]                   = qtum_cfg["name"];
                    out["mm2_cfg"]["rpcport"]                = qtum_cfg["rpcport"];
                    out["mm2_cfg"]["segwit"]                 = qtum_cfg["segwit"];
                    out["mm2_cfg"]["required_confirmations"] = 3;
                    out["mm2_cfg"]["fname"]                  = name_lowercase;
                }
                if (not is_this_ticker_present_in_normal_cfg(QString::fromStdString(ticker)))
                {
                    //!
                    out["adex_cfg"][ticker]                      = nlohmann::json::object();
                    out["adex_cfg"][ticker]["coin"]              = ticker;
                    out["adex_cfg"][ticker]["gui_coin"]          = ticker;
                    out["adex_cfg"][ticker]["name"]              = body_json.at("qrc20").at("name").get<std::string>();
                    out["adex_cfg"][ticker]["coingecko_id"]      = coingecko_id.toStdString();
                    out["adex_cfg"][ticker]["explorer_url"]      = nlohmann::json::array({"https://explorer.qtum.org/"});
                    out["adex_cfg"][ticker]["type"]              = "QRC-20";
                    out["adex_cfg"][ticker]["active"]            = false;
                    out["adex_cfg"][ticker]["currently_enabled"] = false;
                    out["adex_cfg"][ticker]["is_custom_coin"]    = true;
                    if (not out.at("mm2_cfg").empty())
                    {
                        SPDLOG_INFO("mm2_cfg found, backup from new cfg");
                        out["adex_cfg"][ticker]["mm2_backup"] = out["mm2_cfg"];
                    }
                    else
                    {
                        if (mm2.is_this_ticker_present_in_raw_cfg(ticker))
                        {
                            SPDLOG_INFO("mm2_cfg not found backup {} cfg from current cfg", ticker);
                            out["adex_cfg"][ticker]["mm2_backup"] = mm2.get_raw_mm2_ticker_cfg(ticker);
                        }
                    }
                }
            }
            else
            {
                out["error_message"] = body;
                out["error_code"]    = resp.status_code();
            }
            SPDLOG_DEBUG("result json of fetch qrc infos from contract address is: {}", out.dump(4));
            this->set_custom_token_data(nlohmann_json_object_to_qt_json_object(out));
            this->set_fetching_custom_token_data_busy(false);
        };
        ::mm2::api::async_process_rpc_get(::mm2::api::g_qtum_proxy_http_client, "qrc_infos", url).then(answer_functor).then(&handle_exception_pplx_task);
    }

    void
    settings_page::process_token_add(const QString& contract_address, const QString& coingecko_id, const QString& icon_filepath, CoinType coin_type)
    {
        this->set_fetching_custom_token_data_busy(true);
        using namespace std::string_literals;

        auto retrieve_functor_url = [ coin_type, contract_address ]() -> auto
        {
            switch (coin_type)
            {
            case CoinTypeGadget::QRC20:
                return std::make_tuple(
                    &::mm2::api::g_qtum_proxy_http_client, "/contract/"s + contract_address.toStdString(), "QRC20"s, "QTUM"s, "QRC-20"s, "QTUM"s, "QRC20"s);
            case CoinTypeGadget::ERC20:
                return std::make_tuple(
                    &::mm2::api::g_etherscan_proxy_http_client, "/api/v1/token_infos/erc20/"s + contract_address.toStdString(), "ERC20"s, "ETH"s, "ERC-20"s,
                    "ETH"s, "ERC20"s);
            case CoinTypeGadget::BEP20:
                return std::make_tuple(
                    &::mm2::api::g_etherscan_proxy_http_client, "/api/v1/token_infos/bep20/"s + contract_address.toStdString(), "BEP20"s, "ETH"s, "BEP-20"s,
                    "BNB"s, "ERC20"s);
            default:
                return std::make_tuple(&::mm2::api::g_etherscan_proxy_http_client, ""s, ""s, ""s, ""s, ""s, ""s);
            }
        };
        auto&& [endpoint, url, type, platform, adex_platform, parent_chain, parent_type] = retrieve_functor_url();

        auto answer_functor = [this, contract_address, coingecko_id, icon_filepath, type = type, platform = platform, adex_platform = adex_platform,
                               parent_chain = parent_chain, parent_type = parent_type](web::http::http_response resp)
        {
            //! Extract answer
            std::string    body = TO_STD_STR(resp.extract_string(true).get());
            nlohmann::json out  = nlohmann::json::object();
            out["mm2_cfg"]      = nlohmann::json::object();
            out["adex_cfg"]     = nlohmann::json::object();
            const auto& mm2     = this->m_system_manager.get_system<mm2_service>();
            if (resp.status_code() == 200)
            {
                nlohmann::json raw_parent_cfg = mm2.get_raw_mm2_ticker_cfg(parent_chain);
                nlohmann::json body_json      = nlohmann::json::parse(body).at("result")[0];
                const auto     ticker         = body_json.at("symbol").get<std::string>() + "-" + type;
                const auto     name_lowercase = body_json.at("tokenName").get<std::string>();
                out["ticker"]                 = ticker;
                out["name"]                   = name_lowercase;
                copy_icon(icon_filepath, get_custom_coins_icons_path(), atomic_dex::utils::retrieve_main_ticker(ticker));
                if (not is_this_ticker_present_in_raw_cfg(QString::fromStdString(ticker)))
                {
                    out["mm2_cfg"]["protocol"]                              = nlohmann::json::object();
                    out["mm2_cfg"]["protocol"]["type"]                      = parent_type;
                    out["mm2_cfg"]["protocol"]["protocol_data"]             = nlohmann::json::object();
                    out["mm2_cfg"]["protocol"]["protocol_data"]["platform"] = platform;
                    std::string out_address                                 = contract_address.toStdString();
                    boost::algorithm::to_lower(out_address);
                    utils::to_eth_checksum(out_address);
                    out["mm2_cfg"]["protocol"]["protocol_data"]["contract_address"] = out_address;
                    out["mm2_cfg"]["rpcport"]                                       = raw_parent_cfg.at("rpcport");
                    out["mm2_cfg"]["coin"]                                          = ticker;
                    out["mm2_cfg"]["mm2"]                                           = 1;
                    out["mm2_cfg"]["decimals"]                                      = std::stoi(body_json.at("divisor").get<std::string>());
                    out["mm2_cfg"]["avg_blocktime"]                                 = raw_parent_cfg.at("avg_blocktime");
                    out["mm2_cfg"]["required_confirmations"]                        = raw_parent_cfg.at("required_confirmations");
                    out["mm2_cfg"]["name"]                                          = name_lowercase;
                }
                if (not is_this_ticker_present_in_normal_cfg(QString::fromStdString(ticker)))
                {
                    //!
                    out["adex_cfg"][ticker]                      = nlohmann::json::object();
                    out["adex_cfg"][ticker]["coin"]              = ticker;
                    out["adex_cfg"][ticker]["name"]              = name_lowercase;
                    out["adex_cfg"][ticker]["coingecko_id"]      = coingecko_id.toStdString();
                    const auto& coin_info                        = mm2.get_coin_info(parent_chain);
                    out["adex_cfg"][ticker]["nodes"]             = coin_info.urls.value_or(std::vector<std::string>());
                    out["adex_cfg"][ticker]["explorer_url"]      = coin_info.explorer_url;
                    out["adex_cfg"][ticker]["type"]              = adex_platform;
                    out["adex_cfg"][ticker]["active"]            = false;
                    out["adex_cfg"][ticker]["currently_enabled"] = false;
                    out["adex_cfg"][ticker]["is_custom_coin"]    = true;
                    out["adex_cfg"][ticker]["mm2_backup"]        = out["mm2_cfg"];
                }
            }
            else
            {
                out["error_message"] = body;
                out["error_code"]    = resp.status_code();
            }
            SPDLOG_DEBUG("result json of fetch erc infos from contract address is: {}", out.dump(4));
            this->set_custom_token_data(nlohmann_json_object_to_qt_json_object(out));
            this->set_fetching_custom_token_data_busy(false);
        };
        ::mm2::api::async_process_rpc_get(*endpoint, "token_infos", url).then(answer_functor).then(&handle_exception_pplx_task);
    }

    bool
    settings_page::is_fetching_custom_token_data_busy() const
    {
        return m_fetching_erc_data_busy.load();
    }

    void
    settings_page::set_fetching_custom_token_data_busy(bool status)
    {
        if (m_fetching_erc_data_busy != status)
        {
            m_fetching_erc_data_busy = status;
            emit customTokenDataStatusChanged();
        }
    }

    QVariant
    settings_page::get_custom_token_data() const
    {
        return nlohmann_json_object_to_qt_json_object(m_custom_token_data.get());
    }

    void
    settings_page::set_custom_token_data(QVariant rpc_data)
    {
        nlohmann::json out  = nlohmann::json::parse(QString(QJsonDocument(rpc_data.toJsonObject()).toJson()).toStdString());
        m_custom_token_data = out;
        emit customTokenDataChanged();
    }

    void
    settings_page::submit()
    {
        SPDLOG_DEBUG("submit whole cfg");
        nlohmann::json out = m_custom_token_data.get();
        this->m_system_manager.get_system<mm2_service>().add_new_coin(out.at("adex_cfg"), out.at("mm2_cfg"));
        this->set_custom_token_data(QJsonObject{{}});
    }

    void
    settings_page::remove_custom_coin(const QString& ticker)
    {
        SPDLOG_DEBUG("remove ticker: {}", ticker.toStdString());
        this->m_system_manager.get_system<mm2_service>().remove_custom_coin(ticker.toStdString());
    }

    void
    settings_page::set_qml_engine(QQmlApplicationEngine* engine)
    {
        m_qml_engine = engine;
    }

    void
    settings_page::reset_coin_cfg()
    {
        using namespace std::string_literals;
        const std::string wallet_name                = qt_wallet_manager::get_default_wallet_name().toStdString();
        const std::string wallet_cfg_file            = std::string(atomic_dex::get_raw_version()) + "-coins"s + "."s + wallet_name + ".json"s;
        std::string       wallet_custom_cfg_filename = "custom-tokens."s + wallet_name + ".json"s;
        const fs::path    wallet_custom_cfg_path{utils::get_atomic_dex_config_folder() / wallet_custom_cfg_filename};
        const fs::path    wallet_cfg_path{utils::get_atomic_dex_config_folder() / wallet_cfg_file};
        const fs::path    mm2_coins_file_path{atomic_dex::utils::get_current_configs_path() / "coins.json"};
        const fs::path    ini_file_path      = atomic_dex::utils::get_current_configs_path() / "cfg.ini";
        const fs::path    cfg_json_file_path = atomic_dex::utils::get_current_configs_path() / "cfg.json";
        const fs::path    logo_path          = atomic_dex::utils::get_logo_path();
        const fs::path    theme_path         = atomic_dex::utils::get_themes_path();


        if (fs::exists(wallet_custom_cfg_path))
        {
            nlohmann::json custom_config_json_data;
            QFile          fs;
            fs.setFileName(std_path_to_qstring(wallet_custom_cfg_path));
            fs.open(QIODevice::ReadOnly | QIODevice::Text);

            //! Read Contents
            custom_config_json_data = nlohmann::json::parse(QString(fs.readAll()).toStdString());
            fs.close();

            //! Modify
            for (auto&& [key, value]: custom_config_json_data.items()) { value["active"] = false; }

            //! Write
            fs.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate);
            fs.write(QString::fromStdString(custom_config_json_data.dump()).toUtf8());
            fs.close();
        }

        const auto functor_remove = [](auto&& path_to_remove)
        {
            if (fs::exists(path_to_remove))
            {
                fs_error_code ec;
                if (fs::is_directory(path_to_remove))
                {
                    fs::remove_all(path_to_remove, ec);
                }
                else
                {
                    fs::remove(path_to_remove, ec);
                }
                if (ec)
                {
                    LOG_PATH("error when removing {}", path_to_remove);
                    SPDLOG_ERROR("error: {}", ec.message());
                }
                else
                {
                    LOG_PATH("Successfully removed {}", path_to_remove);
                }
            }
        };

        functor_remove(std::move(wallet_cfg_path));
        functor_remove(std::move(mm2_coins_file_path));
        functor_remove(std::move(ini_file_path));
        functor_remove(std::move(cfg_json_file_path));
        functor_remove(std::move(logo_path));
        functor_remove(std::move(theme_path));
    }

    QStringList
    settings_page::retrieve_seed(const QString& wallet_name, const QString& password)
    {
        QStringList     out;
        std::error_code ec;
        auto            key = atomic_dex::derive_password(password.toStdString(), ec);
        if (ec)
        {
            SPDLOG_ERROR("cannot derive the password: {}", ec.message());
            if (ec == dextop_error::derive_password_failed)
            {
                return {"wrong password"};
            }
        }
        using namespace std::string_literals;
        const fs::path seed_path = utils::get_atomic_dex_config_folder() / (wallet_name.toStdString() + ".seed"s);
        auto           seed      = atomic_dex::decrypt(seed_path, key.data(), ec);
        if (ec == dextop_error::corrupted_file_or_wrong_password)
        {
            SPDLOG_ERROR("cannot decrypt the seed with the derived password: {}", ec.message());
            return {"wrong password"};
        }

        if (!ec)
        {
            this->set_fetching_priv_key_busy(true);
            //! Also fetch private keys
            nlohmann::json batch   = nlohmann::json::array();
            const auto*    cfg_mdl = m_system_manager.get_system<portfolio_page>().get_global_cfg();
            const auto     coins   = cfg_mdl->get_enabled_coins();
            for (auto&& [coin, coin_cfg]: coins)
            {
                ::mm2::api::show_priv_key_request req{.coin = coin};
                nlohmann::json                    req_json = ::mm2::api::template_request("show_priv_key");
                to_json(req_json, req);
                batch.push_back(req_json);
            }
            auto&      mm2_system     = m_system_manager.get_system<mm2_service>();
            const auto answer_functor = [this](web::http::http_response resp)
            {
                std::string body = TO_STD_STR(resp.extract_string(true).get());
                if (resp.status_code() == 200)
                {
                    //!
                    auto answers = nlohmann::json::parse(body);
                    SPDLOG_WARN("Priv keys fetched, those are sensitive data.");
                    for (auto&& answer: answers)
                    {
                        auto       show_priv_key_answer = ::mm2::api::rpc_process_answer_batch<::mm2::api::show_priv_key_answer>(answer, "show_priv_key");
                        auto*      portfolio_mdl        = this->m_system_manager.get_system<portfolio_page>().get_portfolio();
                        const auto idx                  = portfolio_mdl->match(
                            portfolio_mdl->index(0, 0), portfolio_model::TickerRole, QString::fromStdString(show_priv_key_answer.coin), 1,
                            Qt::MatchFlag::MatchExactly);
                        if (not idx.empty())
                        {
                            update_value(portfolio_model::PrivKey, QString::fromStdString(show_priv_key_answer.priv_key), idx.at(0), *portfolio_mdl);
                        }
                    }
                }
                this->set_fetching_priv_key_busy(false);
            };
            mm2_system.get_mm2_client().async_rpc_batch_standalone(batch).then(answer_functor);
        }
        return {QString::fromStdString(seed), QString::fromStdString(::mm2::api::get_rpc_password())};
    }

    QString
    settings_page::get_version()
    {
        return QString::fromStdString(atomic_dex::get_version());
    }

    QString
    settings_page::get_log_folder()
    {
        return QString::fromStdString(utils::get_atomic_dex_logs_folder().string());
    }

    QString
    settings_page::get_mm2_version()
    {
        return QString::fromStdString(::mm2::api::rpc_version());
    }

    QString
    settings_page::get_export_folder()
    {
        return QString::fromStdString(utils::get_atomic_dex_export_folder().string());
    }

    bool
    settings_page::is_fetching_priv_key_busy() const
    {
        return m_fetching_priv_keys_busy.load();
    }
    void
    settings_page::set_fetching_priv_key_busy(bool status)
    {
        if (m_fetching_priv_keys_busy != status)
        {
            m_fetching_priv_keys_busy = status;
            emit privKeyStatusChanged();
        }
    }

    void
    settings_page::garbage_collect_qml()
    {
        SPDLOG_INFO("garbage_collect_qml");
        m_qml_engine->collectGarbage();
        // m_qml_engine->
        m_qml_engine->trimComponentCache();
        m_qml_engine->clearComponentCache();
    }
} // namespace atomic_dex
