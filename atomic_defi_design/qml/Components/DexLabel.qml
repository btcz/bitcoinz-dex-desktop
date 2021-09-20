import QtQuick 2.15
import "../Constants" as Constants
import App 1.0

Text {
    property string text_value
    property bool privacy: false

    Behavior on color {
        ColorAnimation {
            duration: Style.animationDuration
        }
    }

    font: Qt.font({
        pixelSize: 13,
        letterSpacing: 0.25,
        weight: Font.Normal
    })

    color: try {
        DexTheme.foregroundColor ?? "white"
    } catch (e) {
        "white"
    }

    text: privacy && Constants.General.privacy_mode ? Constants.General.privacy_text : text_value
    wrapMode: Text.WordWrap

    onLinkActivated: Qt.openUrlExternally(link)
    linkColor: color

    DefaultMouseArea {
        anchors.fill: parent
        cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
        acceptedButtons: Qt.NoButton
    }
}