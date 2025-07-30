#include "widget.h"
#include "ui_widget.h"
#include "day.h"
#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMessageBox>
#include <QMouseEvent>
#include <QNetworkAccessManager>
#include <QPainter>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    setFixedSize(603,1141);
    setWindowFlag(Qt::FramelessWindowHint);
    //新键一个菜单组件
    menuQuit = new QMenu(this);
    //设置菜单项颜色
    menuQuit->setStyleSheet("QMenu::item{color: white}");
    //为菜单组件添加一个动作
    QAction *closeAct = new QAction(QIcon(":/res/close.png"), tr("退出"), this);
    menuQuit->addAction(closeAct);
    //用户选择菜单项并点击后回调函数，退出程序
    connect(menuQuit,&QMenu::triggered,this,[=]{
        this->close();
    });

    //由QNetworkAccessManager网络请求后进行数据读取
    manager = new QNetworkAccessManager(this);
    strUrl = "http://gfeljm.tianqiapi.com/api?unescape=1&version=v91&appid=15166155&appsecret=32TIoiIC";
    QUrl urlTianQi(strUrl);
    //指向指定的url地址
    QNetworkRequest res(urlTianQi);
    reply = manager->get(res);
    //网络请求后进行数据读取
    connect(manager, &QNetworkAccessManager::finished,this,&Widget::readHttpRely);
    connect(ui->lineEditCity, &QLineEdit::returnPressed, this, &Widget::on_pushButton_clicked);

    mDateList << ui->labelDay0 << ui->labelDay1 <<ui->labelDay2
              << ui->labelDay3 << ui->labelDay4 << ui->labelDay5;

    mWeekList << ui->labelDate0 << ui->labelDate1 << ui->labelDate2
              << ui->labelDate3 << ui->labelDate4 << ui->labelDate5;

    mIconList << ui->labelWeathertype0 << ui->labelWeathertype1 << ui->labelWeathertype2
              << ui->labelWeathertype3 << ui->labelWeathertype4 << ui->labelWeathertype5;

    mWeaTypeList << ui->labelWeather0 << ui->labelWeather1 << ui->labelWeather2
                 << ui->labelWeather3<< ui->labelWeather4 << ui->labelWeather5;

    mAirqList << ui->labelAirq0 << ui->labelAirq1 << ui->labelAirq2
              << ui->labelAirq3 <<ui->labelAirq4 << ui->labelAirq5;

    mFxList << ui->labelWindDir0 << ui->labelWindDir1 << ui->labelWindDir2
            << ui->labelWindDir3 << ui->labelWindDir4 << ui->labelWindDir5;

    mFlList << ui->labelWindPow0 << ui->labelWindPow1 << ui->labelWindPow2
            << ui->labelWindPow3 << ui->labelWindPow4 << ui->labelWindPow5;


    //根据keys，设置icon的路径
    mTypeMap.insert("暴雪",":/res/type/BaoXue.png");
    mTypeMap.insert("暴雨",":/res/type/BaoYu.png");
    mTypeMap.insert("暴雨到大暴雨",":/res/type/BaoYuDaBaoYu.png");
    mTypeMap.insert("大暴雨",":/res/type/DaBaoYu.png");
    mTypeMap.insert("大暴雨到 特大暴雨",":/res/type/DaBaoYuTeDaBaoYu.png");
    mTypeMap.insert("大雪",":/res/type/DaXue.png");
    mTypeMap.insert("大雨",":/res/type/DaYu.png");
    mTypeMap.insert("大雨到暴雨",":/res/type/DaYuBaoYu.png");
    mTypeMap.insert("冻雨",":/res/type/DongYu.png");
    mTypeMap.insert("多云",":/res/type/DuoYun.png");
    mTypeMap.insert("浮沉",":/res/type/FuChen.png");
    mTypeMap.insert("雷阵雨",":/res/type/LeiZhenYu.png");
    mTypeMap.insert("雷阵雨伴冰雹",":/res/type/LeiZhenYuBanBingBao.png");
    mTypeMap.insert("霾",":/res/type/Mai.png");
    mTypeMap.insert("强沙尘暴",":/res/type/QiangShanChenBao.png");
    mTypeMap.insert("晴",":/res/type/Qing.png");
    mTypeMap.insert("沙尘暴",":/res/type/ShaChenBao.png");
    mTypeMap.insert("特大暴雨",":/res/type/TeDaBaoYu.png");
    mTypeMap.insert("雾",":/res/type/Wu.png");
    mTypeMap.insert("小雪",":/res/type/XiaoXue.png");
    mTypeMap.insert("小雨",":/res/type/XiaoYu.png");
    mTypeMap.insert("小雨到中雨",":/res/type/XiaoYuZhongYu.png");
    mTypeMap.insert("浮沉",":/res/type/YangSha.png");
    mTypeMap.insert("夜晚多云",":/res/type/YeWanDuoYun.png");
    mTypeMap.insert("夜晚晴",":/res/type/YeWanQing.png");
    mTypeMap.insert("夜晚阵雪",":/res/type/YeWanZhenXue.png");
    mTypeMap.insert("夜晚阵雨",":/res/type/YeWanZhenYu.png");
    mTypeMap.insert("阴",":/res/type/Yin.png");
    mTypeMap.insert("雨夹雪",":/res/type/YuJiaXue.png");
    mTypeMap.insert("阵雪",":/res/type/ZhenXue.png");
    mTypeMap.insert("阵雨",":/res/type/ZhenYu.png");
    mTypeMap.insert("中到大雨",":/res/type/ZhongDaoDaYu.png");
    mTypeMap.insert("中雪",":/res/type/ZhongXue.png");
    mTypeMap.insert("中雪到大雪",":/res/type/ZhongXueDaXue.png");
    mTypeMap.insert("中雨",":/res/type/ZhongYu.png");

    ui->widget0404->installEventFilter(this);
    ui->widget0405->installEventFilter(this);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::RightButton){
         //qDebug() << "检测到鼠标右键按下" << endl;
         menuQuit->exec(QCursor::pos());
    }
    //左键事件，当左键按下实现拖动窗口的功能
    //需要获取三个节点：鼠标当前位置event->global()，
    //窗口当前位置this->pos()，
    //窗口新位置event->global() - this->pos()
    if(event->button() == Qt::LeftButton){
        //qDebug() << "检测到鼠标左键按下" << endl;
        //qDebug() << event->globalPos() << this->pos();
        moffset = event->globalPos() - this->pos();
    }
}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - moffset);
}

