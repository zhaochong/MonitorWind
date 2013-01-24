/****************************************************************************
**
** Copyright (C) 2005-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#include <QtGui>
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QFont>
#include <QTime>
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

#include "MonitorWind.h"

MonitorWind::MonitorWind()
{
    setGeometry(0,0,800,600);
    
    waveShow = new WaveShow(this);
    timer = new QTimer(this);
    pthread = new MonitorThread(this);
    pDatathread = new DataTransferThread(this);

    psetupWind = new SetupWindow(this,0);
    psetupWind->setModal(true);
    psetupWind->setWindowModality(Qt::WindowModal);
    psetupWind->exec();

    MonitorWindsetUp();

    setCurrentTime(2011, 5, 24, 9, 56, 11);

    heartRate = createHeartGroup();
    bodyTemp = createBodyTempGroup();
    pulseRate = createPulseRateGroup();
    breathRate = createBreathRateGroup();
    bloodPress = createBloodPressGroup();

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(heartRate,0,3,2,1);
    mainLayout->addWidget(bodyTemp,2,3);
    mainLayout->addWidget(pulseRate,3,3,2,1);
    mainLayout->addWidget(breathRate,5,3);
    mainLayout->addWidget(bloodPress,6,3,3,1);
    mainLayout->addWidget(waveShow,0,0,10,3);
    mainLayout->setMargin(4);
    mainLayout->setVerticalSpacing(0);
    mainLayout->setHorizontalSpacing(2);
    setLayout(mainLayout); 
    
    QPalette pal;
    pal.setColor(QPalette::Background,QColor(0,0,0));	
    setPalette(pal); 

    connect(timer,SIGNAL(timeout()), this, SLOT(activeWaveShow()));

    connect(pDatathread, SIGNAL(sDataTransfer(int)),this, SLOT(dataTransfer(int)),Qt::BlockingQueuedConnection); 
    connect(pthread, SIGNAL(sShowWave(char *, int)),this, SLOT(showWave(char *,int)),Qt::BlockingQueuedConnection);
    

    connect(EcgBtn, SIGNAL(clicked()), this, SLOT(ecgAnalyse()));
    /* QueuedConnection DirectConnection BlockingQueuedConnection*/

    N = 0;
    /* for(int i=0; i<32;i++)
          for(int j=0;j<4;j++)
            buffer[j+i*4] =(32*N+i)*10; */
    memset(buffer,0,128*sizeof(short));

    pdata=data;
    ptransfer = data;
    cpyenabled =true;
    datanum = 0;

    setWindowFlags(Qt::FramelessWindowHint);

    pDatathread->start();
    pthread->start();

    timer->start(101);
}


void MonitorWind::paintEvent(QPaintEvent * /* event */)
{
    
    QPainter painter(this);

    setAutoFillBackground(true);
}

