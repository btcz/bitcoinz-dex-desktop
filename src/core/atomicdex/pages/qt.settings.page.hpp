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

//! QT
#include <QApplication>
#include <QObject>
#include <QQmlApplicationEngine>
#include <QString>
#include <QTranslator>

//! Deps
#include <antara/gaming/ecs/system.manager.hpp>
#include <boost/thread/synchronized_value.hpp>
#include <nlohmann/json.hpp>

//! Project headers
#include "atomicdex/config/app.cfg.hpp"
#include "atomicdex/constants/qt.coins.enums.hpp"

namespace atomic_dex
{
    class settings_page final : public QObject, public ag::ecs::pre_update_system<settings_page>
    {
        //! Q_Object definition
        Q_OBJECT

        //! Properties
        Q_PROPERTY(QString lang READ get_current_lang WRITE set_current_lang NOTIFY onLangChanged)
        Q_PROPERTY(QString current_currency READ get_current_currency WRITE set_current_currency NOTIFY onCurrencyChanged)
        Q_PROPERTY(QString current_currency_sign READ get_current_currency_sign NOTIFY onCurrencySignChanged)
        Q_PROPERTY(QString current_fiat_sign READ get_current_fiat_sign NOTIFY onFiatSignChanged)
        Q_PROPERTY(QString current_fiat READ get_current_fiat WRITE set_current_fiat NOTIFY onFiatChanged)
        Q_PROPERTY(bool notification_enabled READ is_notification_enabled WRITE set_notification_enabled NOTIFY onNotificationEnabledChanged)
        Q_PROPERTY(QVariant custom_token_data READ get_custom_token_data WRITE set_custom_token_data NOTIFY customTokenDataChanged)
        Q_PROPERTY(bool fetching_custom_token_data_busy READ is_fetching_custom_token_data_busy WRITE set_fetching_custom_token_data_busy NOTIFY
                       customTokenDataStatusChanged)
        Q_PROPERTY(bool fetching_priv_keys_busy READ is_fetching_priv_key_busy WRITE set_fetching_priv_key_busy NOTIFY privKeyStatusChanged)

        using t_synchronized_json = boost::synchronized_value<nlohmann::json>;

        //! Private member fields Fields
        ag::ecs::system_manager&      m_system_manager;
        std::shared_ptr<QApplication> m_app;
        QQmlApplicationEngine*        m_qml_engine{nullptr};
        atomic_dex::cfg               m_config{load_cfg()};
        QTranslator                   m_translator;
        std::atomic_bool              m_fetching_erc_data_busy{false};
        std::atomic_bool              m_fetching_priv_keys_busy{false};
        t_synchronized_json           m_custom_token_data;

      public:
        explicit settings_page(entt::registry& registry, ag::ecs::system_manager& system_manager, std::shared_ptr<QApplication> app, QObject* parent = nullptr);
        ~settings_page() final = default;

        //! Public override
        void update() final;

        //! Properties
        [[nodiscard]] QString  get_current_lang() const;
        void                   set_current_lang(QString new_lang);
        [[nodiscard]] QString  get_current_currency() const;
        [[nodiscard]] QString  get_current_currency_sign() const;
        [[nodiscard]] QString  get_current_fiat_sign() const;
        [[nodiscard]] QString  get_current_fiat() const;
        [[nodiscard]] bool     is_notification_enabled() const;
        void                   set_notification_enabled(bool is_enabled);
        void                   set_current_currency(const QString& current_currency);
        void                   set_current_fiat(const QString& current_fiat);
        [[nodiscard]] bool     is_fetching_custom_token_data_busy() const;
        void                   set_fetching_custom_token_data_busy(bool status);
        [[nodiscard]] QVariant get_custom_token_data() const;
        void                   set_custom_token_data(QVariant rpc_data);
        [[nodiscard]] bool     is_fetching_priv_key_busy() const;
        void                   set_fetching_priv_key_busy(bool status);

        //! Public API
        [[nodiscard]] atomic_dex::cfg&       get_cfg();
        [[nodiscard]] const atomic_dex::cfg& get_cfg() const;
        void                                 init_lang();
        void                                 set_qml_engine(QQmlApplicationEngine* engine);
        void                                 garbage_collect_qml();

        // QML API
        Q_INVOKABLE void                      remove_custom_coin(const QString& ticker);
        Q_INVOKABLE [[nodiscard]] QStringList get_available_langs() const;
        Q_INVOKABLE [[nodiscard]] QStringList get_available_fiats() const;
        Q_INVOKABLE [[nodiscard]] QStringList get_recommended_fiats() const; // Returns 6 recommended fiats. Basically, the first 6 values in cfg.json.
        Q_INVOKABLE [[nodiscard]] QStringList get_available_currencies() const;
        Q_INVOKABLE [[nodiscard]] bool        is_this_ticker_present_in_raw_cfg(const QString& ticker) const;
        Q_INVOKABLE [[nodiscard]] bool        is_this_ticker_present_in_normal_cfg(const QString& ticker) const;
        Q_INVOKABLE [[nodiscard]] QString     get_custom_coins_icons_path() const;
        Q_INVOKABLE void process_token_add(const QString& contract_address, const QString& coingecko_id, const QString& icon_filepath, CoinType coin_type);
        Q_INVOKABLE void process_qrc_20_token_add(const QString& contract_address, const QString& coingecko_id, const QString& icon_filepath);
        Q_INVOKABLE void submit();
        Q_INVOKABLE void reset_coin_cfg();
        Q_INVOKABLE QStringList    retrieve_seed(const QString& wallet_name, const QString& password);
        Q_INVOKABLE static QString get_mm2_version();
        Q_INVOKABLE static QString get_log_folder();
        Q_INVOKABLE static QString get_export_folder();
        Q_INVOKABLE static QString get_version();

        // QML API Properties Signals
      signals:
        void onLangChanged();
        void langChanged();
        void onCurrencyChanged();
        void onCurrencySignChanged();
        void onFiatSignChanged();
        void onFiatChanged();
        void onNotificationEnabledChanged();
        void customTokenDataChanged();
        void customTokenDataStatusChanged();
        void privKeyStatusChanged();
    };
} // namespace atomic_dex

REFL_AUTO(type(atomic_dex::settings_page))
