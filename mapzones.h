#ifndef MAPZONES_H
#define MAPZONES_H

#include <QObject>

class MapZones : public QObject
{
    Q_OBJECT
public:
    explicit MapZones(QObject *parent = nullptr);
    void openMap();
};

#endif // MAPZONES_H
