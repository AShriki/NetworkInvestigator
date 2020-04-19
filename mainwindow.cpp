#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPortInfo>
#include <QDebug>

QVector<QString> scripts;

QVector<QChar> receiveBuffer;

QHash<QString,qint32> flagSizes;
QHash<QString,qint32> flagLocations;

QVector<QVector<QChar>> sentMessages;
QVector<QVector<QChar>> recievedMessages;

QStringList flagOrder;
quint32 packetSize;

quint8 textRepresentation;
QString prependByte;

quint16 packetsDisplayed;

bool paused;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    uartPort = new QSerialPort(this);
    connect(uartPort, SIGNAL(readyRead()), this , SLOT(readData()));

    ui->setupUi(this);

    ui->combo_serialDevSel->setItemText(0,"0 coms");

    scripts.append("");
    textRepresentation = 16;
    prependByte = "0x";
    paused = false;
    setBitSum();
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::setBitSum(){


    flagSizes["anu"] = ui->qLine_flagSzAnu->text().toInt();
    flagSizes["buf"] = ui->qLine_flagSzBuf->text().toInt();
    flagSizes["chk"] = ui->qLine_flagSzChk->text().toInt();
    flagSizes["dat"] = ui->qLine_flagSzDat->text().toInt();
    flagSizes["dst"] = ui->qLine_flagSzDst->text().toInt();
    flagSizes["plc"] = ui->qLine_flagSzPlc->text().toInt();
    flagSizes["rec"] = ui->qLine_flagSzRec->text().toInt();
    flagSizes["seq"] = ui->qLine_flagSzSeq->text().toInt();
    flagSizes["src"] = ui->qLine_flagSzSrc->text().toInt();
    flagSizes["flg"] = 8;

    QStringList l = ui->qLine_packetFormatString->text().split(QRegExp("\\]|\\["), QString::SkipEmptyParts);

    qint16 bits = 0;

    for(QString s : l){
        if(flagSizes.value(s) == 0){
            QString s2 = "[" + s + "]";
            int start = ui->qLine_packetFormatString->text().indexOf(s2);
            s2 = ui->qLine_packetFormatString->text().left(start) + ui->qLine_packetFormatString->text().right(start + s2.length());
            ui->qLine_packetFormatString->clear();
            ui->qLine_packetFormatString->setText(s2);
        }
        flagLocations[s] = bits / 8;
        bits += flagSizes.value(s);
    }
    packetSize = bits/8;
    ui->label_packetSize->setText(QString::number(bits / 8) + " bytes and " + QString::number(bits % 8) + " bits");

}

void MainWindow::on_button_importSession_clicked()
{

}

void MainWindow::on_button_saveSession_clicked()
{

}

void MainWindow::on_button_scriptHelpWindow_clicked()
{

}

void MainWindow::on_button_closeTransScript_clicked()
{
    if(scripts.length() == 0)
        return;
    scripts.remove(ui->tabs_scripts->currentIndex());
    ui->tabs_scripts->removeTab(ui->tabs_scripts->currentIndex());
}

void MainWindow::on_button_newTransScript_clicked()
{
    QString sVis = "Script " + QString::number(scripts.length() + 1);
    scripts.append("");
    ui->tabs_scripts->addTab(new QTabWidget,sVis);
}

void MainWindow::on_button_stopTransScript_clicked()
{

}

void MainWindow::on_button_startTransScript_clicked()
{

}

void MainWindow::refreshComList()
{

    if(uartPort->isOpen()){
        uartPort->flush();
        uartPort->close();
        ui->button_comConnect->setText("Connect");
    }

    ui->combo_serialDevSel->clear();
    ui->combo_serialDevSel->addItem(QString::number(QSerialPortInfo::availablePorts().length()) + " coms");
    for(QSerialPortInfo p : QSerialPortInfo::availablePorts()){
        ui->combo_serialDevSel->addItem(p.portName(),*(new QVariant(p.serialNumber())));
    }
    setComLabels();

}

