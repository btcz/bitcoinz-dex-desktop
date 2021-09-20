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

#include <QObject>

//! Deps
#include <entt/core/attribute.h>

namespace atomic_dex
{
    class ENTT_API CoinTypeGadget
    {
      public:
        Q_GADGET

      public:
        enum CoinTypeEnum
        {
            QRC20      = 0,
            ERC20      = 1,
            BEP20      = 2,
            UTXO       = 3,
            SmartChain = 4,
            SLP        = 5,
            Disabled   = 6,
            All        = 7,
            Size       = 8
        };

        Q_ENUM(CoinTypeEnum)

      private:
        explicit CoinTypeGadget();
    };
} // namespace atomic_dex

using CoinType = atomic_dex::CoinTypeGadget::CoinTypeEnum;