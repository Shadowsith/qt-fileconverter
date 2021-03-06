#include <convertgui.h>
#include <ui_convertgui.h>
#include <ui_aboutwindow.h>
#include <converter.h>
#include <qdesktopservices.h>
#include <qstring.h>
#include <qfiledialog.h>
#include <qtabwidget.h>
#include <qmessagebox.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qdir.h>
#include <qurl.h>
#include <qpushbutton.h>
#include <qmetaobject.h>
#include <qobject.h>
#include <string>
#include <qmenu.h>
#include <qaction.h>
#include <QApplication>

ConvertGui::ConvertGui(QWidget *parent) : QMainWindow(parent), ui(new Ui::ConvertGui)
{
    ui->setupUi(this);

    connect(ui->btnCloseGui, &QPushButton::clicked, [=] {
        close();
        delete ui;
    });
    connect(ui->btnConvert, &QPushButton::clicked, [=] {
        convertFiles();
    });
    connect(ui->btnClearInput, &QPushButton::clicked, [=] {
         clearLineEdits();
    });
    connect(ui->btnImgFrom, &QPushButton::clicked, [=] {
        useFileExplorer(ui->txtImgFrom, FileType::IMAGE, Target::SOURCE);
    });
    connect(ui->btnImgTo, &QPushButton::clicked, [=] {
        useFileExplorer(ui->txtImgTo, FileType::IMAGE, Target::DEST);
    });
    connect(ui->btnAudFrom, &QPushButton::clicked, [=] {
        useFileExplorer(ui->txtAudFrom, FileType::AUIDO, Target::SOURCE);
    });
    connect(ui->btnAudTo, &QPushButton::clicked, [=] {
        useFileExplorer(ui->txtAudTo, FileType::AUIDO, Target::DEST);
    });
    connect(ui->menuAbout, &QMenu::aboutToShow, [=] {
        openAboutWindow();
    });
}

ConvertGui::~ConvertGui()
{
    delete ui;
}

bool ConvertGui::checkPathes(QString source, QString dest) {
    if (source != dest)
        return true;
    else
        return false;
}

void ConvertGui::convertFiles(){
    QMessageBox msg;
    std::string debugCmd = "";
    std::string convMsg = "";
    std::string inOutEqual = "In- and output files can not have the same path!";
    std::string inEmpty = "Input filepath could not be empty!";

    switch(ui->tabWidget->currentIndex()) {
        case 0: {
            Converter conv(ui->cmbImgFrom->currentText().toStdString(),
                             ui->cmbImgTo->currentText().toStdString());

            debugCmd = conv.getFileType(ui->txtImgFrom->text().toStdString());

            if (ui->txtImgFrom->text() == "") {
                convMsg = inEmpty;
            }
            else if(checkPathes(ui->txtImgFrom->text(), ui->txtImgTo->text())) {
            convMsg = conv.convertImg(ui->txtImgFrom->text().toStdString(),
                                      ui->txtImgTo->text().toStdString());
            } else {
                convMsg = inOutEqual;
            }
        } break;
        case 1: {
            Converter conv(ui->cmbAudFrom->currentText().toStdString(),
                           ui->cmbImgTo->currentText().toStdString());

            debugCmd = conv.getFileType(ui->txtAudFrom->text().toStdString());

            if(ui->txtAudFrom->text() == "") {
                convMsg = inEmpty;
            }
            else if(checkPathes(ui->txtAudFrom->text(), ui->txtAudTo->text())) {
            convMsg = conv.convertAudio(ui->txtAudFrom->text().toStdString(),
                                        ui->txtAudTo->text().toStdString());
            } else {
                convMsg = inOutEqual;
            }

        } break;
        case 2: {} break;
        case 3: {} break;
        case 4: {} break;
        case 5: {} break;
        default: break;
    }
        msg.setText(QString::fromStdString(convMsg) + " " + QString::fromStdString(debugCmd));
        msg.exec();
}

void ConvertGui::useFileExplorer(QLineEdit* le, FileType search, Target target){
    QString path;
    QString type;
    QString homePath = QDir::homePath();
    switch(search) {
        case FileType::IMAGE: {
            if (target == Target::SOURCE) {
                type = ui->cmbImgFrom->currentText();
                if(type != "All")
                    path = QFileDialog::getOpenFileName(this, tr("Open Image"), homePath,
                                            type + " Files ( *." + type.toLower() + " )");
                else
                    path = QFileDialog::getOpenFileName(this, tr("Open Image"), homePath,
                                            m_imgFiles);
            } else {
                type = ui->cmbImgTo->currentText();
                if(type != "All")
                    path = QFileDialog::getOpenFileName(this, tr("Open Image"), homePath,
                                            type + " Files ( *." + type.toLower() + " )");
                else
                    path = QFileDialog::getOpenFileName(this, tr("Open Image"), homePath,
                                            m_imgFiles);
            }
        } break;
        case FileType::AUIDO: {
            if (target == Target::SOURCE) {
                type = ui->cmbAudFrom->currentText();
                path = QFileDialog::getOpenFileName(this, tr("Open Image"), homePath,
                                            type + " Files ( *." + type.toLower() + " )");
            } else {
                type = ui->cmbAudTo->currentText();
                path = QFileDialog::getOpenFileName(this, tr("Open Image"), homePath,
                                            type + " Files ( *." + type.toLower() + " )");
            }
        } break;
    case FileType::VIDEO: {

    } break;
    case FileType::AUDIO_VIDEO: {

    } break;
    case FileType::TEXT: {

    }
    default: break;

    }

    le->insert(path);
}

void ConvertGui::checkRequirements() {
    Converter conv;
    std::string which;
    #ifdef __linux
    which = "/usr/bin/which ";
    #endif
    if (conv.checkProgram(which + "convert")) {
        ui->tabImg->setEnabled(true);
        ui->lblImgNA->hide();
    } else {
        ui->tabImg->setDisabled(true);
        ui->lblImgNA->show();
    }
    if (conv.checkProgram(which + "ffmpeg")) {
        ui->tabAudio->setEnabled(true);
        ui->tabVideo->setEnabled(true);
        ui->lblAudNA->hide();
        ui->lblVidNA->hide();
    } else {
        ui->tabAudio->setDisabled(true);
        ui->tabVideo->setDisabled(true);
        ui->lblAudNA->show();
        ui->lblVidNA->show();
    }
    if (conv.checkProgram(which + "soffice")) {
        ui->tabText->setEnabled(true);
        ui->tabData->setEnabled(true);
        ui->tabPresentation->setEnabled(true);
        ui->lblTxtNA->hide();
        ui->lblDataNA->hide();
        ui->lblPresNA->hide();
    } else {
        ui->tabText->setDisabled(true);
        ui->tabData->setDisabled(true);
        ui->tabPresentation->setDisabled(true);
        ui->lblTxtNA->show();
        ui->lblDataNA->show();
        ui->lblPresNA->show();
    }
}

void ConvertGui::getLineEdits(){
    lineEdits = this->findChildren<QLineEdit*>();
}

void ConvertGui::clearLineEdits(){
    for(auto const& value : lineEdits) {
       value->setText("");
    }
}

void ConvertGui::openAboutWindow() {
    AboutWindow *aW = AboutWindow::openWindow();
    if (aW != 0) {
        const int fHeight = aW->height();
        const int fWidth = aW->width();
        aW->setFixedSize(fWidth, fHeight);
        aW->show();
    }
}