void MainWindow::setComLabels()
{

    if(ui->combo_serialDevSel->currentIndex() == 0){
        ui->HDesc->setText("");
        ui->HSn->setText("");
        ui->HID->setText("");
        ui->HLoc->setText("");
        ui->HMfr->setText("");
    }
    else{
        QString s = ui->combo_serialDevSel->currentData().toString();

        QSerialPortInfo *p = new QSerialPortInfo(ui->combo_serialDevSel->currentText());

        ui->HDesc->setText(p->description());
        ui->HMfr->setText(p->manufacturer());
        ui->HLoc->setText(p->systemLocation());
        ui->HSn->setText(p->serialNumber());
        if(p->hasProductIdentifier())
            ui->HID->setText(QString(p->productIdentifier()));
    }

}

void MainWindow::setAvailableBaudRates(){

    if(ui->combo_BaudSel->currentIndex() == 0){
        ui->combo_BaudSel->clear();
        return;
    }
    QString s = ui->combo_serialDevSel->currentData().toString();

    QSerialPortInfo *p = new QSerialPortInfo(ui->combo_serialDevSel->currentText());

    for(qint32 n : p->standardBaudRates()){
        ui->combo_BaudSel->addItem(QString::number(n));
    }

}

void MainWindow::on_qLine_flagSzSrc_textEdited(const QString &arg1)
{
    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    handleFlagSizeChanged(obj,arg1);
}

void MainWindow::on_qLine_flagSzDst_textEdited(const QString &arg1)
{
    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    handleFlagSizeChanged(obj,arg1);
}

void MainWindow::on_qLine_flagSzSeq_textEdited(const QString &arg1)
{
    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    handleFlagSizeChanged(obj,arg1);
}

void MainWindow::on_qLine_flagSzBuf_textEdited(const QString &arg1)
{
    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    handleFlagSizeChanged(obj,arg1);
}

void MainWindow::on_qLine_flagSzChk_textEdited(const QString &arg1)
{
    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    handleFlagSizeChanged(obj,arg1);
}

void MainWindow::on_qLine_flagSzRec_textEdited(const QString &arg1)
{
    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    handleFlagSizeChanged(obj,arg1);
}

void MainWindow::on_qLine_flagSzDat_textEdited(const QString &arg1)
{
    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    handleFlagSizeChanged(obj,arg1);
}

void MainWindow::on_qLine_flagSzAnu_textEdited(const QString &arg1)
{
    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    handleFlagSizeChanged(obj,arg1);
}

void MainWindow::on_qLine_flagSzPlc_textEdited(const QString &arg1)
{
    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    handleFlagSizeChanged(obj,arg1);
}

void MainWindow::on_qLine_flagSzSyn_textEdited(const QString &arg1)
{
    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    handleFlagSizeChanged(obj,arg1);
}

void MainWindow::on_qLine_flagSzAck_textEdited(const QString &arg1)
{
    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    handleFlagSizeChanged(obj,arg1);
}

void MainWindow::on_qLine_flagSzUrg_textEdited(const QString &arg1)
{
    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    handleFlagSizeChanged(obj,arg1);
}

void MainWindow::on_qLine_flagSzPsh_textEdited(const QString &arg1)
{
    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    handleFlagSizeChanged(obj,arg1);
}

void MainWindow::on_qLine_flagSzRst_textEdited(const QString &arg1)
{
    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    handleFlagSizeChanged(obj,arg1);
}

void MainWindow::on_qLine_flagSzFin_textEdited(const QString &arg1)
{
    QLineEdit *obj = qobject_cast<QLineEdit *>(sender());
    handleFlagSizeChanged(obj,arg1);
}

void MainWindow::handleFlagSizeChanged(QLineEdit *obj,const QString &arg1){

    flagLocations.clear();
    if(obj->text().length() == 0)
        return;
    else{
        if(arg1 == "0")
            obj->setText("1");
    }

    setBitSum();
}

void MainWindow::on_combo_serialDevSel_activated(int index)
{
    //TODO Pause all scripts
    if(uartPort->isOpen()){
        uartPort->flush();
        uartPort->close();
        packetsDisplayed = 0;
        ui->button_comConnect->setText("Connect");
    }
    setComLabels();
    setAvailableBaudRates();

}

