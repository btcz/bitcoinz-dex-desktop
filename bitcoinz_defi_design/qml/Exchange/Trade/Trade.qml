import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import Qt.labs.settings 1.0
import QtGraphicalEffects 1.0

import Qaterial 1.0 as Qaterial

import AtomicDEX.MarketMode 1.0
import AtomicDEX.TradingError 1.0
import AtomicDEX.TradingMode 1.0

import "../../Components"
import "../../Wallet"

import "Trading/"

import "SimpleView" as SimpleView

import App 1.0

Item {
    id: exchange_trade
    readonly property string total_amount: API.app.trading_pg.total_amount
    property bool orderSelected: false
    //property var form_base: sell_mode? form_base.formBase : buyBox.formBase
    Component.onCompleted: {
        API.app.trading_pg.on_gui_enter_dex()
        if(dashboard.current_ticker!==undefined){
            onOpened(dashboard.current_ticker)
        }else {
            onOpened()
        }
        dashboard.current_ticker = undefined
    }

    Component.onDestruction: {
        API.app.trading_pg.on_gui_leave_dex()
    }
    property bool isUltraLarge: true // width > 1400
    property bool isBigScreen: width > 1400
    onIsUltraLargeChanged: {
        if (isUltraLarge) {
            API.app.trading_pg.orderbook.asks.proxy_mdl.qml_sort(
                        0, Qt.DescendingOrder)
        } else {
            API.app.trading_pg.orderbook.asks.proxy_mdl.qml_sort(
                        0, Qt.AscendingOrder)
        }
    }

    readonly property bool block_everything: swap_cooldown.running
                                             || fetching_multi_ticker_fees_busy

    readonly property bool fetching_multi_ticker_fees_busy: false

    signal prepareMultiOrder
    property bool multi_order_values_are_valid: true

    readonly property string non_null_price: backend_price === '' ? '0' : backend_price
    readonly property string non_null_volume: backend_volume === '' ? '0' : backend_volume
    readonly property bool price_is_empty: parseFloat(non_null_price) <= 0

    readonly property string backend_price: API.app.trading_pg.price
    function setPrice(v) {
        API.app.trading_pg.price = v
    }
    readonly property int last_trading_error: API.app.trading_pg.last_trading_error
    readonly property string max_volume: API.app.trading_pg.max_volume
    readonly property string backend_volume: API.app.trading_pg.volume
    function setVolume(v) {
        API.app.trading_pg.volume = v
    }

    property bool sell_mode: API.app.trading_pg.market_mode.toString(
                                 ) === "Sell"
    function setMarketMode(v) {
        API.app.trading_pg.market_mode = v
    }

    readonly property string base_amount: API.app.trading_pg.base_amount
    readonly property string rel_amount: API.app.trading_pg.rel_amount

    Timer {
        id: swap_cooldown
        repeat: false
        interval: 1000
    }

    function inCurrentPage() {
        return exchange.inCurrentPage()
                && exchange.current_page === idx_exchange_trade
    }

    readonly property var preffered_order: API.app.trading_pg.preffered_order



    // Cache Trade Info
    property bool valid_fee_info: API.app.trading_pg.fees.base_transaction_fees !== undefined
    readonly property var curr_fee_info: API.app.trading_pg.fees
    property var fees_data: []

    // Trade
    function onOpened(ticker) {

        if (!General.initialized_orderbook_pair) {
            General.initialized_orderbook_pair = true
            API.app.trading_pg.set_current_orderbook(General.default_base,
                                                     General.default_rel)
        }
        setPair(true, ticker)
        app.pairChanged(base_ticker, rel_ticker)
    }

    function setPair(is_left_side, changed_ticker) {
        swap_cooldown.restart()

        if (API.app.trading_pg.set_pair(is_left_side, changed_ticker))
            pairChanged(base_ticker, rel_ticker)
    }

    function trade(options, default_config) {
        // Will move to backend - nota, conf
        let nota = ""
        let confs = ""

        if (options.enable_custom_config) {
            if (options.is_dpow_configurable) {
                nota = options.enable_dpow_confs ? "1" : "0"
            }

            if (nota !== "1") {
                confs = options.required_confirmation_count.toString()
            }
        } else {
            if (General.exists(default_config.requires_notarization)) {
                nota = default_config.requires_notarization ? "1" : "0"
            }

            if (nota !== "1" && General.exists(
                        default_config.required_confirmations)) {
                confs = default_config.required_confirmations.toString()
            }
        }

        if (sell_mode)
            API.app.trading_pg.place_sell_order(nota, confs)
        else
            API.app.trading_pg.place_buy_order(nota, confs)

        orderPlaced()
    }

    signal orderPlaced()

    readonly property bool buy_sell_rpc_busy: API.app.trading_pg.buy_sell_rpc_busy
    readonly property var buy_sell_last_rpc_data: API.app.trading_pg.buy_sell_last_rpc_data

    Loader
    {
        id: _viewLoader
        anchors.fill: parent
        source: API.app.trading_pg.current_trading_mode == TradingMode.Pro ? "ProView.qml" : "SimpleView/Main.qml"
    }

    TradeViewHeader {
    }
}
