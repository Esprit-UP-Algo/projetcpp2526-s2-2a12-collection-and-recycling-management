/********************************************************************************
** Form generated from reading UI file 'mission.ui'
**
** Created by: Qt User Interface Compiler version 6.7.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MISSION_H
#define UI_MISSION_H

#include <QtCore/QDate>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QDateEdit>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableView>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QTimeEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GestionMission
{
public:
    QWidget *centralwidget;
    QHBoxLayout *mainLayout;
    QFrame *contentFrame;
    QVBoxLayout *contentLayout;
    QHBoxLayout *headerLayout;
    QLabel *labelTitle;
    QSpacerItem *headerSpacer;
    QPushButton *btnExportHeader;
    QHBoxLayout *formAndTableLayout;
    QGroupBox *groupBox;
    QVBoxLayout *formLayout;
    QHBoxLayout *layoutId;
    QLabel *labelId;
    QLineEdit *idMission;
    QHBoxLayout *layoutType;
    QLabel *label_2;
    QComboBox *typeMission;
    QHBoxLayout *layoutDate;
    QLabel *label_3;
    QDateEdit *dateMission;
    QHBoxLayout *layoutDuree;
    QLabel *label_4;
    QTimeEdit *dureeMission;
    QLabel *labelDureeHint;
    QHBoxLayout *layoutEquipe;
    QLabel *label_5;
    QComboBox *equipeMission;
    QHBoxLayout *layoutZone;
    QLabel *label_6;
    QComboBox *zoneMission;
    QHBoxLayout *layoutEtat;
    QLabel *label_7;
    QComboBox *etatMission;
    QHBoxLayout *layoutPriorite;
    QLabel *label_8;
    QComboBox *prioriteMission;
    QHBoxLayout *layoutDescription;
    QLabel *label_9;
    QTextEdit *descriptionMission;
    QSpacerItem *formSpacer;
    QHBoxLayout *buttonsFormLayout;
    QPushButton *btnEnregistrer;
    QPushButton *btnModifier;
    QPushButton *btnAnnuler;
    QGroupBox *groupBox_2;
    QVBoxLayout *tableLayout;
    QHBoxLayout *toolbarLayout;
    QLineEdit *rechercheMission;
    QPushButton *btnRechercher;
    QPushButton *btnRefresh;
    QPushButton *btnTrier;
    QSpacerItem *toolbarSpacer;
    QTableView *tableViewMission;
    QHBoxLayout *actionButtonsLayout;
    QPushButton *btnSupprimer;
    QPushButton *btnExportTable;
    QPushButton *btnStatistiques;
    QPushButton *btnRetour;
    QPushButton *btnQuitter;
    QSpacerItem *actionSpacer;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *GestionMission)
    {
        if (GestionMission->objectName().isEmpty())
            GestionMission->setObjectName("GestionMission");
        GestionMission->resize(1243, 657);
        GestionMission->setStyleSheet(QString::fromUtf8("\n"
"    /* Style principal - Fond noir */\n"
"    QMainWindow {\n"
"        background-color: #1E1E1E;\n"
"    }\n"
"\n"
"    QWidget#centralwidget {\n"
"        background-color: #1E1E1E;\n"
"    }\n"
"\n"
"    /* Style de la sidebar */\n"
"    #sidebar {\n"
"        background-color: #2D2D2D;\n"
"        border-right: 1px solid #3D3D3D;\n"
"    }\n"
"\n"
"    #sidebar QPushButton {\n"
"        background: transparent;\n"
"        border: none;\n"
"        padding: 12px;\n"
"        text-align: left;\n"
"        font-size: 14px;\n"
"        color: #FFFFFF;\n"
"        border-radius: 5px;\n"
"        margin: 2px 8px;\n"
"    }\n"
"\n"
"    #sidebar QPushButton:hover {\n"
"        background-color: #9BE15D;\n"
"        color: #1E1E1E;\n"
"    }\n"
"\n"
"    #sidebar QLabel {\n"
"        color: #9BE15D;\n"
"        font-weight: bold;\n"
"        padding-left: 12px;\n"
"        padding-top: 15px;\n"
"    }\n"
"\n"
"    #sidebar QLabel#labelTitleSidebar {\n"
"        color: #FFFFFF;\n"
"        font-size: 16px;\n"
""
                        "        padding: 20px 12px;\n"
