import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import QtCharts 2.3

Page {
    width: 600
    height: 400
    property alias line1: line1
    property alias rangeSlider: rangeSlider
    property alias line: line

    property alias line1_xAxis: line1_xAxis

    ColumnLayout {
        id: columnLayout
        x: 241
        y: 166
        anchors.fill: parent

        ChartView {
            id: line
            width: 300
            height: 300
            Layout.fillHeight: true
            Layout.fillWidth: true
            LineSeries {
                name: "LineSeries"
                useOpenGL: true
            }
        }

        RangeSlider {
            id: rangeSlider
            Layout.rightMargin: 10
            Layout.leftMargin: 10
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            first.value: 0
            second.value: 10
            from: 0
            //            to: 44100.
            stepSize: 1.
        }

        ChartView {
            id: line1
            width: 300
            height: 300
            Layout.fillHeight: true
            Layout.fillWidth: true
            LineSeries {
                name: "LineSeries"
                axisX: line1_xAxis
                axisY: line1_yAxis
                useOpenGL: true
            }
            ValueAxis {
                id: line1_xAxis
                //                tickCount: 5
                min: 0
                //                max: 44100 * 4
            }
            ValueAxis {
                id: line1_yAxis
                min: -1
                max: +1
            }
        }
    }
}