GroupContainer* MonitorWind::createHeartGroup()
{
    QIcon icon;
    QPalette pal,timepal;
    pal.setColor(QPalette::WindowText,QColor(255,255,255));
    GroupContainer *pHeart = new GroupContainer(this);
    pHeart->setFixedSize(190,120);

    timepal.setColor(QPalette::Background,QColor(0,0,255));
    timepal.setColor(QPalette::WindowText,QColor(255,255,255));
    timeLab = new QLabel(tr("2011-07-12 09:15:25"));
    timeLab->setAttribute(Qt::WA_TranslucentBackground,false);
    timeLab->setAutoFillBackground(true);

    timeLab->setFont(QFont("simsun",14,QFont::Bold));
    timeLab->setAlignment(Qt::AlignCenter);
    timeLab->setPalette(timepal);


    heartLab = new QLabel(tr("心率")); /*HeartRate*/
    heartLab->setFont(QFont("wenquanyi",14,QFont::Bold));
    heartLab->setAlignment(Qt::AlignTop);
    heartLab->setWordWrap(true);
    heartLab->setPalette(pal);

    pal.setColor(QPalette::WindowText,QColor(0,255,0));
    heartUnitLab = new QLabel(tr("(bpm)"));
    heartUnitLab->setAlignment(Qt::AlignBottom);
    heartUnitLab->setFont(QFont("simsun",10,QFont::Bold));
    heartUnitLab->setPalette(pal);

    heartNum = new QLabel(tr("172"));
    heartNum ->setAlignment(Qt::AlignCenter);
    heartNum->setFont(QFont("arialnb",54,QFont::Bold));
    heartNum->setPalette(pal);

    pal.setColor(QPalette::WindowText,QColor(0,255,0));
    heartMark = new QLabel(tr(""));//X1
    heartMark ->setAlignment(Qt::AlignCenter);
    heartMark->setFont(QFont("Times",14,QFont::Bold));
    heartMark->setPalette(pal);


    heartIcon = new QLabel;

    heartIcon->setAlignment(Qt::AlignCenter);
    heartIcon->setScaledContents(true);

    QImage image("/usr/testapp/images/xinzang.png"); /*/home/zhigang/make/My Design/MonitorWind/*/ /*  */
    if (!image.isNull())
        icon.addPixmap(QPixmap::fromImage(image), QIcon::Normal, QIcon::On);
    QPixmap pixmap = icon.pixmap(QSize(32,32), QIcon::Normal, QIcon::On);
    
    heartIcon->setPixmap(pixmap);
    heartIcon->setEnabled(true);

    QGridLayout *gridbox = new QGridLayout;
    gridbox->addWidget(timeLab,0,0,1,4);
    gridbox->addWidget(heartLab,1,0);
    gridbox->addWidget(heartNum ,1,1,5,3);
    gridbox->addWidget(heartUnitLab,2,0);
    gridbox->addWidget(heartIcon,3,0,3,1);
    gridbox->addWidget(heartMark,6,2);
    gridbox->setMargin(2);
    gridbox->setVerticalSpacing(1);
    gridbox->setHorizontalSpacing(0);
    pHeart->setLayout(gridbox);
    return pHeart;
}


GroupContainer* MonitorWind::createBodyTempGroup()
{
    QPalette pal;
    pal.setColor(QPalette::WindowText,QColor(255,255,255));
    GroupContainer *pBodyTemp = new GroupContainer(this);/*QGroupBox(tr("2"),this); */
    pBodyTemp->setFixedSize(190,60);
    pBodyTemp->setPalette(pal);

    tempLab = new QLabel(tr("体温")); /*BodyTemp*/
    tempLab->setFont(QFont("wenquanyi",14,QFont::Bold));
    tempLab->setAlignment(Qt::AlignVCenter);
    tempLab->setPalette(pal);

    pal.setColor(QPalette::WindowText,QColor(0,255,255));
    tempUnitLab = new QLabel(tr("'C"));
    tempUnitLab->setFont(QFont("simsun",10,QFont::Bold));
    tempUnitLab->setAlignment(Qt::AlignCenter);
    tempUnitLab->setPalette(pal);

    tempNum = new QLabel(tr("37.5"));
    tempNum->setAlignment(Qt::AlignCenter);
    tempNum->setFont(QFont("arialnb",30,QFont::Bold));
    tempNum->setPalette(pal);

    QGridLayout *gridbox = new QGridLayout;
    gridbox->addWidget(tempLab,0,0);
    gridbox->addWidget(tempUnitLab,2,0);
    gridbox->addWidget(tempNum ,1,1,3,1);
    gridbox->setMargin(2);
    gridbox->setVerticalSpacing(1);
    gridbox->setHorizontalSpacing(0);
    pBodyTemp->setLayout(gridbox);
    return pBodyTemp;

}


GroupContainer* MonitorWind::createPulseRateGroup()
{
    QPalette pal;
    pal.setColor(QPalette::WindowText,QColor(255,255,255));
    GroupContainer *pPulse = new GroupContainer(this);
    pPulse->setFixedSize(190,120);
    pPulse->setPalette(pal);

    pulseLab =  new QLabel(tr("脉搏")); /*Pulse*/
    pulseLab->setFont(QFont("wenquanyi",14,QFont::Bold));

    pulseLab->setPalette(pal);

    pulseO2Lab = new QLabel(tr("脉搏氧"));/*Pulse-O2*/
    pulseO2Lab->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    pulseO2Lab->setFont(QFont("wenquanyi",14,QFont::Bold));
    pulseO2Lab->setPalette(pal);

    pal.setColor(QPalette::WindowText,QColor(255,0,0));

    pulseO2Num = new QLabel(tr("198"));
    pulseO2Num->setAlignment(Qt::AlignCenter); //Qt::AlignVCenter | Qt::AlignRight
    pulseO2Num->setFont(QFont("arialnb",54,QFont::Bold));
    pulseO2Num->setPalette(pal);

    pal.setColor(QPalette::WindowText,QColor(255,100,0));
    pulseNum = new QLabel(tr("63"));
    pulseNum->setAlignment(Qt::AlignCenter);
    pulseNum->setFont(QFont("Times",30,QFont::Bold));
    pulseNum->setPalette(pal);

    QGridLayout *gridbox = new QGridLayout;
    gridbox->addWidget(pulseO2Lab,0,0,1,3);
    gridbox->addWidget(pulseO2Num,0,1,5,2);
    gridbox->addWidget(pulseLab ,4,0);
    gridbox->addWidget(pulseNum,5,0,3,2);
    gridbox->setMargin(2);
    gridbox->setVerticalSpacing(1);
    gridbox->setHorizontalSpacing(0);
    pPulse->setLayout(gridbox);
    return pPulse;
} 


