import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

import QtWebEngine 1.8

import QtGraphicalEffects 1.0
import QtCharts 2.3
import Qaterial 1.0 as Qaterial
import ModelHelper 0.1

import AtomicDEX.WalletChartsCategories 1.0

import "../Components"
import "../Constants"
import App 1.0

// Portfolio
Item {
    property alias pieTimer2: pieTimer
    function refresh() {
        pieSeries.clear()
        for (var i = 0; i < portfolio_mdl.pie_chart_proxy_mdl.rowCount(); i++) {
            let data = portfolio_mdl.pie_chart_proxy_mdl.get(i)
            addItem(data)
        }
    }
    function addItem(value) {
        var item = pieSeries.append(value.ticker, value.main_currency_balance)
        item.labelColor = 'white'
        item.color = Style.getCoinColor(value.ticker)
        item.borderColor = 'transparent'
        item.borderWidth = 2
        item.holeSize = 1
        item.labelFont = DexTypo.body2
        item.hovered.connect(function (state) {
            if (state) {
                item.explodeDistanceFactor = 0.03
                portfolio.currentTotal = API.app.settings_pg.current_fiat_sign + " " + value.main_currency_balance
                portfolio.currentValue = value.balance + " " + item.label
                item.color = Qt.lighter(Style.getCoinColor(value.ticker))
            } else {
                item.borderWidth = 2
                item.explodeDistanceFactor = 0.01
                portfolio.currentValue = ""
                portfolio.currentTotal = ""
                item.color = Style.getCoinColor(value.ticker)
            }
        })
    }
    Timer {
        id: pieTimer
        interval: 500
        onTriggered: {
            refresh()
        }
    }
    Gradient {
        id: gd
        GradientStop {
            position: .80
            color: DexTheme.contentColorTop
        }
        GradientStop {
            position: 1
            color: 'transparent'
        }
    }
    DexRectangle {
        id: bg
        y: 35
        height: parent.height
        width: parent.width
        anchors.centerIn: parent
        border.width: 0
        gradient: DexTheme.portfolioPieGradient ? gd : undefined
        
        RowLayout {
            anchors.fill: parent
            spacing: 0 
            Item {
                Layout.preferredWidth: (parent.width / 2) - 150
                Layout.fillHeight: true
                ChartView {
                    id: _chartView

                    width: 620
                    height: 620
                    theme: ChartView.ChartView.ChartThemeLight
                    antialiasing: true
                    legend.visible: false
                    smooth: true
                    scale: portfolio.isUltraLarge? 1 : 0.7
                    Behavior on scale {
                        NumberAnimation {
                            duration: 200
                        }
                    }
                    y: portfolio.isUltraLarge? -55 : -150
                    backgroundColor: 'transparent'

                    anchors.centerIn: parent
                    dropShadowEnabled: true

                    PieSeries {
                        id: pieSeries
                    }
                    DexRectangle {
                        anchors.centerIn: parent
                        color: DexTheme.theme === "light" ? DexTheme.contentColorTopBold : bg.color
                        width: 380
                        height: width
                        radius: width / 2
                        Column {
                            anchors.centerIn: parent
                            spacing: 5
                            DexLabel {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text_value: currentTotal !== "" ? currentTotal : General.formatFiat("", API.app.portfolio_pg.balance_fiat_all, API.app.settings_pg.current_currency)
                                font: DexTypo.head4
                                color: currency_change_button.containsMouse ? DexTheme.foregroundColor : DexTheme.foregroundColorDarkColor3
                                privacy: true
                                DexFadebehavior on text {
                                    fadeDuration: 100
                                }
                                Component.onCompleted: {
                                    font.family = 'Lato'
                                }
                            }
                            DexLabel {
                                anchors.horizontalCenter: parent.horizontalCenter
                                text_value: portfolio.currentValue
                                font: DexTypo.body2
                                DexFadebehavior on text {
                                    fadeDuration: 100
                                }
                                color: Qt.lighter(
                                           DexTheme.foregroundColor, 0.6)
                                privacy: true
                                Component.onCompleted: {
                                    font.family = 'Lato'
                                }
                            }
                            DexLabel {
                                id: count_label
                                anchors.horizontalCenter: parent.horizontalCenter
                                text_value: portfolio_helper.count + " " + qsTr(
                                                "Assets")
                                font: DexTypo.body2
                                DexFadebehavior on text {
                                    fadeDuration: 100
                                }
                                color: Qt.lighter(
                                           DexTheme.foregroundColor, 0.8)
                                privacy: true
                                visible: portfolio.currentValue == ""

                                Component.onCompleted: {
                                    font.family = 'Lato'
                                }
                            }
                        }
                        DefaultMouseArea {
                            id: currency_change_button

                            width: parent.width - 100
                            height: parent.height - 100
                            anchors.centerIn: parent

                            hoverEnabled: true
                            onClicked: {
                                const current_fiat = API.app.settings_pg.current_currency
                                const available_fiats = API.app.settings_pg.get_available_currencies()
                                const current_index = available_fiats.indexOf(
                                                        current_fiat)
                                const next_index = (current_index + 1)
                                                 % available_fiats.length
                                const next_fiat = available_fiats[next_index]
                                API.app.settings_pg.current_currency = next_fiat
                            }
                        }
                    }
                    Rectangle {
                        anchors.centerIn: parent
                        width: 295
                        height: width
                        color: 'transparent'
                        radius: width/2
                        border.width: API.app.portfolio_pg.balance_fiat_all > 0 ? 0 : 5
                        border.color: Qt.lighter(DexTheme.contentColorTop)
                    }
                }
            }
            Item {
                Layout.preferredWidth: (parent.width / 2) + 150
                Layout.fillHeight: true
                Item {
                    scale: portfolio.isUltraLarge ? 1 : 0.95
                    y: 10 
                    Behavior on scale {
                        NumberAnimation {
                            duration: 200
                        }
                    }
                    width: parent.width-100
                    anchors.verticalCenter: parent.verticalCenter
                    height: !portfolio.isUltraLarge? _pieColumn.contentHeight > 300 ? 300 : _pieColumn.contentHeight : _pieColumn.contentHeight > 550 ? 550 :_pieColumn.contentHeight

                    anchors.horizontalCenter: parent.horizontalCenter

                    DexListView {
                        id: _pieColumn
                        anchors.fill: parent
                        clip: true
                        model: portfolio_mdl.pie_chart_proxy_mdl
                        scrollbar_visible: false
                        delegate: RowLayout {
                            id: rootItem
                            property color itemColor: Style.getCoinColor(
                                                          ticker)
                            width: _pieColumn.width
                            height: 50
                            spacing: 20
                            DexLabel {
                                Layout.preferredWidth: 60
                                text: ticker
                                font.bold: true
                                Layout.alignment: Qt.AlignVCenter
                                Component.onCompleted: font.weight = Font.Bold
                            }

                            Item {
                                Layout.alignment: Qt.AlignVCenter
                                Layout.fillWidth: true
                                height: 5
                                Rectangle {
                                    anchors.fill: parent
                                    radius: 10
                                    opacity: 0.1
                                    color: DexTheme.foregroundColorLightColor5
                                    
                                }
                                Rectangle {
                                    height: parent.height
                                    width: (parseFloat(percent_main_currency) * parent.width) / 100
                                    radius: 10
                                    color: rootItem.itemColor
                                }
                            }

                            

                            DexLabel {
                                
                                text: percent_main_currency + " %"

                                Layout.alignment: Qt.AlignVCenter
                                
                                Component.onCompleted: font.family = 'lato'
                            }
                        }
                    }

                }
            }
        }
        
        


       
    }
}
