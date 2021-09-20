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

//! Qt
#include <QDebug>

//! Project
#include "atomicdex/models/qt.orderbook.model.hpp"
#include "atomicdex/models/qt.orderbook.proxy.model.hpp"
#include "atomicdex/utilities/global.utilities.hpp"
#include "atomicdex/services/price/coingecko/coingecko.provider.hpp"
#include "atomicdex/pages/qt.trading.page.hpp"

namespace atomic_dex
{
    orderbook_proxy_model::orderbook_proxy_model(ag::ecs::system_manager& system_manager, QObject* parent) :
        QSortFilterProxyModel(parent), m_system_mgr(system_manager)
    {
    }

    bool
    orderbook_proxy_model::lessThan(const QModelIndex& source_left, const QModelIndex& source_right) const
    {
        if (!source_left.isValid() || !source_right.isValid())
        {
            SPDLOG_WARN("one of the index is invalid - skipping -> role: {}", this->sortRole());
            return false;
        }
        int      role       = this->sortRole();
        QVariant left_data  = sourceModel()->data(source_left, role);
        QVariant right_data = sourceModel()->data(source_right, role);

        switch (static_cast<atomic_dex::orderbook_model::OrderbookRoles>(role))
        {
        case orderbook_model::PriceRole:
            return safe_float(left_data.toString().toStdString()) < safe_float(right_data.toString().toStdString());
        case orderbook_model::QuantityRole:
            break;
        case orderbook_model::TotalRole:
            break;
        case orderbook_model::UUIDRole:
            break;
        case orderbook_model::IsMineRole:
            break;
        case orderbook_model::PriceDenomRole:
            break;
        case orderbook_model::PriceNumerRole:
            break;
        case orderbook_model::PercentDepthRole:
            break;
        case orderbook_model::QuantityDenomRole:
            break;
        case orderbook_model::QuantityNumerRole:
            break;
        case orderbook_model::CoinRole:
            break;
        case orderbook_model::MinVolumeRole:
            break;
        case orderbook_model::BaseMinVolumeRole:
            break;
        case orderbook_model::BaseMinVolumeDenomRole:
            break;
        case orderbook_model::BaseMinVolumeNumerRole:
            break;
        case orderbook_model::BaseMaxVolumeRole:
            break;
        case orderbook_model::BaseMaxVolumeDenomRole:
            break;
        case orderbook_model::BaseMaxVolumeNumerRole:
            break;
        case orderbook_model::RelMinVolumeRole:
            break;
        case orderbook_model::RelMinVolumeDenomRole:
            break;
        case orderbook_model::RelMinVolumeNumerRole:
            break;
        case orderbook_model::RelMaxVolumeRole:
            break;
        case orderbook_model::RelMaxVolumeDenomRole:
            break;
        case orderbook_model::RelMaxVolumeNumerRole:
            break;
        case orderbook_model::EnoughFundsToPayMinVolume:
            break;
        case orderbook_model::CEXRatesRole:
        {
            t_float_50 left  = safe_float(left_data.toString().toStdString());
            t_float_50 right = safe_float(right_data.toString().toStdString());
            const bool is_buy = this->m_system_mgr.get_system<trading_page>().get_market_mode() == MarketMode::Buy;
            return !is_buy ? left > right : left < right;
        }
        case orderbook_model::SendRole:
            break;
        case orderbook_model::HaveCEXIDRole:
            break;
        case orderbook_model::NameAndTicker:
            break;
        case orderbook_model::PriceFiatRole:
            t_float_50 left  = safe_float(left_data.toString().toStdString());
            t_float_50 right = safe_float(right_data.toString().toStdString());
            return left < right;
        }
        return true;
    }

    void
    orderbook_proxy_model::qml_sort(int column, Qt::SortOrder order)
    {
        this->sort(column, order);
    }

    bool
    orderbook_proxy_model::filterAcceptsRow(int source_row, const QModelIndex& source_parent) const
    {
        [[maybe_unused]] QModelIndex idx = this->sourceModel()->index(source_row, 0, source_parent);
        assert(this->sourceModel()->hasIndex(idx.row(), 0));
        auto* orderbook = qobject_cast<orderbook_model*>(this->sourceModel());

        if (orderbook != nullptr)
        {
            switch (orderbook->get_orderbook_kind())
            {
            case orderbook_model::kind::asks:
            case orderbook_model::kind::bids:
                break;
            case orderbook_model::kind::best_orders:
                t_float_50  rates          = safe_float(this->sourceModel()->data(idx, orderbook_model::CEXRatesRole).toString().toStdString());
                t_float_50  fiat_price     = safe_float(this->sourceModel()->data(idx, orderbook_model::PriceFiatRole).toString().toStdString());
                std::string ticker         = this->sourceModel()->data(idx, orderbook_model::CoinRole).toString().toStdString();
                const auto& gecko_provider = this->m_system_mgr.get_system<coingecko_provider>();
                t_float_50  limit("10000");

                if (rates > 100 || fiat_price <= 0 || safe_float(gecko_provider.get_total_volume(ticker)) < limit)
                {
                    return false;
                }
                break;
            }
        }

        if (orderbook != nullptr && orderbook->get_orderbook_kind() == orderbook_model::kind::best_orders)
        {
            bool is_cex_id_available = this->sourceModel()->data(idx, orderbook_model::HaveCEXIDRole).toBool();
            if (!is_cex_id_available)
            {
                return false;
            }
        }
        return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
    }
} // namespace atomic_dex
