#ifndef MONITORWIND_H
#define MONITORWIND_H

#include <QPixmap>
#include <QWidget>
#include <QBrush>
#include <QPen>
#include <QDateTime>
#include "WaveShow.h"
#include "GroupContainer.h"
#include "MonitorThread.h"
#include "DataTransferThread.h"
#include "SetupWindow.h"

class QGroupBox;
class QLabel;
class QLineEdit;
class QIcon;
class QPushButton;
class SetupWindow;

class MonitorWind : public QWidget
{
    Q_OBJECT

public:
    MonitorWind();
    void MonitorWindsetUp();
    
    GroupContainer *createHeartGroup(); /*QGroupBox*/  
    GroupContainer *createBodyTempGroup();  
    GroupContainer *createPulseRateGroup();  
    GroupContainer *createBreathRateGroup();  
    GroupContainer *createBloodPressGroup();
    GroupContainer *createAnalyseBlock();


    GroupContainer *heartRate;
    GroupContainer *bodyTemp;
    GroupContainer *pulseRate;
    GroupContainer *breathRate;
    GroupContainer *bloodPress;
    GroupContainer *analyseBlock;

    QLabel *timeLab;
    QLabel *heartLab;
    QLabel *heartUnitLab;
    QLabel *heartNum;
    QLabel *heartMark;
    QLabel *heartIcon;

    QLabel *tempLab;
    QLabel *tempUnitLab;
    QLabel *tempNum;

    QLabel *pulseLab;
    QLabel *pulseO2Lab;
    QLabel *pulseNum;
    QLabel *pulseO2Num;

    QLabel *breathLab;
    QLabel *breathUnitLab;
    QLabel *breathNum;
    QLabel *breathMark;
    QLabel *breathIcon;

    QLabel *bloodLab;
    QLabel *bloodUnitLab;
    QLabel *bloodAvgLab;
    QLabel *bloodSystLab;
    QLabel *bloodDiastLab;
    QLabel *bloodAvgNum;
    QLabel *bloodSystNum;
    QLabel *bloodDiastNum;
    QLabel *bloodExam;
    QPushButton *EcgBtn;
    QPushButton *EcgMoveLeftBtn;
    QPushButton *EcgMoveRightBtn;

    QLabel *elapsedTime;
    QLabel *startPoint;
    QLabel *totalNumLab;
    QLabel *foundNumLab;
    QLabel *wrongNumLab;
    QLabel *forgetNumLab;
    QLabel *rightRateLab;
    QLabel *totalNum;
    QLabel *foundNum;
    QLabel *wrongNum;
    QLabel *forgetNum;
    QLabel *rightRate;
    //QPushButton *MonitorSetBtn;
    // QLabel *bloodMark2;

    WaveShow  *waveShow;
    SetupWindow *psetupWind;
    int channel;
    int gain;  
    int scan;  
    int adbit;
    QTimer *timer;
    QDateTime curDatetime;
    short buffer[128];
    short data[2000000];
    short *pdata;
    short *ptransfer;
    bool cpyenabled;
    unsigned long datanum;
    
    MonitorThread *pthread;
    DataTransferThread *pDatathread;
    int N;
    int offset;
    /******************************************************/
    short sig[1000000];
    short sig1[6000];
    short sig2[6000];
    short a[6000];
    short aa1[3000];
    short a2[3000];
    short aa3[3000];
    short diff[10000];
    short diff1[3000];
    short R1[1000000];
    short R2[6000];
    short Q[100];
    short S[100];
    short P[3000];
    short T[3000];
    short Rwave[200];
    short Pwave[200];
    short Twave[200];
    short RRtime[100];
    short QRStime[100];
    short PRtime[100];
    short STtime[100];
    short QTtime[100];
    int rpoint[10000];
    int ann[10000];
    int sub;

protected:
    /*void resizeEvent(QResizeEvent *event);*/
    void paintEvent(QPaintEvent *event);
private slots:
    /* void newScreenshot();
    void saveScreenshot();
    void shootScreen();
    void updateCheckBox(); */
    void dataTransfer(int devNum);
    void showWave(char *pBuffer,int Num); //char
    void setCurrentTime(int year, int mon, int day, int hour, int min, int sec);
    void activeWaveShow();
    void ecgAnalyse();
    void ecgMoveRight();
    void ecgMoveLeft();
    void ecgprocess();
    void getAnn();

private:

    

};

#endif