bool Widget::eventFilter(QObject *watched, QEvent *event)
{
    if(watched == ui->widget0404 && event->type() == QEvent::Paint){
        drawTempLineHigh();
    }
    if(watched == ui->widget0405 && event->type() == QEvent::Paint){
        drawTempLineLow();
    }
}

void Widget::paresWeatherJsonData(QByteArray rawData){
    qDebug() << "paresWeatherJsonData";
    QJsonDocument jsonObj  = QJsonDocument::fromJson(rawData);
    if(!jsonObj.isNull() && jsonObj.isObject()){
        QJsonObject objRoot = jsonObj.object();
        //解析日期
        QString date = objRoot["date"].toString();
        QString week = objRoot["week"].toString();
        ui->labelCurrentDate->setText(date+" "+week);
        //解析城市名称
        QString cityName = objRoot["city"].toString();
        ui->labelCity->setText(cityName);
        //解析当前温度
        QString tem = objRoot["tem"].toString();
        int intTem = qRound(tem.toDouble());// 将字符串转换为double，然后四舍五入为整数
        ui->labelTemperature->setText(QString::number(intTem) + "℃");
        ui->labelTempRange->setText(objRoot["tem2"].toString()+"℃"+"/"+objRoot["tem1"].toString()+"℃");
        //解析天气类型
        ui->labelWeatherType->setText(objRoot["wea"].toString());
        ui->labelWeather->setPixmap(mTypeMap[objRoot["wea"].toString()]);
        //解析感冒指数
        ui->labelGanmao->setText(objRoot["air_tips"].toString());
        //解析风向和风速
        ui->labelFXType->setText(objRoot["win"].toString());
        ui->labelFXType_2->setText(objRoot["win_speed"].toString());
        //解析pm2.5
        ui->labelPM25Data->setText(objRoot["air_pm25"].toString());
        //解析湿度
        ui->labelShiduData->setText(objRoot["humidity"].toString());
        //解析空气质量
        ui->labelAirData->setText(objRoot["air_level"].toString());

    }
}

