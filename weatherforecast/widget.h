#ifndef WIDGET_H
#define WIDGET_H

#include <QMenu>
#include <QWidget>
#include <QNetworkReply>
#include <QLabel>
#include <QList>
#include "citycodeutils.h"
#include "day.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    day days[7];

    QList<QLabel *> mDateList;
    QList<QLabel *> mWeekList;
    QList<QLabel *> mIconList;
    QList<QLabel *> mWeaTypeList;
    QList<QLabel *> mAirqList;
    QList<QLabel *> mFxList;
    QList<QLabel *> mFlList;

    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;

public slots:
    void readHttpRely(QNetworkReply *reply);

private slots:
    void on_pushButton_clicked();

private:
    Ui::Widget *ui;
    QMenu *menuQuit;
    QPoint moffset;
    QNetworkReply *reply;
    QNetworkAccessManager *manager;
    QString strUrl;
    CityCodeUtils cityCodeUtils;
    QMap<QString,QString> mTypeMap;
    void paresWeatherJsonData(QByteArray rawData);
    void paresWeatherJsonDataNew(QByteArray rawData);
    void updateUi();
    void drawTempLineHigh();
    void drawTempLineLow();
};
#endif // WIDGET_H