void MainWindow::setComSettings()
{
    uartPort->setPort(*(new QSerialPortInfo(ui->combo_serialDevSel->currentText())));
    uartPort->setBaudRate(ui->combo_BaudSel->currentText().toUInt());
    uartPort->setFlowControl(QSerialPort::NoFlowControl);
    uartPort->setParity(QSerialPort::NoParity);
    uartPort->setDataBits(QSerialPort::Data8);
    uartPort->setStopBits(QSerialPort::OneStop);
}

void MainWindow::on_button_comConnect_clicked()
{
    setComSettings();

    if(!uartPort->isOpen()){
        if(uartPort->open(QIODevice::ReadWrite) && uartPort->isReadable() && uartPort->isWritable()){
            setComSettings();
            ui->button_comConnect->setText("Disconnect");
        }
    }
    else{
        uartPort->clear();
        uartPort->close();
        packetsDisplayed = 0;
        QSerialPort::SerialPortError err = uartPort->error();
        if(err == QSerialPort::NoError){
            ui->button_comConnect->setText("Connect");
        }else {
            uartPort->clearError();
        }
    }
}

void MainWindow::on_button_Refresh_clicked()
{
    refreshComList();
    ui->combo_BaudSel->clear();
}

void MainWindow::readData(){

    static unsigned int bytesToRead;

    if( (uartPort->bytesAvailable() + bytesToRead) < packetSize || paused)
        return;
    unsigned int i;
    static quint16 bytesRead = 0;

    static QByteArray tmp;

    bytesToRead += uartPort->bytesAvailable();
    tmp += uartPort->readAll();

    for(i = 0 ;i < (bytesToRead/(packetSize) * packetSize); i++){
        if(i == 0)
            ui->text_messageIn->insertPlainText(QString::number(packetsDisplayed) + "   " );
        if(textRepresentation != 0)
            ui->text_messageIn->insertPlainText("|" + QString::number(i) + ": " + prependByte + QString::number(quint8(tmp[i]),textRepresentation) + " ");
        else
            ui->text_messageIn->insertPlainText("|" + QString::number(i) + ": " + QString(char(tmp[i])) + " ");
        if( i == (packetSize - 1) && i > 0){

            uint16_t a = crc16(tmp,flagLocations["chk"]);
            uint16_t b = (quint8(tmp[flagLocations["chk"]]) * 0x100) + quint8(tmp[flagLocations["chk"] + 1]);
            if(a == b)
                ui->label_computedChecksum->setText("True");
            else
                ui->label_computedChecksum->setText("False");

            ui->text_messageIn->insertPlainText("\n");

            ui->text_messageIn->setTextCursor(QTextCursor(ui->text_messageIn->document()));

            packetsDisplayed++;
            tmp = tmp.right(bytesToRead - packetSize);
            bytesToRead -= packetSize;
            bytesRead += packetSize;
            i = -1;
        }
    }
}

QList<QChar> MainWindow::addrVerification(QLineEdit *addrField,QLineEdit *fieldSize){

    QList<QChar> addr;
    int i;

    for(i = 0 ; i < fieldSize->text().toInt()/8 ; i++){
         addr.append('\0');
    }

    if(addrField->text().contains(QRegExp("[^0-9.]"))){
        addrField->setText("0");
    }else{
        QList<QString> addrToken = addrField->text().split(QRegExp("[.:]"),QString::SkipEmptyParts);

        if(addrToken.length()*8 > fieldSize->text().toInt() || addrField->text().length() == 0){
            qDebug() << "source address size mismatch or no address\n addrField:" << addrField->text() << "addrSize" << fieldSize->text().toInt();
            addrField->clear();
            int i;
            for(i = 0 ; i < fieldSize->text().toInt()/8 ; i++){
                addr[i] = '\0';
            }
            addrField->setText("0");
        }
        else{
            addr.clear();
            for(QString s : addrToken){
                bool sValid = false;
                quint16 subAddr = s.toUInt(&sValid);
                if(sValid && subAddr < 0x100){
                    addr.push_back(s.toUInt());
                }
            }
        }
    }
    return addr;
}