GroupContainer* MonitorWind::createBreathRateGroup()
{
    QIcon icon;
    QPalette pal;
    pal.setColor(QPalette::WindowText,QColor(255,255,255));
    GroupContainer *pBreath = new GroupContainer(this);
    pBreath-> setFixedSize(190,60);
    pBreath->setPalette(pal);

    breathLab = new QLabel(tr("呼吸率"));/*Breath*/
    breathLab->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    breathLab->setFont(QFont("wenquanyi",14,QFont::Bold));
    breathLab->setPalette(pal);

    pal.setColor(QPalette::WindowText,QColor(255,0,255));
    breathUnitLab = new QLabel(tr("bpm"));
    breathUnitLab->setAlignment(Qt::AlignCenter);
    breathUnitLab->setFont(QFont("simsun",10,QFont::Bold));
    breathUnitLab->setPalette(pal);

    breathNum = new QLabel(tr("144"));
    breathNum->setAlignment(Qt::AlignCenter);
    breathNum->setFont(QFont("arialnb",30,QFont::Bold));
    breathNum->setPalette(pal);

    breathMark = new QLabel(tr(""));//X1
    breathMark->setFont(QFont("Times",14,QFont::Bold));
    breathMark->setAlignment(Qt::AlignCenter);
    breathMark->setPalette(pal);

    breathIcon = new QLabel;

    breathIcon->setAlignment(Qt::AlignCenter);
    breathIcon->setScaledContents(true);

    QImage image("/usr/testapp/images/fei.png");
    if (!image.isNull())
        icon.addPixmap(QPixmap::fromImage(image), QIcon::Normal, QIcon::On);
    QPixmap pixmap = icon.pixmap(QSize(32,32), QIcon::Normal, QIcon::On);
    
    breathIcon->setPixmap(pixmap);
    breathIcon->setEnabled(true);
    // breathIcon->setPalette(pal);

    QGridLayout *gridbox = new QGridLayout;
    gridbox->addWidget(breathLab,0,0);
    gridbox->addWidget(breathUnitLab,0,1);
    gridbox->addWidget(breathNum ,0,2,3,2);
    gridbox->addWidget(breathIcon,1,0,3,1);
    gridbox->addWidget(breathMark, 3,3);
    gridbox->setMargin(2);
    gridbox->setVerticalSpacing(1);
    gridbox->setHorizontalSpacing(0);
    pBreath->setLayout(gridbox);
    return pBreath;

} 


