import QtQuick 2.15
import QtQuick.Controls 2.15
import QtCharts 2.3
import QtQuick.Layouts 1.11

Page {
    id: page
    width: 600
    height: 400
    property alias line1: line1
    property alias scatter: scatter
    property alias line: line

    ColumnLayout {
        id: columnLayout
        anchors.fill: parent
        spacing: 0

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
            }
            ValueAxis {
                id: line1_xAxis
                min: 0
                max: 44100 * 4
                tickCount: 5
            }
            ValueAxis {
                id: line1_yAxis
                min: -1
                max: +1
            }
        }

        ChartView {
            id: line
            height: 300
            Layout.fillHeight: true
            Layout.fillWidth: true
            property alias line_series: line_series

            LineSeries {
                id: line_series
                name: "LineSeries"
                axisX: line_xAxis
                axisY: line_yAxis
                useOpenGL: true
            }
            ValueAxis {
                id: line_xAxis
                min: 0
                max: 10
                tickCount: 20
            }
            LogValueAxis {
                id: line_yAxis
                min: .1
                max: 10
            }
        }

        ChartView {
            id: scatter
            width: 300
            height: 300
            Layout.fillWidth: true
            Layout.fillHeight: true
            property alias scatter_series: scatter_series
            ScatterSeries {
                id: scatter_series
                name: "ScatterSeries"
                useOpenGL: true
                markerShape: ScatterSeries.MarkerShapeCircle
            }
            ValueAxis {
                id: scatter_xAxis
                min: 0
                max: 10
                tickCount: 5
            }
            ValueAxis {
                id: scatter_yAxis
                min: 0
                max: 10
            }
        }
    }
}
