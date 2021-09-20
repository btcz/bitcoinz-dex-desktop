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

#include "atomicdex/pages/widgets/dex/qt.multi.orders.widget.hpp"

namespace atomic_dex
{
    qt_multi_orders_widget::qt_multi_orders_widget(antara::gaming::ecs::system_manager& system_manager, QObject* parent)  :
        QObject(parent), m_system_mgr(system_manager)

    {
        SPDLOG_INFO("qt_orders_widget created");
    }

    qt_multi_orders_widget::~qt_multi_orders_widget()  { SPDLOG_INFO("qt_orders widget destroyed"); }

} // namespace atomic_dex