"        border-bottom: 1px solid #3D3D3D;\n"
"    }\n"
"\n"
"    /* Style des groupes */\n"
"    QGroupBox {\n"
"        font-weight: bold;\n"
"        border: 1px solid #3D3D3D;\n"
"        border-radius: 10px;\n"
"        margin-top: 10px;\n"
"        padding-top: 10px;\n"
"        background-color: #2D2D2D;\n"
"        color: #FFFFFF;\n"
"    }\n"
"\n"
"    QGroupBox::title {\n"
"        subcontrol-origin: margin;\n"
"        left: 20px;\n"
"        padding: 0 10px 0 10px;\n"
"        color: #9BE15D;\n"
"    }\n"
"\n"
"    /* Style des tableaux */\n"
"    QTableView, QTableWidget {\n"
"        background-color: #2D2D2D;\n"
"        border-radius: 10px;\n"
"        gridline-color: #3D3D3D;\n"
"        selection-background-color: #9BE15D;\n"
"        selection-color: #1E1E1E;\n"
"        alternate-background-color: #353535;\n"
"        color: #FFFFFF;\n"
"    }\n"
"\n"
"    QTableView::item:hover, QTableWidget::item:hover {\n"
"        background-color: rgba(155, 225, 93, 0.3);"
                        "\n"
"    }\n"
"\n"
"    QTableView::item, QTableWidget::item {\n"
"        padding: 5px;\n"
"        border-bottom: 1px solid #3D3D3D;\n"
"    }\n"
"\n"
"    /* Style des en-t\303\252tes de tableau */\n"
"    QHeaderView::section {\n"
"        background-color: #1E1E1E;\n"
"        color: #9BE15D;\n"
"        padding: 10px;\n"
"        font-weight: bold;\n"
"        border: none;\n"
"        border-bottom: 2px solid #9BE15D;\n"
"    }\n"
"\n"
"    /* Style des boutons */\n"
"    QPushButton {\n"
"        background-color: #7ED957;\n"
"        color: #1E1E1E;\n"
"        border-radius: 5px;\n"
"        padding: 10px;\n"
"        font-size: 14px;\n"
"        font-weight: bold;\n"
"        border: none;\n"
"    }\n"
"\n"
"    QPushButton:hover {\n"
"        background-color: #6AC94F;\n"
"    }\n"
"\n"
"    QPushButton.btnDanger {\n"
"        background-color: #FF6B6B;\n"
"        color: white;\n"
"    }\n"
"\n"
"    QPushButton.btnDanger:hover {\n"
"        background-color: #FF5252;\n"
"    }\n"
"\n"
"    QPushB"
                        "utton#btnLogout {\n"
"        background-color: transparent;\n"
"        color: #FF6B6B;\n"
"        border: 1px solid #FF6B6B;\n"
"    }\n"
"\n"
"    QPushButton#btnLogout:hover {\n"
"        background-color: #FF6B6B;\n"
"        color: white;\n"
"    }\n"
"\n"
"    /* Style des labels */\n"
"    QLabel {\n"
"        color: #FFFFFF;\n"
"        font-size: 13px;\n"
"    }\n"
"\n"
"    QLabel[bold=\"true\"] {\n"
"        font-weight: bold;\n"
"        color: #9BE15D;\n"
"    }\n"
"\n"
"    /* Style des champs de saisie */\n"
"    QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox, QDateEdit, QTimeEdit, QTextEdit {\n"
"        border: 1px solid #3D3D3D;\n"
"        border-radius: 5px;\n"
"        padding: 8px;\n"
"        background-color: #353535;\n"
"        color: #FFFFFF;\n"
"        font-size: 13px;\n"
"    }\n"
"\n"
"    QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus, QDateEdit:focus, QTimeEdit:focus, QTextEdit:focus {\n"
"        border: 1px solid #9BE15D;\n"
"        background-color"
                        ": #404040;\n"