GroupContainer* MonitorWind::createBloodPressGroup()
{
    QPalette pal,buttonpal;
    pal.setColor(QPalette::WindowText,QColor(255,255,255));
    GroupContainer *pBlood = new GroupContainer(this);
    pBlood-> setFixedSize(190,230);
    pBlood->setPalette(pal);

    bloodLab = new QLabel(tr("血压"));/*BloodP*/
    bloodLab->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    bloodLab->setFont(QFont("wenquanyi",14,QFont::Bold));
    bloodLab->setPalette(pal);

    bloodAvgLab = new QLabel(tr("平均压")); /*AverP*/
    bloodAvgLab->setFont(QFont("wenquanyi",14,QFont::Bold));
    //bloodAvgLab->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
    bloodAvgLab->setPalette(pal);

    bloodSystLab = new QLabel(tr("舒张压")); /*SystP*/
    bloodSystLab->setFont(QFont("wenquanyi",14,QFont::Bold));
    bloodSystLab->setAlignment(Qt::AlignCenter);
    bloodSystLab->setPalette(pal);

    bloodDiastLab = new QLabel(tr("收缩压")); /*DiastP*/
    bloodDiastLab->setFont(QFont("wenquanyi",14,QFont::Bold));
    bloodDiastLab->setAlignment(Qt::AlignCenter);
    bloodDiastLab->setPalette(pal);

    bloodExam = new QLabel(tr("KPa 160/106(125)"));
    bloodExam->setAlignment(Qt::AlignCenter);
    bloodExam->setFont(QFont("simsun",12,QFont::Bold));
    bloodExam->setPalette(pal);


    EcgBtn = new QPushButton(tr("心电分析"));
    buttonpal = EcgBtn->palette();
    buttonpal.setColor(QPalette::ButtonText,QColor(255,255,255));
    buttonpal.setColor(QPalette::Button,QColor(0,255,150));
    EcgBtn->setFont(QFont("wenquanyi",16,QFont::Bold));
    EcgBtn->setAutoDefault(true);
    EcgBtn->setPalette(buttonpal);

    /*    MonitorSetBtn = new QPushButton(tr("功能设置"));
     buttonpal = MonitorSetBtn->palette();
     buttonpal.setColor(QPalette::ButtonText,QColor(255,255,255));
     buttonpal.setColor(QPalette::Button,QColor(0,255,150));
     MonitorSetBtn->setFont(QFont("wenquanyi",16,QFont::Bold));
     MonitorSetBtn->setAutoDefault(true);
     MonitorSetBtn->setPalette(buttonpal); */

    pal.setColor(QPalette::WindowText,QColor(155,255,155));
    bloodUnitLab = new QLabel(tr("mmHg"));
    bloodUnitLab->setAlignment(Qt::AlignCenter);
    bloodUnitLab->setFont(QFont("simsun",10,QFont::Bold));
    bloodUnitLab->setPalette(pal);

    bloodAvgNum = new QLabel(tr("94"));
    //bloodAvgNum->setAlignment(Qt::AlignCenter);
    bloodAvgNum->setFont(QFont("arialnb",30,QFont::Bold));
    //bloodAvgNum->setAlignment(Qt::AlignVCenter | Qt::AlignLeft);
    bloodAvgNum->setPalette(pal);

    bloodSystNum = new QLabel(tr("120"));
    bloodSystNum->setAlignment(Qt::AlignCenter);
    bloodSystNum->setFont(QFont("arialnb",30,QFont::Bold));
    bloodSystNum->setPalette(pal);

    bloodDiastNum = new QLabel(tr("85"));
    bloodDiastNum->setAlignment(Qt::AlignCenter);
    bloodDiastNum->setFont(QFont("arialnb",30,QFont::Bold));
    bloodDiastNum->setPalette(pal);


    QGridLayout *gridbox = new QGridLayout;
    gridbox->addWidget(bloodLab,0,0);
    gridbox->addWidget(bloodUnitLab,0,1);
    gridbox->addWidget(bloodSystLab ,0,2,1,2);
    gridbox->addWidget(bloodAvgLab ,3,1,1,2);
    gridbox->addWidget(bloodSystNum ,1,2,3,2);
    gridbox->addWidget(bloodAvgNum ,4,1,3,2);
    gridbox->addWidget(bloodDiastLab ,5,2,1,2);
    gridbox->addWidget(bloodDiastNum ,6,2,3,2);
    gridbox->addWidget(bloodExam ,9,0,1,4);
    // gridbox->addWidget(MonitorSetBtn ,10,0,2,2);
    gridbox->addWidget(EcgBtn ,10,0,2,4);
    //gridbox->addWidget(bloodMark2 ,11,1,1,2);

    gridbox->setMargin(2);
    gridbox->setVerticalSpacing(1);
    gridbox->setHorizontalSpacing(0);
    pBlood->setLayout(gridbox);
    return pBlood;
}  

