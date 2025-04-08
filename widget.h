#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTcpSocket>
#include <QSslSocket>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

public slots:
    void doConnected();
    void doDisconnected();
    void doReadyRead();
    void socketStateChanged(QAbstractSocket::SocketState state);

private slots:
    void on_pnConnect_clicked();
    void on_pnDisconnect_clicked();
    void on_pbSend_clicked();
    void on_rbSsl_toggled(bool checked);

private:
    Ui::Widget *ui;
    QTcpSocket *socket_;
    bool useSSL_;

    void updateButtons();
    void saveToFile();
    void loadFromFile();
};

#endif // WIDGET_H
