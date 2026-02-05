#ifndef GESTIONEQUIPE_H
#define GESTIONEQUIPE_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class GestionEquipe;
}
QT_END_NAMESPACE

class GestionEquipe : public QMainWindow
{
    Q_OBJECT

public:
    GestionEquipe(QWidget *parent = nullptr);
    ~GestionEquipe();

private slots:
    void on_retourne_2_clicked();

    void on_quitEQ_clicked();

    void on_chatEQ_clicked();

private:
    Ui::GestionEquipe *ui;
};
#endif // GESTIONEQUIPE_H
