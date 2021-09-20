import QtQuick 2.15
import QtQuick.Controls 2.15
import Qaterial 1.0 as Qaterial
import QtQuick.Controls.Universal 2.15
import QtQuick.Layouts 1.12

import App 1.0

Item {
    anchors.fill: parent
    Item {
        width: parent.width
        y: 1
        height: 30
        Rectangle {
            width: parent.width - 2
            height: 30
            border.color: 'transparent'
            border.width: 2
            anchors.horizontalCenter: parent.horizontalCenter
            gradient: Gradient {
                orientation: Qt.Horizontal
                GradientStop {
                    position: 0.0;color: "transparent"
                }
                GradientStop {
                    position: 0.4;color: "transparent"
                }
                GradientStop {
                    position: 0.6;color: DexTheme.surfaceColor
                }
                GradientStop {
                    position: 1.0;color: Qt.darker(DexTheme.dexBoxBackgroundColor, 0.9)
                }
            }
        }
        MouseArea {
            onPressed: window.startSystemMove();
            anchors.fill: parent
            anchors.rightMargin: 280
            onDoubleClicked: {
                if (window.visibility === ApplicationWindow.Maximized) {
                    window.showNormal()
                } else {
                    window.showMaximized()
                }
            }
        }
        DexMacosHeaderControl {
            anchors.verticalCenter: parent.verticalCenter
        }
    }
    Item {
        id: _left_resize
        height: parent.height
        width: 3
        MouseArea {
            onPressed: window.startSystemResize(Qt.LeftEdge)
            anchors.fill: parent
            cursorShape: "SizeHorCursor"
        }
    }
    Item {
        id: _right_resize
        height: parent.height
        anchors.right: parent.right
        width: 3
        MouseArea {
            onPressed: {
                window.startSystemResize(Qt.RightEdge)
            }
            cursorShape: "SizeHorCursor"
        }
    }
    Item {
        id: _bottom_resize
        height: 3
        width: parent.width
        anchors.bottom: parent.bottom
        MouseArea {
            onPressed: if (active) window.startSystemResize(Qt.BottomEdge)
            //target: null
            anchors.fill: parent
            cursorShape: "SizeVerCursor"
        }
    }
    Item {
        id: _top_resize
        height: 3
        width: parent.width
        MouseArea {
            onPressed: window.startSystemResize(Qt.TopEdge)
            //target: null
            anchors.fill: parent
            cursorShape: "SizeVerCursor"
        }
    }
    Item {
        id: _bottom_right_resize
        height: 6
        width: 6
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        MouseArea {
            onPressed: if (active) window.startSystemResize(Qt.BottomEdge | Qt.RightEdge)
            anchors.fill: parent
            cursorShape: "SizeFDiagCursor"
        }
    }
}