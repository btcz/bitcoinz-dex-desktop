// Qt Imports
import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

// Project Imports
import "../Components"
import "../Constants"
import App 1.0

BasicModal {
    id: root

    width: 500
    height: 400

    property string ticker: api_wallet_page.ticker
    property var selected_address: ""

    ModalContent {
        title: qsTr("Select a contact with an %1 address").arg(ticker)

        DefaultTextField { // Searchbar
            Layout.fillWidth: true
            placeholderText: qsTr("Search for contacts...")
            onTextChanged: API.app.addressbook_pg.model.proxy.search_exp = text
            Component.onDestruction: API.app.addressbook_pg.model.proxy.search_exp = ""
        }

        // Contact List
        InnerBackground { // Contact List Background
            id: contact_list_bg

            Layout.fillWidth: true
            shadowOff: true
            radius: 4
            color: 'transparent'

            content: DefaultListView { // Contact List
                readonly property int rowHeight: 30 // Visual height of a row.

                height: rowHeight * 6
                Component.onCompleted: {
                    API.app.addressbook_pg.model.proxy.type_filter = ticker
                    console.debug("SendModal: Show contact list of given ticker %1 and of size %2"
                                    .arg(API.app.addressbook_pg.model.proxy.type_filter)
                                    .arg(count))
                }
                Component.onDestruction: {
                    API.app.addressbook_pg.model.proxy.type_filter = ""
                    console.debug("SendModal: Destroying contact list")
                }
                model: API.app.addressbook_pg.model.proxy
                delegate: AnimatedRectangle {
                    property int addressesCount
                    property var contactModel: modelData

                    Component.onCompleted: {
                        modelData.proxy_filter.filter_type = ticker
                        addressesCount = modelData.proxy_filter.rowCount()
                        console.debug("SendModal: Apply %1 filter to contact %2. It has %3 %4 addresses"
                                        .arg(ticker)
                                        .arg(modelData.name)
                                        .arg(addressesCount)
                                        .arg(modelData.proxy_filter.filter_type))
                    }

                    Component.onDestruction: {
                        contactModel.proxy_filter.filter_type = ""
                        console.debug("SendModal: Remove %1 filter to contact %2"
                                        .arg(ticker)
                                        .arg(contactModel.name))
                    }
                    implicitWidth: contact_list_bg.width
                    height: 30
                    color: mouse_area.containsMouse ? DexTheme.buttonColorHovered : index % 2 === 0 ? DexTheme.contentColorTopBold : "transparent"

                    DexMouseArea {
                        id: mouse_area
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            addresses_view.contactModel = modelData
                            root.currentIndex = 1
                        }
                    }

                    DefaultText { // Contact Name
                        width: 400
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        text: modelData.name
                        elide: Qt.ElideRight
                    }

                    DefaultText { // Contact Addresses Count
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 18
                        text: addressesCount > 1 ? qsTr("%1 addresses").arg(addressesCount) :
                                                   qsTr("1 address")
                    }

                    HorizontalLine {
                        width: parent.width
                        height: 2
                        anchors.bottom: parent.bottom
                    }
                }
            }
        }

        DefaultButton { // Back to Send Modal Button
            Layout.fillWidth: true
            text: qsTr("Back")
            onClicked: close()
        }
    }

    ModalContent {
        id: addresses_view

        readonly property var defaultContactModel:
        {
            "proxy_filter":
            {
                "filter_type": ""
            },
            "name": ""
        }
        property var contactModel: defaultContactModel

        title: qsTr("Choose an %1 address of %2")
                   .arg(contactModel.proxy_filter.filter_type)
                   .arg(contactModel.name)

        RowLayout {
            Layout.fillWidth: true

            DefaultText {
                Layout.leftMargin: 5
                Layout.preferredWidth: 210
                text: qsTr("Name")
                color: DexTheme.foregroundColor
                opacity: .7
            }

            DefaultText {
                text: qsTr("Address")
                color: DexTheme.foregroundColor
                opacity: .7
            }
        }

        // Address List
        InnerBackground { // Address List Background
            id: address_list_bg

            Layout.fillWidth: true
            shadowOff: true
            radius: 4
            color: 'transparent'

            content: DefaultListView { // Address List
                readonly property int rowHeight: 30 // Visual height of a row.

                height: rowHeight * 6
                model: addresses_view.contactModel.proxy_filter
                delegate: AnimatedRectangle { // Address Row
                    implicitWidth: address_list_bg.width
                    height: 30
                    color: address_mouse_area.containsMouse ? DexTheme.buttonColorHovered : index % 2 === 0 ? DexTheme.contentColorTopBold : "transparent"
                    radius: address_list_bg.radius

                    DexMouseArea {
                        id: address_mouse_area
                        anchors.fill: parent
                        hoverEnabled: true
                        onClicked: {
                            selected_address = model.address_value
                            close()
                        }
                    }

                    DefaultText { // Address Key
                        width: 150
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left
                        anchors.leftMargin: 10
                        text: model.address_key
                        elide: Qt.ElideRight
                    }

                    DefaultText { // Address Value
                        width: 220
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.right: parent.right
                        anchors.rightMargin: 10
                        text: model.address_value
                        elide: Qt.ElideRight
                    }
                }
            }
        }

        DexAppButton {
            leftPadding: 20
            rightPadding: 20
            radius: 16
            Layout.alignment: Qt.AlignBottom
            text: qsTr("Back")
            onClicked: currentIndex = 0
        }
    }
}