void MonitorWind::showWave(char *pBuffer,int Num)
{

    /*printf("dataNum=%d..................\n",Num);
   for(int j=0;j<Num;j++)
        printf("data=%02x\n",pBuffer[j]); */
    int i,k=0;

    if(cpyenabled)
    {

        for(i=0;i<Num;i +=3)
        {
            buffer[k] = (0x00ff & pBuffer[i]) | ((pBuffer[i+1] & 0x000f)<<8);
            // buffer[k] <<=1;
            buffer[k+1] = ((0x00f0 & pBuffer[i+1])<<4) | (0x00ff & pBuffer[i+2]);
            // buffer[k+1] <<=1;
            k +=2;
        }

        waveShow->WaveDisplay(buffer, k*2);

        memcpy(pdata,buffer,256);
        pdata +=128;
        datanum +=128;

        if(pdata>=(data+2000000))
        {
            //pdata =data;
            cpyenabled =false;
        }
    }
}

void MonitorWind::dataTransfer(int devNum)
{    
    int count;

    if((ptransfer+32)<pdata)
    {
        count = write(devNum,ptransfer,64);
        ptransfer +=32;
    }
}

void MonitorWind::MonitorWindsetUp()
{
    int dataRang;
    dataRang = 1<<(adbit);
    waveShow->setGain(gain);
    waveShow->setFreqRatio(scan);
    waveShow->setWaveChannel(channel);
    waveShow->setDataRange(dataRang);
}

