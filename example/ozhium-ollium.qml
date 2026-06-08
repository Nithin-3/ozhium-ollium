import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Window 2.15

Window {
    id: osd
    visible: true
    x: 20
    y: (Screen.height - height) /2
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
    color: "transparent"

    width: visibleSlider ? sliderCol.childrenRect.width + 24 : textRow.childrenRect.width + 24
    height: visibleSlider ? sliderCol.childrenRect.height + 16 : textRow.childrenRect.height + 16

    readonly property bool visibleSlider: argsElement === "slider"

    readonly property var actionIcons: ({
        0: "\uEA61",
        1: "\uF028",
        2: "\uDB81\uDD81",
        3: "\uDB80\uDF6C",
        4: "\uF131",
        5: "\uDB80\uDC83",
        6: "\uDB84\uDEA1",
        7: "\uF240",
        8: "\uDB80\uDC84",
        9: "\uDB80\uDC7E",
        10: "\uDB80\uDC79",
        11: "\uF1EB",
        12: "\uDB80\uDE00",
        13: "\uF293",
        14: "\uDB84\uDDF0",
        15: "\uDB85\uDD81",
        16: "\uDB85\uDD4A"
    })

    function iconFor(action) {
        return actionIcons[parseInt(action)] || "?";
    }

    Rectangle {
        anchors.fill: parent
        color: "#dd222222"
        radius: 8
        border.color: "#666666"
        border.width: 1

        Row {
            id: textRow
            anchors.centerIn: parent
            visible: !visibleSlider
            spacing: 8

            Text {
                text: iconFor(argsAction)
                color: "#f0f0f0"
                font.pixelSize: 22
                font.family: "monospace"
                verticalAlignment: Text.AlignVCenter
            }

            Text {
                text: argsText
                color: "#f0f0f0"
                font.pixelSize: 20
                font.family: "monospace"
                verticalAlignment: Text.AlignVCenter
            }
        }

        Column {
            id: sliderCol
            anchors.centerIn: parent
            visible: visibleSlider
            spacing: 10

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: iconFor(argsAction)
                color: "#f0f0f0"
                font.pixelSize: 28
                font.family: "monospace"
            }

            Slider {
                id: slider
                anchors.horizontalCenter: parent.horizontalCenter
                orientation: Qt.Vertical
                from: argsMin
                to: argsMax
                value: argsCurrent
                enabled: false
                implicitWidth: 24
                implicitHeight: 240

                background: Rectangle {
                    x: parent.leftPadding + parent.availableWidth / 2 - width / 2
                    y: parent.topPadding
                    width: 6
                    height: parent.availableHeight
                    radius: 3
                    color: "#444"
                }

                handle: Rectangle {
                    x: parent.leftPadding + parent.availableWidth / 2 - width / 2
                    y: parent.topPadding + parent.visualPosition * (parent.availableHeight - height)
                    width: 16
                    height: 16
                    radius: 8
                    color: "#08bdba"
                }
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                text: {
                    var range = argsMax - argsMin;
                    if (range <= 0) return "0%";
                    return Math.round((argsCurrent - argsMin) / range * 100) + "%";
                }
                color: "#f0f0f0"
                font.pixelSize: 18
                font.family: "monospace"
            }
        }
    }
}