void MainWindow::on_qLine_msgLine_returnPressed()
{
    QList<QString> listTokens = ui->qLine_msgLine->text().split(QRegExp(" "), QString::SkipEmptyParts);
    QList<QChar> data;
    QList<QChar> tmpAddress;
    QHash<QString,QList<QChar>> fieldData;
    QVector<QChar> tmpMsg;

    QStringList msgFormat = ui->qLine_packetFormatString->text().split(QRegExp("\\]|\\["), QString::SkipEmptyParts);

    quint32 seqNum = 1;
    quint32 mask = 0xFF;
    quint16 crc;

    QVector<QVector<QChar>> sendBuffer;
    // seperate the message line into tokens
    for(QString tmp  : listTokens){
        bool bStatus = false;
        if(tmp[0] == '0'){

            // if we are looking at a non decimal number
            switch(tmp[1].unicode())
            {
            case 'x' : {if(tmp.right(tmp.length()-2).toUInt(&bStatus,16) <= 0xFF)data.append(tmp.right(tmp.length()-2).toUInt(&bStatus,16));break;}
            case 'b' : {if(tmp.right(tmp.length()-2).toUInt(&bStatus,2) <= 0b11111111)data.append(tmp.right(tmp.length()-2).toUInt(&bStatus,2));break;}
            case 'o' : {if(tmp.right(tmp.length()-2).toUInt(&bStatus,8) < 0400)data.append(tmp.right(tmp.length()-2).toUInt(&bStatus,8));break;}
            default:{break;}
            }
        }
        else if(tmp.contains(QRegExp("[A-Za-z]"))){// otherwise we a re looking at a string

            for(QChar c : tmp)
                data.append(c);

        }else{ // else it's a digit
            if(tmp.toUInt(&bStatus,10) <= 255 && bStatus)
                data.append(tmp.toUInt(&bStatus,10));
        }
    }

    setBitSum();

    int i = 0;
    fieldData["dat"] = data;
    fieldData["src"] = addrVerification(ui->qLine_myAddr,ui->qLine_flagSzSrc);

    fieldData["dst"].clear();

    for(QChar c : ui->combo_addressesFound->currentText())
        fieldData["dst"].append(c);

    for(i = 0 ; i<flagSizes["seq"]/8 - 1;i++)
        fieldData["seq"].append(0);
    fieldData["seq"].append(seqNum);
    for(i = 0 ; i<flagSizes["buf"] / 8 ; i++)
        fieldData["buf"].append(0);
    for(i = 0 ; i < flagSizes["anu"] / 8 ; i++)
        fieldData["anu"].append(0);
    for(i = 0 ; i < flagSizes["chk"] / 8 ; i++)
        fieldData["chk"].append(0);
    for(i = 0 ; i < flagSizes["plc"] / 8 ; i++)
        fieldData["plc"].append(0);
    fieldData["flg"].append(0b000010);

    //first packet
    for(QString s : msgFormat){
        int bytes = 0;
        if(s != "dat"){
            while(!fieldData[s].isEmpty() && bytes++ < flagSizes["dat"]/8){
                tmpMsg.append(fieldData[s].first());
                fieldData[s].removeFirst();
            }
        }else{
            // append data and empty spaces to message
            for(int j = 0 ; j < flagSizes[s]/8 ; j++){
                if(fieldData[s].length() == 0){
                    tmpMsg.insert(flagLocations[s],0);
                }else{
                    tmpMsg.append(fieldData[s].first());
                    fieldData[s].removeFirst();
                }
            }
        }
    }
    crc = 0;

    crc = crc16(tmpMsg,flagLocations["chk"]);
    tmpMsg[flagLocations["chk"]] = (crc & 0xFF00) >> 8;
    tmpMsg[flagLocations["chk"] + 1] = (crc & 0xFF);

    sendBuffer.append(tmpMsg);
    while(!fieldData["dat"].isEmpty()){
        seqNum++;
        qint32 j = 0;
        crc = 0;
        tmpMsg[flagLocations["flg"]] = 0;

        int dataStartPoint;

        if(fieldData["dat"].length() >= flagSizes["dat"]/8){
            dataStartPoint = flagLocations["dat"];
        }
        else{
            dataStartPoint = flagLocations["dat"] + (flagSizes["dat"]/8 - fieldData["dat"].length());
        }
        for(j = flagLocations["dat"] ; j < flagLocations["dat"] + flagSizes["dat"]/8 ; j++){
            if(j < dataStartPoint)
                tmpMsg[j] = 0;
            else{
                tmpMsg[j] = fieldData["dat"].first();
                fieldData["dat"].removeFirst();
            }
        }
        for(j = 0 , i = flagSizes["seq"]/8 + flagLocations["seq"] - 1 ; i >= flagLocations["seq"] ;i--){
            tmpMsg[i] = ((seqNum & mask) >> j);
            mask *= 0x100;
            j+=8;
        }
        crc = crc16(tmpMsg,flagLocations["chk"]);

        tmpMsg[flagLocations["chk"]] = (crc & 0xFF00) >> 8;
        tmpMsg[flagLocations["chk"] + 1] = (crc & 0xFF);

        sendBuffer.append(tmpMsg);
        mask = 0xFF;
    }
    sendMessage(sendBuffer);
}

