import QtQuick 2.15
import QtQuick.Controls 2.15
import QtCharts 2.3
import QtQuick.Layouts 1.15
//import QtDataVisualization 1.15

import PadSynthGraph 1.0

ApplicationWindow {
    id: appWindow
    width: 2000
    height: 700
    visible: true
    title: qsTr("Tabs")
    PadSynthGraph {
        id : pad_synth_graph
    }
    SwipeView {
        id: swipeView
        anchors.fill: parent
        currentIndex: tabBar.currentIndex
        interactive: true
        Item {
            id: page1
            ColumnLayout{
                id: mainLayout
                anchors.fill: parent
                spacing: 0
                ChartView {
                    id: chart_harmonics_profile
                    //            title: "harmonics profile"
                    //            anchors.fill: parent
                    //            Layout.alignment: Qt.AlignTop
                    //            Layout.preferredHeight: 40
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    antialiasing: true
                    theme: ChartView.ChartThemeDark
                    LineSeries {
                        name: "harmonics profile"

                        id: series_harmonics_profile
                        axisX: axisX_harmonics_profile
                        axisY: axisY_harmonics_profile
                        useOpenGL: true
                        Component.onCompleted: {
                            pad_synth_graph.spectrum_update_series(this, 0);
                            axisY_harmonics_profile.max = pad_synth_graph.spectrum_max_y_value(0) * 1.1;
                            var axisX_harmonics_profile_min_max = pad_synth_graph.harmonics_profile_min_max(1);
                            axisX_harmonics_profile.min = axisX_harmonics_profile_min_max.x;
                            axisX_harmonics_profile.max = axisX_harmonics_profile_min_max.y;
                        }
                    }
                    ValueAxis {
                        id: axisX_harmonics_profile
                        min: 0
                        max: 1
                        tickCount: 5
                    }

                    ValueAxis {
                        id: axisY_harmonics_profile
                        min: 0
                        max: 1
                    }
                }
                ChartView {
                    id: chart_spectrum
                    //            title: "spectrum"
                    //            anchors.fill: parent
                    //            Layout.alignment: Qt.AlignTop

                    //            Layout.preferredHeight: appWindow.height/2
                    //            Layout.minimumHeight: 30
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    antialiasing: true
                    theme: ChartView.ChartThemeDark
                    LineSeries {
                        name: "spectrum"
                        id: series1
                        axisX: axisX_spectrum
                        axisY: axisY_spectrum
                        useOpenGL: true
                        Component.onCompleted: {
                            pad_synth_graph.spectrum_update_series(this, 0);
                            axisY_spectrum.max = pad_synth_graph.spectrum_max_y_value(0) * 1.1;
//                            axisY_spectrum.max = 10000.;
//                            axisY_spectrum.min = .1;
                            axisX_spectrum.max = pad_synth_graph.spectrum_max_x_value();
                        }
                    }
                    ValueAxis {
                        id: axisX_spectrum
                        min: 0
                        max: 1000
                        tickCount: 20
                    }

                    LogValueAxis {
                        id: axisY_spectrum
                        min: .1
                    }

                    //            PinchArea {
                    //                id: pa
                    //                anchors.fill: parent
                    //                onPinchUpdated: {
                    //                    chart.zoomReset();
                    //                    var center_x = pinch.center.x
                    //                    var center_y = pinch.center.y
                    //                    var width_zoom = window.height/pinch.scale;
                    //                    var height_zoom = window.width/pinch.scale;
                    //                    var r = Qt.rect(center_x-width_zoom/2, center_y - height_zoom/2, width_zoom, height_zoom)
                    //                    chart.zoomIn(r);
                    //                    console.log("hello");
                    //                }
                    //            }
                }
                ChartView {
                    id: chart_harmonics_amplitude
                    //            title: "harmonics amplitude"
                    //            anchors.fill: parent
                    //            Layout.alignment: Qt.AlignTop
                    //            Layout.preferredHeight: 40
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    antialiasing: true
                    theme: ChartView.ChartThemeDark
                    ScatterSeries {
                        name: "harmonics amplitude"
                        id: series_harmonics_amplitude
                        axisX: axisX_harmonics_amplitude
                        axisY: axisY_harmonics_amplitude
                        useOpenGL: true
                        Component.onCompleted: {
                            pad_synth_graph.harmonics_amplitude_update_series(this, 0);
                            axisY_harmonics_amplitude.max = pad_synth_graph.harmonics_amplitude_max_value(0) * 1.1;
                            axisX_harmonics_amplitude.max = pad_synth_graph.harmonics_amplitude_length(0);
                        }
                    }
                    ValueAxis {
                        id: axisX_harmonics_amplitude
                        min: 0
                        max: 1
                        tickCount: 5
                    }

                    ValueAxis {
                        id: axisY_harmonics_amplitude
                        min: 0
                        max: 1
                    }
                }
            }
        }
        Page1Form {
            scatter.theme: ChartView.ChartThemeDark
            line.theme: ChartView.ChartThemeDark
            line1.theme: ChartView.ChartThemeDark
            Component.onCompleted: {
                pad_synth_graph.harmonics_amplitude_update_series(scatter.series(0), 1);
                scatter.axisX().max = pad_synth_graph.harmonics_amplitude_length(1);
                scatter.axisY().max = pad_synth_graph.harmonics_amplitude_max_value(1) * 1.1;
                pad_synth_graph.spectrum_update_series(line.series(0), 1);
                line.axisX().max = pad_synth_graph.spectrum_max_x_value();
                line.axisY().max = pad_synth_graph.spectrum_max_y_value(1) * 1.1;
//                pad_synth_graph.sound_samples_update_series(line1.series(0));
//                line1.axisX().max = pad_synth_graph.sound_samples_size();
            }
        }
        Page2Form {
            scatter.theme: ChartView.ChartThemeDark
            
            line.theme: ChartView.ChartThemeDark
            line1.theme: ChartView.ChartThemeDark
            Component.onCompleted: {
                pad_synth_graph.harmonics_amplitude_update_series(scatter.series(0), 1);
                scatter.axisX().max = pad_synth_graph.harmonics_amplitude_length(1);
                scatter.axisY().max = pad_synth_graph.harmonics_amplitude_max_value(1) * 1.1;
                pad_synth_graph.spectrum_update_series(line.series(0), 1);
                line.axisX().max = pad_synth_graph.spectrum_max_x_value();
                line.axisY().max = pad_synth_graph.spectrum_max_y_value(1) * 1.1;
            }
        }
        Loader {
//            anchors.fill: parent
            active: SwipeView.isCurrentItem || SwipeView.isPreviousItem// || SwipeView.isNextItem

            sourceComponent:
                Page3Form {
                line.theme: ChartView.ChartThemeDark
                line1.theme: ChartView.ChartThemeDark
                line1_xAxis.min: rangeSlider.first.value
                line1_xAxis.max: rangeSlider.second.value
                Component.onCompleted: {
                    pad_synth_graph.sound_samples_update_series(line.series(0));
                    pad_synth_graph.sound_samples_update_series(this.line1.series(0));
                    line.axisX().max = pad_synth_graph.sound_samples_size();
                    line.axisY().max = 1;
                    line.axisY().min = -1;
                    //                line1_xAxis.max = pad_synth_graph.sound_samples_size(); // problem
                    line1.axisY().max = 1;
                    line1.axisY().min = -1;
                    //                console.log(rangeSlider.to);
                    rangeSlider.to = pad_synth_graph.sound_samples_size();
                    rangeSlider.second.value = pad_synth_graph.sound_samples_size();
                    //                console.log(pad_synth_graph.sound_samples_size());
                    //                console.log(rangeSlider.to);
                    //                rangeSlider.to = 44100.
                }
            }
        }
//        Item {
//            id: page2
//            Bars3D {
//                width: parent.width
//                height: parent.height
//                Layout.fillHeight: true
//                Layout.fillWidth: true

//                Bar3DSeries {
//                    itemLabelFormat: "@colLabel, @rowLabel: @valueLabel"

//                    ItemModelBarDataProxy {
//                        itemModel: dataModel
//                        // Mapping model roles to bar series rows, columns, and values.
//                        rowRole: "year"
//                        columnRole: "city"
//                        valueRole: "expenses"
//                    }
//                }
//            }
//            ListModel {
//                id: dataModel
//                ListElement{ year: "2012"; city: "Oulu";     expenses: "4200"; }
//                ListElement{ year: "2012"; city: "Rauma";    expenses: "2100"; }
//                ListElement{ year: "2012"; city: "Helsinki"; expenses: "7040"; }
//                ListElement{ year: "2012"; city: "Tampere";  expenses: "4330"; }
//                ListElement{ year: "2013"; city: "Oulu";     expenses: "3960"; }
//                ListElement{ year: "2013"; city: "Rauma";    expenses: "1990"; }
//                ListElement{ year: "2013"; city: "Helsinki"; expenses: "7230"; }
//                ListElement{ year: "2013"; city: "Tampere";  expenses: "4650"; }
//            }
//        }
    }

    footer: TabBar {
        id: tabBar
        currentIndex: swipeView.currentIndex

        TabButton {
            text: qsTr("Page 1")
        }
        TabButton {
            text: qsTr("Page 2")
        }
        TabButton {
            text: qsTr("Page 3")
        }
        TabButton {
            text: qsTr("Page 4")
        }
    }
}
