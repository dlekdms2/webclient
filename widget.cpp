#include "widget.h"
#include "ui_widget.h"

#include <QFile>
#include <QTextStream>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
    , socket_(nullptr)
    , useSSL_(false)
{
    ui->setupUi(this);
    loadFromFile();
    updateButtons();

    connect(ui->rbSsl, &QRadioButton::toggled, this, &Widget::on_rbSsl_toggled);
}

Widget::~Widget()
{
    saveToFile();
    delete socket_;
    delete ui;
}

void Widget::doConnected(){
    QString msg = "Connected\r\n";
    ui->pteMessage->insertPlainText(msg);
}

void Widget::doDisconnected(){
    QString msg = "Disconnected\r\n";
    ui->pteMessage->insertPlainText(msg);
}

void Widget::doReadyRead(){
    if (socket_) {
        QByteArray data = socket_->readAll();
        ui->pteMessage->insertPlainText(QString::fromUtf8(data));
    }
}

void Widget::socketStateChanged(QAbstractSocket::SocketState state) {
    updateButtons();
}

void Widget::on_pnConnect_clicked()
{

    if (socket_) {
        delete socket_;
        socket_ = nullptr;
    }

    // SSL On/Off
    if (useSSL_) {
        auto *sslSocket = new QSslSocket(this);
        socket_ = sslSocket;
        connect(sslSocket, &QSslSocket::encrypted, this, &Widget::doConnected);
    } else {
        socket_ = new QTcpSocket(this);
    }

    connect(socket_, &QAbstractSocket::connected, this, &Widget::doConnected);
    connect(socket_, &QAbstractSocket::disconnected, this, &Widget::doDisconnected);
    connect(socket_, &QIODevice::readyRead, this, &Widget::doReadyRead);
    connect(socket_, &QAbstractSocket::stateChanged, this, &Widget::socketStateChanged);

    socket_->connectToHost(ui->leHost->text(), ui->lePort->text().toUShort());


    if (useSSL_) {
        auto *sslSocket = qobject_cast<QSslSocket*>(socket_);
        if (sslSocket) {
            sslSocket->startClientEncryption();
        }
    }
}
void Widget::on_pnDisconnect_clicked()
{
    if (socket_ && (socket_->state() == QAbstractSocket::ConnectedState)) {
        socket_->disconnectFromHost();
    }
}

void Widget::on_pbSend_clicked()
{
    if (socket_ && socket_->state() == QAbstractSocket::ConnectedState) {
        socket_->write(ui->pteSend->toPlainText().toUtf8());
        ui->pteSend->clear();
    }
}

void Widget::on_rbSsl_toggled(bool checked)
{
    useSSL_ = checked;
}

void Widget::updateButtons(){
    bool connected = socket_ && socket_->state() == QAbstractSocket::ConnectedState;
    ui->pnConnect->setEnabled(!connected && !useSSL_);
    ui->pnDisconnect->setEnabled(connected);
    ui->pbSend->setEnabled(connected);
}

void Widget::saveToFile()
{
    QFile file("settings.txt");
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        out << ui->leHost->text() << "\n";
        out << ui->lePort->text() << "\n";
        out << ui->pteSend->toPlainText() << "\n";
        out << ui->pteMessage->toPlainText() << "\n";
        out << (useSSL_ ? "1\n" : "0\n");
    }
    file.close();
}

void Widget::loadFromFile()
{
    QFile file("settings.txt");
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        ui->leHost->setText(in.readLine());
        ui->lePort->setText(in.readLine());
        ui->pteSend->setPlainText(in.readLine());
        ui->pteMessage->setPlainText(in.readAll());
        bool sslState = in.readLine().trimmed() == "1";
        ui->rbSsl->setChecked(sslState);
        useSSL_ = sslState;
    }
    file.close();
}