void MainWindow::sendMessage(QVector<QVector<QChar>> sendBuffer){
    if(uartPort->isOpen() && uartPort->isWritable()){
        QVector<QChar> tmpMsg;
        while(!sendBuffer.isEmpty()){
            tmpMsg.clear();
            for(QChar c : sendBuffer[0]){
                uartPort->putChar(c.unicode());
                tmpMsg.append(c);
            }
            sentMessages.append(tmpMsg);
        }
    }
}

quint16 MainWindow::crc16(QByteArray str , quint16 len){
    unsigned short crc16tab[256] = {
        0x0,0xAC9A,0xF5AE,0x5934,0x47C6,0xEB5C,0xB268,0x1EF2,0x8F8C,0x2316,0x7A22,0xD6B8,0xC84A,0x64D0,0x3DE4,0x917E,
        0xB382,0x1F18,0x462C,0xEAB6,0xF444,0x58DE,0x1EA,0xAD70,0x3C0E,0x9094,0xC9A0,0x653A,0x7BC8,0xD752,0x8E66,0x22FC,
        0xCB9E,0x6704,0x3E30,0x92AA,0x8C58,0x20C2,0x79F6,0xD56C,0x4412,0xE888,0xB1BC,0x1D26,0x3D4,0xAF4E,0xF67A,0x5AE0,
        0x781C,0xD486,0x8DB2,0x2128,0x3FDA,0x9340,0xCA74,0x66EE,0xF790,0x5B0A,0x23E,0xAEA4,0xB056,0x1CCC,0x45F8,0xE962,
        0x3BA6,0x973C,0xCE08,0x6292,0x7C60,0xD0FA,0x89CE,0x2554,0xB42A,0x18B0,0x4184,0xED1E,0xF3EC,0x5F76,0x642,0xAAD8,
        0x8824,0x24BE,0x7D8A,0xD110,0xCFE2,0x6378,0x3A4C,0x96D6,0x7A8,0xAB32,0xF206,0x5E9C,0x406E,0xECF4,0xB5C0,0x195A,
        0xF038,0x5CA2,0x596,0xA90C,0xB7FE,0x1B64,0x4250,0xEECA,0x7FB4,0xD32E,0x8A1A,0x2680,0x3872,0x94E8,0xCDDC,0x6146,
        0x43BA,0xEF20,0xB614,0x1A8E,0x47C,0xA8E6,0xF1D2,0x5D48,0xCC36,0x60AC,0x3998,0x9502,0x8BF0,0x276A,0x7E5E,0xD2C4,
        0x774C,0xDBD6,0x82E2,0x2E78,0x308A,0x9C10,0xC524,0x69BE,0xF8C0,0x545A,0xD6E,0xA1F4,0xBF06,0x139C,0x4AA8,0xE632,
        0xC4CE,0x6854,0x3160,0x9DFA,0x8308,0x2F92,0x76A6,0xDA3C,0x4B42,0xE7D8,0xBEEC,0x1276,0xC84,0xA01E,0xF92A,0x55B0,
        0xBCD2,0x1048,0x497C,0xE5E6,0xFB14,0x578E,0xEBA,0xA220,0x335E,0x9FC4,0xC6F0,0x6A6A,0x7498,0xD802,0x8136,0x2DAC,
        0xF50,0xA3CA,0xFAFE,0x5664,0x4896,0xE40C,0xBD38,0x11A2,0x80DC,0x2C46,0x7572,0xD9E8,0xC71A,0x6B80,0x32B4,0x9E2E,
        0x4CEA,0xE070,0xB944,0x15DE,0xB2C,0xA7B6,0xFE82,0x5218,0xC366,0x6FFC,0x36C8,0x9A52,0x84A0,0x283A,0x710E,0xDD94,
        0xFF68,0x53F2,0xAC6,0xA65C,0xB8AE,0x1434,0x4D00,0xE19A,0x70E4,0xDC7E,0x854A,0x29D0,0x3722,0x9BB8,0xC28C,0x6E16,
        0x8774,0x2BEE,0x72DA,0xDE40,0xC0B2,0x6C28,0x351C,0x9986,0x8F8,0xA462,0xFD56,0x51CC,0x4F3E,0xE3A4,0xBA90,0x160A,
        0x34F6,0x986C,0xC158,0x6DC2,0x7330,0xDFAA,0x869E,0x2A04,0xBB7A,0x17E0,0x4ED4,0xE24E,0xFCBC,0x5026,0x912,0xA588
    };
    int counter;
    unsigned short crc = 0;
    for( counter = 0; counter < len; counter++){
        crc = (crc<<8) ^ crc16tab[((crc>>8) ^ str[counter])&0x00FF];
    }
    return crc;
}
quint16 MainWindow::crc16(QVector<QChar> str , quint16 len){
    unsigned short crc16tab[256] = {
        0x0,0xAC9A,0xF5AE,0x5934,0x47C6,0xEB5C,0xB268,0x1EF2,0x8F8C,0x2316,0x7A22,0xD6B8,0xC84A,0x64D0,0x3DE4,0x917E,
        0xB382,0x1F18,0x462C,0xEAB6,0xF444,0x58DE,0x1EA,0xAD70,0x3C0E,0x9094,0xC9A0,0x653A,0x7BC8,0xD752,0x8E66,0x22FC,
        0xCB9E,0x6704,0x3E30,0x92AA,0x8C58,0x20C2,0x79F6,0xD56C,0x4412,0xE888,0xB1BC,0x1D26,0x3D4,0xAF4E,0xF67A,0x5AE0,
        0x781C,0xD486,0x8DB2,0x2128,0x3FDA,0x9340,0xCA74,0x66EE,0xF790,0x5B0A,0x23E,0xAEA4,0xB056,0x1CCC,0x45F8,0xE962,
        0x3BA6,0x973C,0xCE08,0x6292,0x7C60,0xD0FA,0x89CE,0x2554,0xB42A,0x18B0,0x4184,0xED1E,0xF3EC,0x5F76,0x642,0xAAD8,
        0x8824,0x24BE,0x7D8A,0xD110,0xCFE2,0x6378,0x3A4C,0x96D6,0x7A8,0xAB32,0xF206,0x5E9C,0x406E,0xECF4,0xB5C0,0x195A,
        0xF038,0x5CA2,0x596,0xA90C,0xB7FE,0x1B64,0x4250,0xEECA,0x7FB4,0xD32E,0x8A1A,0x2680,0x3872,0x94E8,0xCDDC,0x6146,
        0x43BA,0xEF20,0xB614,0x1A8E,0x47C,0xA8E6,0xF1D2,0x5D48,0xCC36,0x60AC,0x3998,0x9502,0x8BF0,0x276A,0x7E5E,0xD2C4,
        0x774C,0xDBD6,0x82E2,0x2E78,0x308A,0x9C10,0xC524,0x69BE,0xF8C0,0x545A,0xD6E,0xA1F4,0xBF06,0x139C,0x4AA8,0xE632,
        0xC4CE,0x6854,0x3160,0x9DFA,0x8308,0x2F92,0x76A6,0xDA3C,0x4B42,0xE7D8,0xBEEC,0x1276,0xC84,0xA01E,0xF92A,0x55B0,
        0xBCD2,0x1048,0x497C,0xE5E6,0xFB14,0x578E,0xEBA,0xA220,0x335E,0x9FC4,0xC6F0,0x6A6A,0x7498,0xD802,0x8136,0x2DAC,
        0xF50,0xA3CA,0xFAFE,0x5664,0x4896,0xE40C,0xBD38,0x11A2,0x80DC,0x2C46,0x7572,0xD9E8,0xC71A,0x6B80,0x32B4,0x9E2E,
        0x4CEA,0xE070,0xB944,0x15DE,0xB2C,0xA7B6,0xFE82,0x5218,0xC366,0x6FFC,0x36C8,0x9A52,0x84A0,0x283A,0x710E,0xDD94,
        0xFF68,0x53F2,0xAC6,0xA65C,0xB8AE,0x1434,0x4D00,0xE19A,0x70E4,0xDC7E,0x854A,0x29D0,0x3722,0x9BB8,0xC28C,0x6E16,
        0x8774,0x2BEE,0x72DA,0xDE40,0xC0B2,0x6C28,0x351C,0x9986,0x8F8,0xA462,0xFD56,0x51CC,0x4F3E,0xE3A4,0xBA90,0x160A,
        0x34F6,0x986C,0xC158,0x6DC2,0x7330,0xDFAA,0x869E,0x2A04,0xBB7A,0x17E0,0x4ED4,0xE24E,0xFCBC,0x5026,0x912,0xA588
    };
    int counter;
    unsigned short crc = 0;
    for( counter = 0; counter < len; counter++)
        crc = (crc<<8) ^ crc16tab[((crc>>8) ^ str[counter].toLatin1())];
    return crc;
}
void MainWindow::on_lineEdit_editingFinished()
{
    QStringList tokens = ui->qLine_packetOrder->text().split(QRegExp("\\]|\\["), QString::SkipEmptyParts);
    if(tokens.length() > 6){
        ui->qLine_packetOrder->setText("[urg][ack][psh][rst][syn][fin]");
        tokens = ui->qLine_packetOrder->text().split(QRegExp("\\]|\\["), QString::SkipEmptyParts);
    }
    flagOrder = tokens;
}

