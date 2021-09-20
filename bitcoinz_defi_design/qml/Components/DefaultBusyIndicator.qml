import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Universal 2.15
import "../Constants"
import App 1.0

BusyIndicator {
    id: control

    Universal.theme: Style.dark_theme ? Universal.Dark : Universal.Light
    Universal.accent: DexTheme.greenColor
    Universal.foreground: Style.colorQtThemeForeground
    Universal.background: Style.colorQtThemeBackground

    implicitWidth: 48
    implicitHeight: 48
}
