import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.11
import QtCharts 2.3

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
            id: line
            width: 300
            height: 300
            Layout.fillHeight: true
            Layout.fillWidth: true

            LineSeries {
                name: "LineSeries"
            }
        }

        RowLayout {
            id: rowLayout
            width: 100
            height: 100
            spacing: 0

            ChartView {
                id: scatter
                width: 300
                height: 300
                Layout.fillWidth: true
                Layout.fillHeight: true

                ScatterSeries {
                    name: "ScatterSeries"
                }
            }

            ChartView {
                id: line1
                width: 300
                height: 300
                Layout.maximumWidth: 10535
                Layout.fillWidth: true
                Layout.fillHeight: true
                LineSeries {
                    name: "LineSeries"
                }
            }
        }
    }
}