void MainWindow::on_button_clearRecieved_clicked()
{
    ui->text_messageIn->clear();
    packetsDisplayed = 0;
}

void MainWindow::on_chkbox_displayDec_clicked()
{
    ui->chkbox_displayASC->setCheckState(Qt::Unchecked);
    ui->chkbox_displayBin->setCheckState(Qt::Unchecked);
    ui->chkbox_displayHex->setCheckState(Qt::Unchecked);
    textRepresentation = 10;
    prependByte = "";
}

void MainWindow::on_chkbox_displayASC_clicked()
{
    ui->chkbox_displayHex->setCheckState(Qt::Unchecked);
    ui->chkbox_displayBin->setCheckState(Qt::Unchecked);
    ui->chkbox_displayDec->setCheckState(Qt::Unchecked);
    textRepresentation = 0;
}

void MainWindow::on_chkbox_displayBin_clicked()
{
    ui->chkbox_displayASC->setCheckState(Qt::Unchecked);
    ui->chkbox_displayHex->setCheckState(Qt::Unchecked);
    ui->chkbox_displayDec->setCheckState(Qt::Unchecked);
    textRepresentation = 2;
    prependByte = "0b";
}

void MainWindow::on_chkbox_displayHex_clicked()
{
    ui->chkbox_displayASC->setCheckState(Qt::Unchecked);
    ui->chkbox_displayBin->setCheckState(Qt::Unchecked);
    ui->chkbox_displayDec->setCheckState(Qt::Unchecked);
    textRepresentation = 16;
    prependByte = "0x";
}

void MainWindow::on_button_pauseSession_clicked()
{
    paused = !paused;
    if(paused)
        ui->button_pauseSession->setText("Resume");
    else
        ui->button_pauseSession->setText("Pause");
}

void MainWindow::on_button_refreshIP_clicked()
{
    if(ui->qLine_myAddr->text().isEmpty()){

        return;
    }
}