//************************************************
void MonitorWind::ecgprocess()
{
    short *p_data = data;

    int i;
    for(i=0;i<6000;i++)
    {
        sig[i]=p_data[i*2];
    }
    int j,k;
    for(j=2;j<5999;j++)
    {
        diff[j]=sig[j+1]-sig[j-1];
    }
    int r1;
    for (r1=0;r1<6000;r1++)
    {
        R1[r1]=0;
    }
    int MaxSlope,MaxData,MaxDataGate,MaxSlopeGate;
    MaxSlope=0;
    MaxData=0;
    for(k=2;k<500;k++)
    {
        if (sig[k]>=MaxData)
        {
            MaxData=sig[k];
        }
    }
    int k1;
    for(k1=2;k1<500;k1++)
    {
        if(diff[k1]>=MaxSlope)
        {
            MaxSlope=diff[k1];
        }
    }
    MaxDataGate=MaxData*0.6;
    MaxSlopeGate=MaxSlope*0.5;
    int mdata,mm;
    mdata=0;mm=0;
    int k2,k3,k4;
    for(k2=2;k2<6000-20;k2++)
    {
        if((diff[k2]>MaxSlopeGate)&&(sig[k2]>MaxDataGate))
        {
            mdata=sig[i];
            mm=k2;
            for(k3=k2;k3<k2+20;k3++)
            {
                if (sig[k3]>mdata)
                {
                    mdata=sig[k3];
                    mm=k3;
                }
            }
            for (k4=k2;k4>k2-5;k4--)
            {
                if (sig[k4]>mdata)
                {
                    mdata=sig[k4];
                    mm=k4;
                }
            }
            R1[mm]=1;
        }
    }
    int ii;
    for(ii=1;ii<2999;ii++)
    {
        a[ii]=sig[ii];
    }
    int r2;
    for(r2=1;r2<2999;r2++)
    {
        if (R1[r2]==1)
        {
            R2[r2]=1;
        }
    }
    int r3,r4,rm,rm1;
    for(r3=6;r3<2993;r3++)
    {
        if(R2[r3]==1)
        {
            rm=a[r3];
            rm1=r3;
            for(r4=r3-10;r4<r3+10;r4++)
            {
                if (a[r4]<rm)
                {
                    rm=a[r4];
                    rm1=r4;
                }
            }
            if (rm1!=r3)
            {
                R2[r3]=0;
                R2[rm1]=1;
            }
        }
    }
    int min1,m1,min2,m2;
    int q1,q2,q3;
    int qq1;
    qq1=1;
    for(q1=20;q1<2999-20;q1++)
    {
        if (R2[q1]==1)
        {
            min1=a[q1];
            m1=q1;
            for(q2=q1-20;q2<q1-1;q2++)
            {
                if (a[q2]>min1)
                {
                    min1=a[q2];
                    m1=q2;
                }
            }
            Q[qq1]=m1;
            min2=a[q1];
            m2=q1;
            for(q3=q1+1;q3<q1+20;q3++)
            {
                if (a[q3]>min2)
                {
                    min2=a[q3];
                    m2=q3;
                }
            }
            S[qq1]=m2;
            qq1=qq1+1;
        }
    }
    //去掉QRS波，为P波检测作好准备
    int a1n;
    for(a1n=1;a1n<3000;a1n++)
    {
        aa1[a1n]=a[a1n];
    }
    int p1,p2,p3,p4,p5,p6,p7,p8,p9,p10,p11,p12,p13,p14,p15,p16,p17,p18,p19/*,p20*/;
    int pp1;
    pp1=1;
    for(p1=1;p1<2999;p1++)
    {
        if (R2[p1]==1)
        {
            for(p2=Q[pp1];p2<S[pp1];p2++)
            {
                aa1[p2]=400;
            }
            pp1=pp1+1;
        }
    }
    //8点平滑滤波
    int a2n;
    for(a2n=1;a2n<3000;a2n++)
    {
        a2[a2n]=a[a2n];
    }
    for(p3=8;p3<3000;p3++)
    {
        a2[p3]=(aa1[p3-7]+aa1[p3-6]+aa1[p3-5]+aa1[p3-4]+aa1[p3-3]+aa1[p3-2]+aa1[p3-1]+aa1[p3])/8;
    }
    //求差分
    for(p4=2;p4<2999;p4++)
    {
        diff1[p4]=a2[p4+1]-a2[p4-1];
    }
    //双门限法判定P波位置
    int MaxSlope1,MaxData1,MaxSlopeGate1,MaxDataGate1;
    MaxData1=120;MaxSlope1=0;
    for(p5=20;p5<250;p5++)
    {
        if (a2[p5]<MaxData1)
        {
            MaxData1=a2[p5];
        }
        if (diff1[p5]<MaxSlope1)
        {
            MaxSlope1=diff1[p5];
        }
    }
    MaxDataGate1=MaxData1*1.08;
    MaxSlopeGate1=-1;
    int Pdata,pp;
    Pdata=0;pp=0;
    for (p6=40;p6<3000-40;p6++)
    {
        if ((diff1[p6]<MaxSlopeGate1)&&(a2[p6]<MaxDataGate1))
        {
            Pdata=a2[p6];
            pp=p6;
            for (p7=p6;p7<p6+6;p7++)
            {
                if (a2[p7]<Pdata)
                {
                    Pdata=a2[p7];
                    pp=p7;
                }
            }
            for (p8=p6;p8>p6-12;p8--)
            {
                if (a2[p8]<Pdata)
                {
                    Pdata=a2[p8];
                    pp=p8;
                }
            }
            P[pp]=1;
        }
    }
    int PPmax,PPnum;
    PPmax=0;PPnum=0;
    for (p9=12;p9<3000-12;p9++)
    {
        if (P[p9]==1)
        {
            PPmax=a2[p9];
            PPnum=p9;
            for (p10=p9-6;p10<p9+6;p10++)
            {
                if (a2[p10]<PPmax)
                {
                    PPmax=a2[p10];
                    PPnum=p10;
                }
            }
            if ((PPnum>p9)||(PPnum<p9))
            {
                P[p9]=0;
                P[PPnum]=1;
            }
        }
    }
    for (p11=101;p11<3000-150;p11++)
    {
        if (P[p11]==1)
        {
            PPmax=a2[p11];
            PPnum=p11;
            for (p12=p11-100;p12<p11+150;p12++)
            {
                if (a2[p12]<PPmax)
                {
                    PPmax=a2[p12];
                    PPnum=p12;
                }
            }
            if ((PPnum>p11)||(PPnum<p11))
            {
                P[p11]=0;
                P[PPnum]=1;
            }
        }
    }
    for (p13=61;p13<3000-60;p13++)
    {
        if (P[p13]==1)
        {
            PPmax=a2[p13];
            PPnum=p13;
            for (p14=p13-100;p14<p13+150;p14++)
            {
                if (a2[p14]<PPmax)
                {
                    PPmax=a2[p14];
                    PPnum=p14;
                }
            }
            if ((PPnum>p13)||(PPnum<p13))
            {
                P[p13]=0;
                P[PPnum]=1;
            }
        }
    }
    for (p15=10;p15<3000-10;p15++)
    {
        if (P[p15]==1)
        {
            PPmax=a[p15];
            PPnum=p15;
            for (p16=p15-10;p16<p15+10;p16++)
            {
                if (a[p16]<PPmax)
                {
                    PPmax=a2[p16];
                    PPnum=p16;
                }
            }
            if ((PPnum>p15)||(PPnum<p15))
            {
                P[p15]=0;
                P[PPnum]=1;
            }
        }
    }
    //P波段标记
    int pwave1,pw1,pwave2,pw2,pcount;
    pwave1=0;pw1=0;pwave2=0;pw2=0;pcount=1;
    for(p17=26;p17<3000-24;p17++)
    {
        if (P[p17]==1)
        {
            pwave1=a[p17];
            pw1=p17;
            for(p18=p17-24;p18<p17-1;p18++)
            {
                if (a[p18]>pwave1)
                {
                    pwave1=a[p18];
                    pw1=p18;
                }
            }
            pwave2=a[p17];
            pw2=p17;
            for(p19=p17+1;p19<p17+24;p19++)
            {
                if (a[p19]>pwave2)
                {
                    pwave2=a[p19];
                    pw2=p19;
                }
            }
            Pwave[pcount]=pw1;
            Pwave[pcount+1]=pw2;
            pcount=pcount+2;
        }
    }
    //再去掉P波，为T波检测作好准备
    int a3n;
    for(a3n=1;a3n<3000;a3n++)
    {
        aa3[a3n]=a2[a3n];
    }
    int t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,/*t13,t14,t15,t16,*/t17,t18,t19;
    int tt1;
    tt1=1;
    for(t1=1;t1<2999;t1++)
    {
        if (P[t1]==1)
        {
            for(t2=Pwave[tt1];t2<Pwave[tt1+1];t2++)
            {
                aa3[t2]=400;
            }
            tt1=tt1+2;
        }
    }
    //T波检测
    int MaxSlope2,MaxData2,MaxSlopeGate2,MaxDataGate2;
    MaxData2=400;MaxSlope2=0;
    for(t5=2;t5<250;t5++)
    {
        if (aa3[t5]<MaxData2)
        {
            MaxData2=aa3[t5];
        }
        if (diff1[t5]<MaxSlope2)
        {
            MaxSlope2=diff1[t5];
        }
    }
    MaxDataGate2=MaxData2*1.1;
    int Tdata,tt;
    Tdata=0;tt=0;
    for (t6=40;t6<3000-6;t6++)
    {
        if ((diff1[t6]<0)&&(aa3[t6]<MaxDataGate2))
        {
            Tdata=aa3[t6];
            tt=t6;
            for (t7=t6;t7<t6+6;t7++)
            {
                if (aa3[t7]<Tdata)
                {
                    Tdata=aa3[t7];
                    tt=t7;
                }
            }
            for (t8=t6;t8>t6-6;t8--)
            {
                if (aa3[t8]<Tdata)
                {
                    Tdata=aa3[t8];
                    tt=t8;
                }
            }
            T[tt]=1;
        }
    }
    for(t3=51;t3<3000-50;t3++)
    {
        if (T[t3]==1)
        {
            for(t4=t3-50;t4<t3-1;t4++)
            {
                if ((T[t4]==1)&&(aa3[t4]<aa3[t3]))
                {
                    T[t3]=0;
                }
            }
            for(t9=t3+1;t9<t3+50;t9++)
            {
                if ((T[t9]==1)&&(aa3[t9]<aa3[t3]))
                {
                    T[t3]=0;
                }
            }
        }
    }
    for(t10=161;t10<3000-160;t10++)
    {
        if (T[t10]==1)
        {
            for(t11=t10-160;t11<t10-1;t11++)
            {
                if ((T[t11]==1)&&(aa3[t11]<aa3[t10]))
                {
                    T[t10]=0;
                }
            }
            for(t12=t10+1;t12<t10+160;t12++)
            {
                if ((T[t12]==1)&&(aa3[t12]<aa3[10]))
                {
                    T[t10]=0;
                }
            }
        }
    }
    //T波段标记
    int twave1,tw1,twave2,tw2,tcount;
    twave1=0;tw1=0;twave2=0;tw2=0;tcount=1;
    for(t17=25;t17<3000-24;t17++)
    {
        if (T[t17]==1)
        {
            twave1=a[t17];
            tw1=t17;
            for(t18=t17-24;t18<t17-1;t18++)
            {
                if (a[t18]>twave1)
                {
                    twave1=a[t18];
                    tw1=t18;
                }
            }
            twave2=a[t17];
            tw2=t17;
            for(t19=t17+1;t19<t17+24;t19++)
            {
                if (a[t19]>twave2)
                {
                    twave2=a[t19];
                    tw2=t19;
                }
            }
            Twave[tcount]=tw1;
            Twave[tcount+1]=tw2;
            tcount=tcount+2;
        }
    }

    int RRnum1,RRnum2,Total,Pnum,Qnum,Snum,Tnum;
    int nn1,nn2,nn3/*,nn4,nn5,nn6,nn7,nn8,nn9,nn10*/;
    RRnum1=1;RRnum2=1;Total=1;Pnum=1;Qnum=1;Snum=1;Tnum=1;
    for(nn1=1;nn1<2999;nn1++)
    {
        if (R2[nn1]==1)
        {
            Rwave[RRnum1]=nn1;
            RRnum1=RRnum1+1;
        }
    }
    for(nn2=1;nn2<99;nn2++)
    {
        if(Rwave[nn2+1]>0)
        {
            RRtime[nn2]=Rwave[nn2+1]-Rwave[nn2];
        }
    }
    if (Pwave[Qnum]>Q[Qnum])
    {
        Qnum=Qnum+1;
        Snum=Snum+1;
        Tnum=Tnum+2;
    }
    for(nn3=1;nn3<99;nn3++)
    {
        if((Pwave[Pnum]>0)&&(Q[Qnum]>0)&&(S[Snum]>0)&&(Twave[Tnum]>0))
        {
            PRtime[Total]=Q[Qnum]-Pwave[Pnum];
            QRStime[Total]=S[Snum]-Q[Qnum];
            STtime[Total]=Twave[Tnum]-S[Snum];
            QTtime[Total]=QRStime[Total]+STtime[Total];
            Pnum=Pnum+2;
            Qnum=Qnum+1;
            Snum=Snum+1;
            Tnum=Tnum+2;
            Total=Total+1;
        }
    }
}