void Widget::paresWeatherJsonDataNew(QByteArray rawData)
{
    qDebug()<<"paresWeatherJsonDataNew";
    QJsonDocument jsonDoc = QJsonDocument::fromJson(rawData);
    if(!jsonDoc.isNull() && jsonDoc.isObject()){
        QJsonObject jsonRoot = jsonDoc.object();
        days[0].mcity = jsonRoot["city"].toString();
        days[0].mPm25 = jsonRoot["aqi"].toObject()["pm25"].toString();
        if(jsonRoot.contains("data")&& jsonRoot["data"].isArray()){
            QJsonArray weaArray = jsonRoot["data"].toArray();
            for(int i = 0; i < weaArray.size(); i ++){
                QJsonObject obj = weaArray[i].toObject();
                //qDebug() << obj["date"].toString() << obj["wea"].toString();
                days[i].mDate = obj["date"].toString();
                days[i].mWeek = obj["week"].toString();
                days[i].mWeathType = obj["wea"].toString();
                days[i].mTemp = obj["tem"].toString();
                days[i].mTempLow = obj["tem2"].toString();
                days[i].mTempHigh = obj["tem1"].toString();
                days[i].mFx = obj["win"].toArray()[0].toString();
                days[i].mFl = obj["win_speed"].toString();
                days[i].mAirq = obj["air_level"].toString();
                days[i].mTips = obj["air_tips"].toString();
                days[i].mHu = obj["humidity"].toString();
            }
        }
    }
    updateUi();
}

void Widget::updateUi()
{
    QPixmap pixmap;
    //解析日期
    ui->labelCurrentDate->setText(days[0].mDate+" "+days[0].mWeek);
    //解析城市名称
    ui->labelCity->setText(days[0].mcity);
    //解析当前温度
    ui->labelTemperature->setText(days[0].mTemp + "℃");
    ui->labelTempRange->setText(days[0].mTempLow+"℃"+"/"+days[0].mTempHigh+"℃");
    //解析天气类型
    ui->labelWeatherType->setText(days[0].mWeathType);
    ui->labelWeather->setPixmap(mTypeMap[days[0].mWeathType]);
    //解析感冒指数
    ui->labelGanmao->setText(days[0].mTips);
    //解析风向和风速
    ui->labelFXType->setText(days[0].mFx);
    ui->labelFXType_2->setText(days[0].mFl);
    //解析pm2.5
    ui->labelPM25Data->setText(days[0].mPm25);
    //解析湿度
    ui->labelShiduData->setText(days[0].mHu);
    //解析空气质量
    ui->labelAirData->setText(days[0].mAirq);

    mWeekList[0]->setText("昨天");
    mWeekList[1]->setText("今天");
    mWeekList[2]->setText("明天");

    for(int i = 3; i < 6; i++){
        mWeekList[i]->setText(days[i].mWeek);
    }
    for(int i = 0; i < 6; i++){
        QStringList dayList = days[i].mDate.split("-");
        mDateList[i]->setText(dayList.at(1)+"-"+dayList.at(2));

        int index = days[i].mWeathType.indexOf("转");
        if(index != -1){
            pixmap = mTypeMap[days[i].mWeathType.left(index)];
        }else{
            pixmap = mTypeMap[days[i].mWeathType];
        }
        pixmap = pixmap.scaled(mIconList[i]->size(), Qt::KeepAspectRatio,Qt::SmoothTransformation);
        mIconList[i]->setMaximumHeight(900);
        mIconList[i]->setMaximumWidth(600);
        mIconList[i]->setPixmap(pixmap);
        mWeaTypeList[i]->setText(days[i].mWeathType);

        QString airQ = days[i].mAirq;
        mAirqList[i]->setText(airQ);
        if(airQ == "优"){
            mAirqList[i]->setStyleSheet("background-color:rgb(68, 255, 0); border-radius:7px");
        }
        if(airQ == "良"){
            mAirqList[i]->setStyleSheet("background-color:rgb(195, 255, 0); border-radius:7px");
        }
        if(airQ == "轻度污染"){
            mAirqList[i]->setStyleSheet("background-color:rgb(255, 150, 0); border-radius:7px");
        }
        if(airQ == "中度污染"){
            mAirqList[i]->setStyleSheet("background-color:rgb(255, 120, 0); border-radius:7px");
        }
        if(airQ == "重度污染"){
            mAirqList[i]->setStyleSheet("background-color:rgb(255, 80, 0); border-radius:7px");
        }
        if(airQ == "严重污染"){
            mAirqList[i]->setStyleSheet("background-color:rgb(255, 40, 0); border-radius:7px");
        }

        mFxList[i]->setText(days[i].mFx);

        index = days[i].mFl.indexOf("转");
        if(index != -1){
            mFlList[i]->setText(days[i].mFl.left(index));
        }else{
            mFlList[i]->setText(days[i].mFl);
        }
    }
    update();
}

