import QtQuick 2.15
import QtQuick.Controls 2.15
import QtLocation 5.15
import QtPositioning 5.15

Item {
    id: root
    width: parent.width
    height: parent.height

    // Signal pour communiquer avec C++
    signal markerClicked(string info)

    Plugin {
        id: mapPlugin
        name: "osm"
        PluginParameter { name: "osm.mapping.custom.host"; value: "https://tile.openstreetmap.org/" }
    }

    Map {
        id: map
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(36.8190, 10.1658) // Tunis
        zoomLevel: 12
        copyrightsVisible: false

        // Permet le zoom avec la molette
        WheelArea {
            anchors.fill: parent
            onWheel: {
                if (wheel.angleDelta.y > 0)
                    map.zoomLevel = Math.min(map.zoomLevel + 0.5, 19)
                else
                    map.zoomLevel = Math.max(map.zoomLevel - 0.5, 1)
            }
        }

        // Drag pour déplacer la carte
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton
            property var lastPos

            onPressed: lastPos = Qt.point(mouse.x, mouse.y)

            onPositionChanged: {
                if (pressed) {
                    var dx = mouse.x - lastPos.x
                    var dy = mouse.y - lastPos.y
                    map.pan(-dx, -dy)
                    lastPos = Qt.point(mouse.x, mouse.y)
                }
            }
        }

        // Marqueurs dynamiques
        MapItemView {
            id: markersView
            model: markerModel

            delegate: MapQuickItem {
                coordinate: QtPositioning.coordinate(model.lat, model.lng)
                anchorPoint.x: markerImage.width / 2
                anchorPoint.y: markerImage.height

                sourceItem: Column {
                    spacing: 2

                    Rectangle {
                        width: labelText.width + 12
                        height: labelText.height + 6
                        color: model.color
                        radius: 6
                        visible: labelText.text !== ""

                        Text {
                            id: labelText
                            anchors.centerIn: parent
                            text: model.label
                            color: "white"
                            font.pixelSize: 11
                            font.bold: true
                        }
                    }

                    Image {
                        id: markerImage
                        width: 32
                        height: 32
                        source: "qrc:/icons/marker.png"
                        anchors.horizontalCenter: parent.horizontalCenter

                        MouseArea {
                            anchors.fill: parent
                            onClicked: root.markerClicked(model.info)
                        }
                    }
                }
            }
        }
    }

    // Boutons zoom
    Column {
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.margins: 12
        spacing: 4

        Button {
            width: 36; height: 36
            text: "+"
            font.bold: true
            onClicked: map.zoomLevel = Math.min(map.zoomLevel + 1, 19)
        }
        Button {
            width: 36; height: 36
            text: "−"
            font.bold: true
            onClicked: map.zoomLevel = Math.max(map.zoomLevel - 1, 1)
        }
    }

    // Fonctions appelables depuis C++
    function addMarker(lat, lng, label, info, color) {
        markerModel.append({
            "lat": lat,
            "lng": lng,
            "label": label,
            "info": info,
            "color": color
        })
    }

    function clearMarkers() {
        markerModel.clear()
    }

    function centerMap(lat, lng, zoom) {
        map.center = QtPositioning.coordinate(lat, lng)
        if (zoom > 0) map.zoomLevel = zoom
    }

    ListModel { id: markerModel }
}