void MonitorWind::ecgAnalyse()
{
    if(cpyenabled){
        cpyenabled = false;
    }else{
        cpyenabled = true;
    }
    /************************************************/
    ecgprocess();
    for(int i=0;i<200;i++){
        printf("Pwave[%d]=%d\n",i,Pwave[i]);
    }
    for(int i=0;i<200;i++){
        printf("Twave[%d]=%d\n",i,Twave[i]);
    }
    for(int i=0;i<200;i++){
        printf("Q[%d]=%d\n",i,Q[i]);
    }
    printf("sizeof(Pwave)=%d\n",sizeof(Pwave));
    printf("sizeof(Twave)=%d\n",sizeof(Twave));
    printf("sizeof(sig)=%d\n",sizeof(sig));
    printf("sizeof(Q)=%d\n",sizeof(Q));
    waveShow->Draw_Ecgwave(sig,1200);
    //waveShow->Draw_R(a,R2);
    waveShow->Draw_QRS(a, Q, S, R2);
    //waveShow->Draw_Pwave(a, Pwave, P);
    //waveShow->Draw_Twave(a, Twave, T);
}

void MonitorWind::activeWaveShow()
{
    QString strtime;

    /*  waveShow->WaveDisplay(buffer, 256);  */
    curDatetime = QDateTime::currentDateTime();
    strtime = curDatetime.toString("yyyy-MM-dd hh:mm:ss");

    timeLab->setText(strtime);

    if(N%6==0)
    {
        heartNum->setNum(N*3);
        tempNum->setNum(N*3);
        pulseO2Num->setNum(N*3);
        pulseNum->setNum(N*3);

        breathNum->setNum(N*3);

        bloodAvgNum->setNum(N*3);
        bloodSystNum->setNum(N*3);
        bloodDiastNum->setNum(N*3);
    }
    /*  for(int i=0; i<32;i++)
          for(int j=0;j<4;j++)
             buffer[j+i*4] =(32*N+i)*10; */
    N++;
    if(N>50)
        N =0;
}

void MonitorWind::setCurrentTime(int year, int mon, int day, int hour, int min, int sec)
{
    time_t  t;
    struct  tm nowtime;
    nowtime.tm_sec = sec;
    nowtime.tm_min = min;
    nowtime.tm_hour = hour;
    nowtime.tm_mday = day;
    nowtime.tm_mon = mon-1;
    nowtime.tm_year = year-1900;
    nowtime.tm_isdst = -1;
    t = mktime(&nowtime);
    stime(&t);
}