"    }\n"
"\n"
"    QLineEdit:hover, QSpinBox:hover, QDoubleSpinBox:hover, QComboBox:hover, QDateEdit:hover, QTimeEdit:hover, QTextEdit:hover {\n"
"        border: 1px solid #9BE15D;\n"
"    }\n"
"\n"
"    /* Style des combobox dropdown */\n"
"    QComboBox QAbstractItemView {\n"
"        background-color: #2D2D2D;\n"
"        color: #FFFFFF;\n"
"        selection-background-color: #9BE15D;\n"
"        selection-color: #1E1E1E;\n"
"        border: 1px solid #3D3D3D;\n"
"    }\n"
"\n"
"    /* Style de la barre de statut */\n"
"    QStatusBar {\n"
"        background-color: #2D2D2D;\n"
"        color: #9BE15D;\n"
"        border-top: 1px solid #3D3D3D;\n"
"    }\n"
"\n"
"    /* Style de la barre de recherche */\n"
"    QLineEdit#rechercheMission {\n"
"        background-color: #353535;\n"
"        color: #FFFFFF;\n"
"        border: 1px solid #3D3D3D;\n"
"    }\n"
"\n"
"    QLineEdit#rechercheMission:focus {\n"
"        border: 1px solid #9BE15D;\n"
"    }\n"
"   "));
        centralwidget = new QWidget(GestionMission);
        centralwidget->setObjectName("centralwidget");
        mainLayout = new QHBoxLayout(centralwidget);
        mainLayout->setSpacing(0);
        mainLayout->setObjectName("mainLayout");
        mainLayout->setContentsMargins(0, 0, 0, 0);
        contentFrame = new QFrame(centralwidget);
        contentFrame->setObjectName("contentFrame");
        contentFrame->setStyleSheet(QString::fromUtf8("\n"
"        QFrame {\n"
"            background-color: #1E1E1E;\n"
"        }\n"
"       "));
        contentLayout = new QVBoxLayout(contentFrame);
        contentLayout->setSpacing(20);
        contentLayout->setObjectName("contentLayout");
        contentLayout->setContentsMargins(30, 30, 30, 30);
        headerLayout = new QHBoxLayout();
        headerLayout->setObjectName("headerLayout");
        labelTitle = new QLabel(contentFrame);
        labelTitle->setObjectName("labelTitle");
        QFont font;
        font.setBold(true);
        labelTitle->setFont(font);
        labelTitle->setStyleSheet(QString::fromUtf8("\n"
"             color: #9BE15D;\n"
"             padding: 10px 0;\n"
"            "));

        headerLayout->addWidget(labelTitle);

        headerSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        headerLayout->addItem(headerSpacer);

        btnExportHeader = new QPushButton(contentFrame);
        btnExportHeader->setObjectName("btnExportHeader");

        headerLayout->addWidget(btnExportHeader);


        contentLayout->addLayout(headerLayout);

        formAndTableLayout = new QHBoxLayout();
        formAndTableLayout->setSpacing(20);
        formAndTableLayout->setObjectName("formAndTableLayout");
        groupBox = new QGroupBox(contentFrame);
        groupBox->setObjectName("groupBox");
        groupBox->setMinimumSize(QSize(400, 0));
        groupBox->setMaximumSize(QSize(400, 16777215));
        formLayout = new QVBoxLayout(groupBox);
        formLayout->setSpacing(15);
        formLayout->setObjectName("formLayout");
        formLayout->setContentsMargins(20, 30, 20, -1);
        layoutId = new QHBoxLayout();
        layoutId->setObjectName("layoutId");
        labelId = new QLabel(groupBox);
        labelId->setObjectName("labelId");
        labelId->setMinimumSize(QSize(120, 30));
        labelId->setFont(font);

        layoutId->addWidget(labelId);

        idMission = new QLineEdit(groupBox);
        idMission->setObjectName("idMission");
        idMission->setMinimumSize(QSize(0, 35));

        layoutId->addWidget(idMission);


        formLayout->addLayout(layoutId);

        layoutType = new QHBoxLayout();
        layoutType->setObjectName("layoutType");
        label_2 = new QLabel(groupBox);
        label_2->setObjectName("label_2");
        label_2->setMinimumSize(QSize(120, 30));
        label_2->setFont(font);

        layoutType->addWidget(label_2);

        typeMission = new QComboBox(groupBox);
        typeMission->addItem(QString());
        typeMission->addItem(QString());
        typeMission->addItem(QString());
        typeMission->addItem(QString());
        typeMission->addItem(QString());
        typeMission->addItem(QString());
        typeMission->setObjectName("typeMission");
        typeMission->setMinimumSize(QSize(0, 35));

        layoutType->addWidget(typeMission);


        formLayout->addLayout(layoutType);

        layoutDate = new QHBoxLayout();
        layoutDate->setObjectName("layoutDate");
        label_3 = new QLabel(groupBox);
        label_3->setObjectName("label_3");
        label_3->setMinimumSize(QSize(120, 30));
        label_3->setFont(font);

        layoutDate->addWidget(label_3);

        dateMission = new QDateEdit(groupBox);
        dateMission->setObjectName("dateMission");
        dateMission->setMinimumSize(QSize(0, 35));
        dateMission->setCalendarPopup(true);
        dateMission->setDate(QDate(2026, 2, 12));

        layoutDate->addWidget(dateMission);


        formLayout->addLayout(layoutDate);

        layoutDuree = new QHBoxLayout();
        layoutDuree->setObjectName("layoutDuree");
        label_4 = new QLabel(groupBox);
        label_4->setObjectName("label_4");
        label_4->setMinimumSize(QSize(120, 30));
        label_4->setFont(font);

        layoutDuree->addWidget(label_4);

        dureeMission = new QTimeEdit(groupBox);
        dureeMission->setObjectName("dureeMission");
        dureeMission->setMinimumSize(QSize(0, 35));
        dureeMission->setTime(QTime(2, 0, 0));

        layoutDuree->addWidget(dureeMission);

        labelDureeHint = new QLabel(groupBox);
        labelDureeHint->setObjectName("labelDureeHint");
        labelDureeHint->setStyleSheet(QString::fromUtf8("color: #9E9E9E; font-size: 11px;"));

        layoutDuree->addWidget(labelDureeHint);


        formLayout->addLayout(layoutDuree);

        layoutEquipe = new QHBoxLayout();
        layoutEquipe->setObjectName("layoutEquipe");
        label_5 = new QLabel(groupBox);
        label_5->setObjectName("label_5");
        label_5->setMinimumSize(QSize(120, 30));
        label_5->setFont(font);

        layoutEquipe->addWidget(label_5);

        equipeMission = new QComboBox(groupBox);
        equipeMission->addItem(QString());
        equipeMission->addItem(QString());
        equipeMission->addItem(QString());
        equipeMission->addItem(QString());
        equipeMission->addItem(QString());
        equipeMission->setObjectName("equipeMission");
        equipeMission->setMinimumSize(QSize(0, 35));

        layoutEquipe->addWidget(equipeMission);


        formLayout->addLayout(layoutEquipe);

        layoutZone = new QHBoxLayout();
        layoutZone->setObjectName("layoutZone");
        label_6 = new QLabel(groupBox);
        label_6->setObjectName("label_6");
        label_6->setMinimumSize(QSize(120, 30));
        label_6->setFont(font);

        layoutZone->addWidget(label_6);

        zoneMission = new QComboBox(groupBox);
        zoneMission->addItem(QString());
        zoneMission->addItem(QString());
        zoneMission->addItem(QString());
        zoneMission->addItem(QString());
        zoneMission->addItem(QString());
        zoneMission->setObjectName("zoneMission");
        zoneMission->setMinimumSize(QSize(0, 35));

        layoutZone->addWidget(zoneMission);


        formLayout->addLayout(layoutZone);

        layoutEtat = new QHBoxLayout();
        layoutEtat->setObjectName("layoutEtat");
        label_7 = new QLabel(groupBox);
        label_7->setObjectName("label_7");
        label_7->setMinimumSize(QSize(120, 30));
        label_7->setFont(font);

        layoutEtat->addWidget(label_7);

        etatMission = new QComboBox(groupBox);
        etatMission->addItem(QString());
        etatMission->addItem(QString());
        etatMission->addItem(QString());
        etatMission->addItem(QString());
        etatMission->addItem(QString());
        etatMission->setObjectName("etatMission");
        etatMission->setMinimumSize(QSize(0, 35));

        layoutEtat->addWidget(etatMission);


        formLayout->addLayout(layoutEtat);

        layoutPriorite = new QHBoxLayout();
        layoutPriorite->setObjectName("layoutPriorite");
        label_8 = new QLabel(groupBox);
        label_8->setObjectName("label_8");
        label_8->setMinimumSize(QSize(120, 30));
        label_8->setFont(font);

        layoutPriorite->addWidget(label_8);

        prioriteMission = new QComboBox(groupBox);
        prioriteMission->addItem(QString());
        prioriteMission->addItem(QString());
        prioriteMission->addItem(QString());
        prioriteMission->addItem(QString());
        prioriteMission->setObjectName("prioriteMission");
        prioriteMission->setMinimumSize(QSize(0, 35));

        layoutPriorite->addWidget(prioriteMission);


        formLayout->addLayout(layoutPriorite);

        layoutDescription = new QHBoxLayout();
        layoutDescription->setObjectName("layoutDescription");
        label_9 = new QLabel(groupBox);
        label_9->setObjectName("label_9");
        label_9->setMinimumSize(QSize(120, 30));
        label_9->setFont(font);

        layoutDescription->addWidget(label_9);

        descriptionMission = new QTextEdit(groupBox);
        descriptionMission->setObjectName("descriptionMission");
        descriptionMission->setMinimumSize(QSize(0, 60));
        descriptionMission->setMaximumSize(QSize(16777215, 60));

        layoutDescription->addWidget(descriptionMission);


        formLayout->addLayout(layoutDescription);

        formSpacer = new QSpacerItem(20, 20, QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

        formLayout->addItem(formSpacer);

        buttonsFormLayout = new QHBoxLayout();
        buttonsFormLayout->setObjectName("buttonsFormLayout");
        btnEnregistrer = new QPushButton(groupBox);
        btnEnregistrer->setObjectName("btnEnregistrer");
        btnEnregistrer->setMinimumSize(QSize(110, 40));

        buttonsFormLayout->addWidget(btnEnregistrer);

        btnModifier = new QPushButton(groupBox);
        btnModifier->setObjectName("btnModifier");
        btnModifier->setMinimumSize(QSize(110, 40));

        buttonsFormLayout->addWidget(btnModifier);

        btnAnnuler = new QPushButton(groupBox);
        btnAnnuler->setObjectName("btnAnnuler");
        btnAnnuler->setMinimumSize(QSize(110, 40));
        btnAnnuler->setStyleSheet(QString::fromUtf8("background-color: #FF6B6B; color: white;"));

        buttonsFormLayout->addWidget(btnAnnuler);


        formLayout->addLayout(buttonsFormLayout);


        formAndTableLayout->addWidget(groupBox);

        groupBox_2 = new QGroupBox(contentFrame);
        groupBox_2->setObjectName("groupBox_2");
        tableLayout = new QVBoxLayout(groupBox_2);
        tableLayout->setSpacing(15);
        tableLayout->setObjectName("tableLayout");
        toolbarLayout = new QHBoxLayout();
        toolbarLayout->setObjectName("toolbarLayout");
        rechercheMission = new QLineEdit(groupBox_2);
        rechercheMission->setObjectName("rechercheMission");
        rechercheMission->setMinimumSize(QSize(200, 35));

        toolbarLayout->addWidget(rechercheMission);

        btnRechercher = new QPushButton(groupBox_2);
        btnRechercher->setObjectName("btnRechercher");
        btnRechercher->setMinimumSize(QSize(100, 35));

        toolbarLayout->addWidget(btnRechercher);

        btnRefresh = new QPushButton(groupBox_2);
        btnRefresh->setObjectName("btnRefresh");
        btnRefresh->setMinimumSize(QSize(100, 35));

        toolbarLayout->addWidget(btnRefresh);

        btnTrier = new QPushButton(groupBox_2);
        btnTrier->setObjectName("btnTrier");
        btnTrier->setMinimumSize(QSize(100, 35));

        toolbarLayout->addWidget(btnTrier);

        toolbarSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        toolbarLayout->addItem(toolbarSpacer);


        tableLayout->addLayout(toolbarLayout);

        tableViewMission = new QTableView(groupBox_2);
        tableViewMission->setObjectName("tableViewMission");
        tableViewMission->setMinimumSize(QSize(0, 300));
        tableViewMission->setAlternatingRowColors(true);
        tableViewMission->setSelectionBehavior(QAbstractItemView::SelectionBehavior::SelectRows);

        tableLayout->addWidget(tableViewMission);

        actionButtonsLayout = new QHBoxLayout();
        actionButtonsLayout->setObjectName("actionButtonsLayout");
        btnSupprimer = new QPushButton(groupBox_2);
        btnSupprimer->setObjectName("btnSupprimer");
        btnSupprimer->setMinimumSize(QSize(100, 35));
        btnSupprimer->setStyleSheet(QString::fromUtf8("background-color: #FF6B6B; color: white;"));

        actionButtonsLayout->addWidget(btnSupprimer);

        btnExportTable = new QPushButton(groupBox_2);
        btnExportTable->setObjectName("btnExportTable");
        btnExportTable->setMinimumSize(QSize(100, 35));

        actionButtonsLayout->addWidget(btnExportTable);

        btnStatistiques = new QPushButton(groupBox_2);
        btnStatistiques->setObjectName("btnStatistiques");
        btnStatistiques->setMinimumSize(QSize(100, 35));

        actionButtonsLayout->addWidget(btnStatistiques);

        btnRetour = new QPushButton(groupBox_2);
        btnRetour->setObjectName("btnRetour");
        btnRetour->setMinimumSize(QSize(100, 35));
        btnRetour->setStyleSheet(QString::fromUtf8("background-color: #6C757D; color: white;"));

        actionButtonsLayout->addWidget(btnRetour);

        btnQuitter = new QPushButton(groupBox_2);
        btnQuitter->setObjectName("btnQuitter");
        btnQuitter->setMinimumSize(QSize(100, 35));
        btnQuitter->setStyleSheet(QString::fromUtf8("background-color: #FF6B6B; color: white;"));

        actionButtonsLayout->addWidget(btnQuitter);

        actionSpacer = new QSpacerItem(40, 20, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        actionButtonsLayout->addItem(actionSpacer);


        tableLayout->addLayout(actionButtonsLayout);


        formAndTableLayout->addWidget(groupBox_2);


        contentLayout->addLayout(formAndTableLayout);


        mainLayout->addWidget(contentFrame);

        GestionMission->setCentralWidget(centralwidget);
        menubar = new QMenuBar(GestionMission);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 1243, 25));
        menubar->setStyleSheet(QString::fromUtf8("\n"
"     background-color: #2D2D2D;\n"
"     color: #FFFFFF;\n"
"    "));
        GestionMission->setMenuBar(menubar);
        statusbar = new QStatusBar(GestionMission);
        statusbar->setObjectName("statusbar");
        statusbar->setStyleSheet(QString::fromUtf8("\n"
"     background-color: #2D2D2D;\n"
"     color: #9BE15D;\n"
"     border-top: 1px solid #3D3D3D;\n"
"     font-weight: bold;\n"
"    "));
        GestionMission->setStatusBar(statusbar);

        retranslateUi(GestionMission);

        QMetaObject::connectSlotsByName(GestionMission);
    } // setupUi

    void retranslateUi(QMainWindow *GestionMission)
    {
        GestionMission->setWindowTitle(QCoreApplication::translate("GestionMission", "Gestion des Missions", nullptr));
        labelTitle->setText(QCoreApplication::translate("GestionMission", "\360\237\216\257 Gestion des Missions", nullptr));
        btnExportHeader->setText(QCoreApplication::translate("GestionMission", "\360\237\223\212 Exporter", nullptr));
        groupBox->setTitle(QCoreApplication::translate("GestionMission", "\360\237\223\235 Formulaire Mission", nullptr));
        labelId->setText(QCoreApplication::translate("GestionMission", "\360\237\206\224 ID Mission :", nullptr));
        idMission->setPlaceholderText(QCoreApplication::translate("GestionMission", "MSN-001", nullptr));
        label_2->setText(QCoreApplication::translate("GestionMission", "\360\237\223\213 Type :", nullptr));
        typeMission->setItemText(0, QCoreApplication::translate("GestionMission", "Collecte", nullptr));
        typeMission->setItemText(1, QCoreApplication::translate("GestionMission", "Maintenance", nullptr));
        typeMission->setItemText(2, QCoreApplication::translate("GestionMission", "Inspection", nullptr));
        typeMission->setItemText(3, QCoreApplication::translate("GestionMission", "Installation", nullptr));
        typeMission->setItemText(4, QCoreApplication::translate("GestionMission", "R\303\251paration", nullptr));
        typeMission->setItemText(5, QCoreApplication::translate("GestionMission", "Sensibilisation", nullptr));

        label_3->setText(QCoreApplication::translate("GestionMission", "\360\237\223\205 Date :", nullptr));
        label_4->setText(QCoreApplication::translate("GestionMission", "\342\217\261\357\270\217 Dur\303\251e :", nullptr));
        dureeMission->setDisplayFormat(QCoreApplication::translate("GestionMission", "HH:mm", nullptr));
        labelDureeHint->setText(QCoreApplication::translate("GestionMission", "(HH:MM)", nullptr));
        label_5->setText(QCoreApplication::translate("GestionMission", "\360\237\221\245 \303\211quipe :", nullptr));
        equipeMission->setItemText(0, QCoreApplication::translate("GestionMission", "\303\211quipe Alpha", nullptr));
        equipeMission->setItemText(1, QCoreApplication::translate("GestionMission", "\303\211quipe Beta", nullptr));
        equipeMission->setItemText(2, QCoreApplication::translate("GestionMission", "\303\211quipe Gamma", nullptr));
        equipeMission->setItemText(3, QCoreApplication::translate("GestionMission", "\303\211quipe Delta", nullptr));
        equipeMission->setItemText(4, QCoreApplication::translate("GestionMission", "Non assign\303\251e", nullptr));

        label_6->setText(QCoreApplication::translate("GestionMission", "\360\237\227\272\357\270\217 Zone :", nullptr));
        zoneMission->setItemText(0, QCoreApplication::translate("GestionMission", "Zone Nord", nullptr));
        zoneMission->setItemText(1, QCoreApplication::translate("GestionMission", "Zone Sud", nullptr));
        zoneMission->setItemText(2, QCoreApplication::translate("GestionMission", "Zone Est", nullptr));
        zoneMission->setItemText(3, QCoreApplication::translate("GestionMission", "Zone Ouest", nullptr));
        zoneMission->setItemText(4, QCoreApplication::translate("GestionMission", "Centre", nullptr));

        label_7->setText(QCoreApplication::translate("GestionMission", "\342\232\231\357\270\217 \303\211tat :", nullptr));
        etatMission->setItemText(0, QCoreApplication::translate("GestionMission", "Planifi\303\251e", nullptr));
        etatMission->setItemText(1, QCoreApplication::translate("GestionMission", "En cours", nullptr));
        etatMission->setItemText(2, QCoreApplication::translate("GestionMission", "Termin\303\251e", nullptr));
        etatMission->setItemText(3, QCoreApplication::translate("GestionMission", "Annul\303\251e", nullptr));
        etatMission->setItemText(4, QCoreApplication::translate("GestionMission", "En attente", nullptr));

        label_8->setText(QCoreApplication::translate("GestionMission", "\342\232\240\357\270\217 Priorit\303\251 :", nullptr));
        prioriteMission->setItemText(0, QCoreApplication::translate("GestionMission", "Critique", nullptr));
        prioriteMission->setItemText(1, QCoreApplication::translate("GestionMission", "Haute", nullptr));
        prioriteMission->setItemText(2, QCoreApplication::translate("GestionMission", "Moyenne", nullptr));
        prioriteMission->setItemText(3, QCoreApplication::translate("GestionMission", "Basse", nullptr));

        label_9->setText(QCoreApplication::translate("GestionMission", "\360\237\223\204 Description :", nullptr));
        descriptionMission->setPlaceholderText(QCoreApplication::translate("GestionMission", "Description de la mission...", nullptr));
        btnEnregistrer->setText(QCoreApplication::translate("GestionMission", "\342\236\225 Enregistrer", nullptr));
        btnModifier->setText(QCoreApplication::translate("GestionMission", "\342\234\217\357\270\217 Modifier", nullptr));
        btnAnnuler->setText(QCoreApplication::translate("GestionMission", "\360\237\227\221\357\270\217 Annuler", nullptr));
        groupBox_2->setTitle(QCoreApplication::translate("GestionMission", "\360\237\223\212 Liste des Missions", nullptr));
        rechercheMission->setPlaceholderText(QCoreApplication::translate("GestionMission", "\360\237\224\215 Rechercher une mission...", nullptr));
        btnRechercher->setText(QCoreApplication::translate("GestionMission", "Rechercher", nullptr));
        btnRefresh->setText(QCoreApplication::translate("GestionMission", "\360\237\224\204 Refresh", nullptr));
        btnTrier->setText(QCoreApplication::translate("GestionMission", "\360\237\223\212 Trier", nullptr));
        btnSupprimer->setText(QCoreApplication::translate("GestionMission", "\360\237\227\221\357\270\217 Supprimer", nullptr));
        btnExportTable->setText(QCoreApplication::translate("GestionMission", "\360\237\223\212 Export", nullptr));
        btnStatistiques->setText(QCoreApplication::translate("GestionMission", "\360\237\223\210 Stats", nullptr));
        btnRetour->setText(QCoreApplication::translate("GestionMission", "\342\227\200 Retour", nullptr));
        btnQuitter->setText(QCoreApplication::translate("GestionMission", "\360\237\232\252 Quitter", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GestionMission: public Ui_GestionMission {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MISSION_H
