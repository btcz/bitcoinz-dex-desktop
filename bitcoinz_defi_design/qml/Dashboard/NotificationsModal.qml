import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15
import Qt.labs.platform 1.0
import Qaterial 1.0 as Qaterial

import "../Constants"
import App 1.0
import "../Components"

DexPopup {
    id: root

    width: 400
    height: 440

    property
    var notification_map: [{
        icon: Qaterial.Icons.arrowUpCircleOutline,
        color: DexTheme.redColor
    }, {
        icon: Qaterial.Icons.arrowDownCircleOutline,
        color: DexTheme.greenColor
    }, {
        icon: Qaterial.Icons.emailOutline,
        color: DexTheme.foregroundColor
    }]
    backgroundColor: Qt.darker(DexTheme.dexBoxBackgroundColor, 0.9)

    function reset() {
        notifications_list = []
        root.close()
    }
    enum NotificationKind {
        Send,
        Receive,
        Others
    }

    function showApp() {
        switch (window.real_visibility) {
            case 4:
                window.showMaximized()
                break
            case 5:
                window.showFullScreen()
                break
            default:
                window.show()
                break
        }
        window.raise()
        window.requestActivate()
    }

    function performNotificationAction(notification) {
        root.close()

        switch (notification.click_action) {
            case "open_notifications":
                root.open()
                break
            case "open_wallet_page":
                api_wallet_page.ticker = notification.params.ticker
                dashboard.current_page = idx_dashboard_wallet
                break
            case "open_swaps_page":
                dashboard.current_page = idx_dashboard_exchange

                dashboard.loader.onLoadComplete = () => {
                    dashboard.current_component.current_page = dashboard.isSwapDone(notification.params.new_swap_status) ? idx_exchange_history : idx_exchange_orders
                }
                break
            case "open_log_modal":
                showError(notification.title, notification.long_message)
                break
            default:
                console.log("Unknown notification click action", notification.click_action)
                break
        }
    }

    function newNotification(event_name, params, id, title, message, human_date, click_action = "open_notifications", long_message = "") {

        let obj;
        if (title.indexOf("You received") !== -1) {
            obj = {
                event_name,
                params,
                id,
                title,
                message,
                human_date,
                click_action,
                long_message,
                kind: NotificationsModal.NotificationKind.Receive
            }
        } else if (title.indexOf("You sent") !== -1) {
            obj = {
                event_name,
                params,
                id,
                title,
                message,
                human_date,
                click_action,
                long_message,
                kind: NotificationsModal.NotificationKind.Send
            }
        } else {
            obj = {
                event_name,
                params,
                id,
                title,
                message,
                human_date,
                click_action,
                long_message,
                kind: NotificationsModal.NotificationKind.Others
            }
        }

        // Update if it already exists
        let updated_existing_one = false
        for (let i = 0; i < notifications_list.length; ++i) {
            if (notifications_list[i].id === obj.id) {
                notifications_list[i] = General.clone(obj)
                updated_existing_one = true
                break
            }
        }

        // Add new line
        if (!updated_existing_one) {
            notifications_list = [obj].concat(notifications_list)
        }

        // Display OS notification
        displayMessage(obj.title, obj.message)

        // Refresh the list if updated an existing one
        if (updated_existing_one)
            notifications_list = notifications_list
    }


    function getOrderStatusText(status, short_text = false) {
        switch (status) {
            case "matching":
                return short_text ? qsTr("Matching") : qsTr("Order Matching")
            case "matched":
                return short_text ? qsTr("Matched") : qsTr("Order Matched")
            case "ongoing":
                return short_text ? qsTr("Ongoing") : qsTr("Swap Ongoing")
            case "successful":
                return short_text ? qsTr("Successful") : qsTr("Swap Successful")
            case "refunding":
                return short_text ? qsTr("Refunding") : qsTr("Refunding")
            case "failed":
                return short_text ? qsTr("Failed") : qsTr("Swap Failed")
            default:
                return short_text ? qsTr("Unknown") : qsTr("Unknown State")
        }
    }

    // Events
    function onUpdateSwapStatus(old_swap_status, new_swap_status, swap_uuid, base_coin, rel_coin, human_date) {
        newNotification("onUpdateSwapStatus", {
                old_swap_status,
                new_swap_status,
                swap_uuid,
                base_coin,
                rel_coin,
                human_date
            },
            swap_uuid,
            base_coin + "/" + rel_coin + " - " + qsTr("Swap status updated"),
            getOrderStatusText(old_swap_status) + " " + General.right_arrow_icon + " " + getOrderStatusText(new_swap_status),
            human_date,
            "open_swaps_page")
    }

    function onBalanceUpdateStatus(am_i_sender, amount, ticker, human_date, timestamp) {
        const change = General.formatCrypto("", amount, ticker)
        if(!app.segwit_on) {
            newNotification("onBalanceUpdateStatus", {
                am_i_sender,
                amount,
                ticker,
                human_date,
                timestamp
            },
            timestamp,
            am_i_sender ? qsTr("You sent %1").arg(change) : qsTr("You received %1").arg(change),
            qsTr("Your wallet balance changed"),
            human_date,
            "open_wallet_page")
        } else { app.segwit_on = false } 
    }

    readonly property string check_internet_connection_text: qsTr("Please check your internet connection (e.g. VPN service or firewall might block it).")
    function onEnablingCoinFailedStatus(coin, error, human_date, timestamp) {
        // Check if there is mismatch error, ignore this one
        for (let n of notifications_list) {
            if (n.event_name === "onMismatchCustomCoinConfiguration" && n.params.asset === coin) {
                console.log("Ignoring onEnablingCoinFailedStatus event because onMismatchCustomCoinConfiguration exists for", coin)
                return
            }
        }

        // Display the notification
        const title = qsTr("Failed to enable %1", "TICKER").arg(coin)

        error = check_internet_connection_text + "\n\n" + error

        newNotification("onEnablingCoinFailedStatus", {
                coin,
                error,
                human_date,
                timestamp
            },
            timestamp,
            title,
            check_internet_connection_text,
            human_date,
            "open_log_modal",
            error)

        toast.show(title, General.time_toast_important_error, error)
    }

    function onEndpointNonReacheableStatus(base_uri, human_date, timestamp) {
        const title = qsTr("Endpoint not reachable")

        const error = qsTr("Could not reach to endpoint") + ". " + check_internet_connection_text + "\n\n" + base_uri

        newNotification("onEndpointNonReacheableStatus", {
                base_uri,
                human_date,
                timestamp
            },
            timestamp,
            title,
            base_uri,
            human_date,
            "open_log_modal",
            error)

        toast.show(title, General.time_toast_important_error, error)
    }

    function onMismatchCustomCoinConfiguration(asset, human_date, timestamp) {
        const title = qsTr("Mismatch at %1 custom asset configuration", "TICKER").arg(asset)

        newNotification("onMismatchCustomCoinConfiguration", {
                asset,
                human_date,
                timestamp
            },
            timestamp,
            title,
            qsTr("Application needs to be restarted for %1 custom asset.", "TICKER").arg(asset),
            human_date)

        toast.show(title, General.time_toast_important_error, "", true, true)
    }

    function onBatchFailed(reason, from, human_date, timestamp) {
        const title = qsTr("Batch %1 failed. Reason: %2").arg(from).arg(reason)

        newNotification("onBatchFailed", {
                human_date,
                timestamp
            },
            timestamp,
            title,
            reason,
            human_date)

        toast.show(title, General.time_toast_important_error, reason)
    }

    // System
    Component.onCompleted: {
        API.app.notification_mgr.updateSwapStatus.connect(onUpdateSwapStatus)
        API.app.notification_mgr.balanceUpdateStatus.connect(onBalanceUpdateStatus)
        API.app.notification_mgr.enablingCoinFailedStatus.connect(onEnablingCoinFailedStatus)
        API.app.notification_mgr.endpointNonReacheableStatus.connect(onEndpointNonReacheableStatus)
        API.app.notification_mgr.mismatchCustomCoinConfiguration.connect(onMismatchCustomCoinConfiguration)
        API.app.notification_mgr.batchFailed.connect(onBatchFailed)
    }
    Component.onDestruction: {
        API.app.notification_mgr.updateSwapStatus.disconnect(onUpdateSwapStatus)
        API.app.notification_mgr.balanceUpdateStatus.disconnect(onBalanceUpdateStatus)
        API.app.notification_mgr.enablingCoinFailedStatus.disconnect(onEnablingCoinFailedStatus)
        API.app.notification_mgr.endpointNonReacheableStatus.disconnect(onEndpointNonReacheableStatus)
        API.app.notification_mgr.mismatchCustomCoinConfiguration.disconnect(onMismatchCustomCoinConfiguration)
        API.app.notification_mgr.batchFailed.disconnect(onBatchFailed)
    }

    function displayMessage(title, message) {
        if (API.app.settings_pg.notification_enabled)
            tray.showMessage(title, message)
    }
    SystemTrayIcon {
        id: tray
        visible: true
        iconSource: General.image_path + "dex-tray-icon.png"

        tooltip: API.app_name
        onMessageClicked: {
            if (notifications_list.length > 0)
                performNotificationAction(notifications_list[0])
            showApp()
        }
        menu: Menu {
            MenuItem {
                text: qsTr("Show")
                onTriggered: showApp()
            }

            MenuItem {
                text: qsTr("Restart")
                onTriggered: API.app.restart()
            }

            MenuItem {
                text: qsTr("Quit")
                onTriggered: Qt.quit()
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        Item {
            Layout.preferredHeight: 65
            Layout.fillWidth: parent
            RowLayout {
                anchors.fill: parent
                DexLabel {
                    Layout.alignment: Qt.AlignVCenter
                    Layout.fillWidth: true
                    leftPadding: 15
                    font: DexTypo.head6
                    text: "Notifications"
                }
                Qaterial.AppBarButton {
                    enabled: list.count > 0
                    Layout.alignment: Qt.AlignVCenter
                    foregroundColor: DexTheme.foregroundColor
                    icon.source: Qaterial.Icons.checkAll
                    onClicked: notifications_list = []
                }
            }
            Rectangle {
                height: 2
                color: DexTheme.foregroundColor
                opacity: .05
                width: parent.width - 20
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
            }
        }

        Item {
            Layout.fillWidth: true
            Layout.fillHeight: true

            DefaultText {
                anchors.centerIn: parent
                visible: !list.visible
                text_value: qsTr("There isn't any notification")
                font.pixelSize: Style.textSizeSmall2
            }

            DefaultListView {
                id: list
                visible: notifications_list.length !== 0
                anchors.fill: parent
                model: notifications_list
                delegate: Rectangle {
                    color: mouseArea.containsMouse ? DexTheme.dexBoxBackgroundColor : 'transparent'
                    function removeNotification() {
                        notifications_list.splice(index, 1)
                        notifications_list = notifications_list
                    }
                    height: _column.height + 10
                    width: list.width - 10
                    MouseArea {
                        id: mouseArea
                        hoverEnabled: true
                        anchors.fill: parent
                        onClicked: {
                            performNotificationAction(notifications_list[index])
                            removeNotification()
                        }
                    }
                    RowLayout {
                        anchors.fill: parent
                        Item {
                            Layout.fillHeight: true
                            Layout.preferredWidth: 60
                            Qaterial.ColorIcon {
                                anchors.verticalCenter: parent.verticalCenter
                                source: notification_map[modelData.kind].icon
                                iconSize: 32
                                x: 10
                                color: notification_map[modelData.kind].color
                                opacity: .6
                            }
                        }
                        VerticalLine {
                            Layout.preferredHeight: 50
                            Layout.preferredWidth: 1
                        }
                        Item {
                            Layout.fillHeight: true
                            Layout.fillWidth: true
                            Column {
                                id: _column
                                width: parent.width
                                leftPadding: 15
                                topPadding: 5
                                bottomPadding: 5
                                spacing: 5
                                DexLabel {
                                    text: modelData.title
                                    font: DexTypo.body1
                                    width: parent.width
                                    wrapMode: Label.Wrap
                                }
                                DexLabel {
                                    text: modelData.message
                                    font: DexTypo.subtitle2
                                    width: parent.width
                                    wrapMode: Label.Wrap
                                    color: DexTheme.accentColor
                                }
                                DexLabel {
                                    text: modelData.human_date
                                    font: DexTypo.caption
                                }

                            }
                            Qaterial.AppBarButton {
                                id: action_button
                                scale: .6
                                anchors.bottom: parent.bottom
                                anchors.right: parent.right
                                anchors.bottomMargin: -4
                                foregroundColor: DexTheme.foregroundColor
                                icon.source: {
                                    let name
                                    switch (modelData.event_name) {
                                        case "onEnablingCoinFailedStatus":
                                            name = "repeat"
                                            break
                                        case "onMismatchCustomCoinConfiguration":
                                            name = "restart-alert"
                                            break
                                        default:
                                            name = "check"
                                            break
                                    }

                                    return General.qaterialIcon(name)
                                }

                                function removeNotification() {
                                    notifications_list.splice(index, 1)
                                    notifications_list = notifications_list
                                }

                                onClicked: {
                                    // Action might create another event so we save it and then remove the current one, then take the action
                                    const event_before_removal = General.clone(modelData)

                                    // Action
                                    switch (event_before_removal.event_name) {
                                        case "onEnablingCoinFailedStatus":
                                            removeNotification()
                                            console.log("Retrying to enable", event_before_removal.params.coin, "asset...")
                                            API.app.enable_coins([event_before_removal.params.coin])
                                            break
                                        case "onMismatchCustomCoinConfiguration":
                                            console.log("Restarting for", event_before_removal.params.asset, "custom asset configuration mismatch...")
                                            root.close()
                                            restart_modal.open()
                                            break
                                        default:
                                            removeNotification()
                                            break
                                    }
                                }
                            }
                        }
                    }

                    Rectangle {
                        height: 2
                        color: DexTheme.foregroundColor
                        opacity: .05
                        visible: !(list.count == index + 1)
                        width: parent.width - 20
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.bottom: parent.bottom
                    }
                }
            }
        }
    }
}
