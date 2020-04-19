#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_button_importSession_clicked();

    void on_button_saveSession_clicked();

    void on_button_scriptHelpWindow_clicked();

    void on_button_closeTransScript_clicked();

    void on_button_newTransScript_clicked();

    void on_button_stopTransScript_clicked();

    void on_button_startTransScript_clicked();

    void on_qLine_flagSzSrc_textEdited(const QString &arg1);

    void on_qLine_flagSzDst_textEdited(const QString &arg1);

    void on_qLine_flagSzSeq_textEdited(const QString &arg1);

    void on_qLine_flagSzBuf_textEdited(const QString &arg1);

    void on_qLine_flagSzChk_textEdited(const QString &arg1);

    void on_qLine_flagSzRec_textEdited(const QString &arg1);

    void on_qLine_flagSzDat_textEdited(const QString &arg1);

    void on_qLine_flagSzAnu_textEdited(const QString &arg1);

    void on_qLine_flagSzPlc_textEdited(const QString &arg1);

    void on_qLine_flagSzSyn_textEdited(const QString &arg1);

    void on_qLine_flagSzAck_textEdited(const QString &arg1);

    void on_qLine_flagSzUrg_textEdited(const QString &arg1);

    void on_qLine_flagSzPsh_textEdited(const QString &arg1);

    void on_qLine_flagSzRst_textEdited(const QString &arg1);

    void on_qLine_flagSzFin_textEdited(const QString &arg1);

    void on_combo_serialDevSel_activated(int index);

    void on_button_comConnect_clicked();

    void on_button_Refresh_clicked();

    void readData();

    void on_qLine_msgLine_returnPressed();

    void on_lineEdit_editingFinished();

    void on_button_clearRecieved_clicked();

    void on_chkbox_displayDec_clicked();

    void on_chkbox_displayASC_clicked();

    void on_chkbox_displayBin_clicked();

    void on_chkbox_displayHex_clicked();

    void on_button_pauseSession_clicked();

    void on_button_refreshIP_clicked();

private:
    Ui::MainWindow *ui;
    void refreshComList();
    void setComLabels();
    void setComSettings();
    void setAvailableBaudRates();
    void setBitSum();
    void handleFlagSizeChanged(QLineEdit *obj, const QString &arg1);
    void sendMessage(QVector<QVector<QChar>>);
    QList<QChar> addrVerification(QLineEdit *, QLineEdit *);
    quint16 crc16(QByteArray,quint16);
    quint16 crc16(QVector<QChar>,quint16);

    QSerialPort *uartPort = nullptr;
};

#endif // MAINWINDOW_H