void Widget::drawTempLineHigh()
{
    //qDebug() << "drawTempLineHigh";
    QPainter painter(ui->widget0404);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::yellow);
    painter.setPen(Qt::yellow);

    int ave;
    int sum = 0;
    int offSet = 0;
    int middle = ui->widget0404->height() / 2;

    // 计算平均高温
    for (int i = 0; i < 6; i++) {
        sum += days[i].mTempHigh.toInt();
    }
    ave = sum / 6;

    //添加一个向下偏移量（单位：像素）
    int downShift = 10;

    QPoint points[6];
    for (int i = 0; i < 6; i++) {
        points[i].setX(mAirqList[i]->x() + mAirqList[i]->width() / 2);
        offSet = (days[i].mTempHigh.toInt() - ave) * 3;
        points[i].setY(middle - offSet + downShift);  // 👈 这里加偏移

        // 画点
        painter.drawEllipse(points[i], 3, 3);
        // 画温度文字
        painter.drawText(points[i].x() - 15, points[i].y() - 15, days[i].mTempHigh + "℃");
    }

    // 画连线
    for (int i = 0; i < 5; i++) {
        painter.drawLine(points[i], points[i + 1]);
    }
}

void Widget::drawTempLineLow()
{
    //qDebug() << "drawTempLineLow";
    QPainter painter(ui->widget0405);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(Qt::green);
    painter.setPen(Qt::green);

    int ave;
    int sum = 0;
    int offSet = 0;
    int middle = ui->widget0405->height() / 2;

    // 计算平均高温
    for (int i = 0; i < 6; i++) {
        sum += days[i].mTempLow.toInt();
    }
    ave = sum / 6;

    //添加一个向下偏移量（单位：像素）
    int downShift = 10;

    QPoint points[6];
    for (int i = 0; i < 6; i++) {
        points[i].setX(mAirqList[i]->x() + mAirqList[i]->width() / 2);
        offSet = (days[i].mTempLow.toInt() - ave) * 3;
        points[i].setY(middle - offSet + downShift);

        // 画点
        painter.drawEllipse(points[i], 3, 3);
        // 画温度文字
        painter.drawText(points[i].x() - 15, points[i].y() - 15, days[i].mTempLow + "℃");
    }

    // 画连线
    for (int i = 0; i < 5; i++) {
        painter.drawLine(points[i], points[i + 1]);
    }
}

void Widget::readHttpRely(QNetworkReply *reply)
{
    int resCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    //qDebug() << resCode;
    if(reply->error() == QNetworkReply::NoError && resCode == 200){
        //大多数服务器返回的字符编码格式是utf-8
        QByteArray data = reply->readAll();
        //qDebug() << "http reply size:" << data.size();
        //paresWeatherJsonData(data);
        paresWeatherJsonDataNew(data);
        //qDebug() << QString::fromUtf8(data);
    }else{
        //QMessageBox::warning(this,"错误","网络请求失败",QMessageBox::Ok);
        QMessageBox mes;
        mes.setWindowTitle("错误");
        mes.setText("网络请求失败");
        mes.setStyleSheet("QPushButton{color:red}");
        mes.setStandardButtons(QMessageBox::Ok);
        mes.exec();
    }

}

void Widget::on_pushButton_clicked()
{
    QString cityNameFromUser = ui->lineEditCity->text();
    QString cityCode = cityCodeUtils.getCityCodeFromName(cityNameFromUser);

    if(cityCode != NULL) {
        QString newUrl = "http://gfeljm.tianqiapi.com/api?unescape=1&version=v91&appid=15166155&appsecret=32TIoiIC&cityid=" + cityCode;
        manager->get(QNetworkRequest(QUrl(newUrl)));
    } else {
        QMessageBox mse;
        mse.setWindowTitle("错误");
        mse.setText("请输入正确的城市名称");
        mse.setStyleSheet("QPushButton{color:black}");
        mse.setStandardButtons(QMessageBox::Ok);
        mse.exec();
    }
}
