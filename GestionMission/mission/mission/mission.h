#ifndef MISSION_H
#define MISSION_H

#include <QMainWindow>

namespace Ui {
class GestionMission;
}

class GestionMission : public QMainWindow
{
    Q_OBJECT

public:
    explicit GestionMission(QWidget *parent = nullptr);
    ~GestionMission();

private slots:
    void on_btnRetour_clicked();
    void on_btnQuitter_clicked();

private:
    Ui::GestionMission *ui;
};

#endif // MISSION_H
