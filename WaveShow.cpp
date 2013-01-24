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

#include <QSize>
#include <QPalette>
#include <QtGui>
#include "WaveShow.h"
#include <stdlib.h>

const int dispChanMap[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
const int dispChanPos[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};
const int dispChanGross[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};


WaveShow::WaveShow(QWidget *parent)
{
    setPalette(QPalette(Qt::black));
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_PaintOutsidePaintEvent);
    setAutoFillBackground(true);

    Wavemap = new QPixmap(size());
    Wavemap->fill(Qt::black);

    nWaveChannel = 2;
    nGain = 50;
    for(int i=0;i<MAX_CHANNEL;i++)
        nWaveDCOffset[i] = 1024;

    nWaveChanColor[0] = QPen(QColor(0,255,0));
    nWaveChanColor[1] = QPen(QColor(0,255,255));
    nWaveChanColor[2] = QPen(QColor(255,0,0));
    nWaveChanColor[3] = QPen(QColor(255,0,255));
    nWaveChanColor[4] = QPen(QColor(155,255,155));
    nWaveChanColor[5] = QPen(QColor(255,255,0));
    nWaveChanColor[6] = QPen(QColor(255,63,63));
    nWaveChanColor[7] = QPen(QColor(0,255,255));
    nWaveChanColor[8] = QPen(QColor(255,0,0));
    nWaveChanColor[9] = QPen(QColor(255,255,0));
    nWaveChanColor[10] = QPen(QColor(0,255,255));
    nWaveChanColor[11] = QPen(QColor(63,255,63));

    nScanSpeed = 200 ;
    nPBuffer = 0;
    nMapCount = 0;
    nDataRange = 4096;/*default 12-bit ADC*/
    freqRatio = 200;
    nXratio = int (5000./freqRatio/(TYPICAL_SCAN_SPEED*nScanSpeed/1000.));

    pDispBuffer = new float[600*10*MAX_CHANNEL];
    if(pDispBuffer)
        memset(pDispBuffer, 0, 600*10*MAX_CHANNEL*sizeof(float));
    /* setFrameStyle(NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); */

}


WaveShow::~WaveShow()
{
    if(pDispBuffer) 
        delete[] pDispBuffer;

}


void WaveShow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    Wavemap = new QPixmap(size());
    Wavemap->fill(Qt::black);

}

void WaveShow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setPen(QPen(Qt::white));/*,1,Qt::DashDotLine */
    painter.drawPixmap(QPoint(0,0), *Wavemap);
    painter.drawRect(QRect(0, 0, width()-1, height()-1));


}

long WaveShow::WaveDisplay(short *pDataBuffer, long nDataBytes)
{
    int left,i,j,nSamples;

    left = nPBuffer;
    nSamples = nDataBytes/nWaveChannel/sizeof(short);

    short *pData = pDataBuffer;
    float *pDisp = pDispBuffer+nPBuffer*nWaveChannel;
    for(i=0; i<nSamples; i++)
    {
        for(j=0; j<nWaveChannel; j++)
        {
            pDisp[j] += pData[dispChanMap[j]]; /*进行累加*/

        }
        nMapCount++;
        if(nMapCount>=nXratio)
        {
            for(j=0;j<nWaveChannel;j++)
            {
                pDisp[j]/=nMapCount; /*对累加取平均*/
                pDisp[j]=float((pDisp[j]-nWaveDCOffset[j])*nGain*height()/200/nDataRange*5);/*计算显示坐标*/
            }
            nPBuffer++;
            pDisp +=nWaveChannel;
            if(nPBuffer>=(width())) /*x()+*/
            {
                nPBuffer = 0;
                pDisp = pDispBuffer;
            }
            memset(pDisp, 0, nWaveChannel*sizeof(int));
            nMapCount = 0;
        }
        pData += nWaveChannel;
    }

    if(nPBuffer==left) return 0;

    QPainter *paint = new QPainter(Wavemap);
    //paint->begin(Wavemap);
    paint->setPen(QPen(Qt::black));
    paint->setBrush(QBrush(Qt::black));

    if(nPBuffer>left)
        paint->drawRect(QRect(left, y(), nPBuffer+1-left, height()));
    
    else
    {
        paint->drawRect(QRect(left, y(), width()+1-left, height()));
        paint->drawRect(QRect(0, y(), nPBuffer+1, height()));
    }

    if(pDispBuffer)
    {
        //QPainterPath path;
        // QPen wavePen;
        // wavePen.setWidth(2);

        for(int k=0; k<nWaveChannel; k++)
        {
            //wavePen.setColor(nWaveChanColor[k]);
            nWaveChanColor[k].setWidth(1);
            paint->setPen(nWaveChanColor[k]);

            float *pOffset = pDispBuffer+k+(left-(!!left))*nWaveChannel;/*前一个点的纵坐标*/
            int yPos= (y()+height())*dispChanPos[k]/dispChanGross[nWaveChannel];

            QPoint p1(left-1, yPos-*pOffset);
            paint->drawPoint(p1);
            // path.moveTo(left-1, yPos-*pOffset);
            pOffset +=(!!left)*nWaveChannel;/*当前点的纵坐标*/

            if(nPBuffer>left)
                for(i=left; i<nPBuffer;i++)
                {
                    QPoint p2(i,yPos-*pOffset);
                    paint->drawLine(p1,p2);
                    p1=p2;
                    //path.lineTo(i,yPos-*pOffset);
                    //painter->drawPath(path);
                    pOffset += nWaveChannel;
                }
            else
            {
                for(i=left; i<(width());i++)
                {
                    QPoint p2(i,yPos-*pOffset);
                    paint->drawLine(p1,p2);
                    p1=p2;
                    //path.lineTo(i,yPos-*pOffset);
                    //painter->drawPath(path);
                    pOffset += nWaveChannel;
                }
                pOffset = pDispBuffer+k;
                QPoint p1(-1, yPos-*pOffset);
                paint->drawPoint(p1);
                //  path.moveTo(-1,yPos-*pOffset);

                for(i=0; i<nPBuffer;i++)
                {
                    QPoint p2(i,yPos-*pOffset);
                    paint->drawLine(p1,p2);
                    p1=p2;
                    //path.lineTo(i,yPos-*pOffset);
                    //painter->drawPath(path);
                    pOffset += nWaveChannel;
                }
            }

        }
    }
    if(nPBuffer>=(x()+width()))
        nPBuffer -=(x()+width());
    paint->end();
    update();
    return 0;
}


void WaveShow::Draw_Ecgwave(short *sig,int Num)
{
    QPainter *paint = new QPainter(Wavemap);
    paint->setPen(QPen(Qt::black));
    paint->setBrush(QBrush(Qt::black));

    paint->drawRect(QRect(0, y(), width(), height()));

    int yPos= (y()+height())*dispChanPos[0]/dispChanGross[1];
    nWaveChanColor[0].setWidth(1);
    paint->setPen(nWaveChanColor[0]);


    int i;
    float offset;

    QPoint p1(0, yPos-float((sig[0]-nWaveDCOffset[0])*nGain*height()/100/nDataRange*15/2));
    paint->drawPoint(p1);
    for(i=1; i<Num;i++)
    {
        offset = float(((float)(sig[i])-nWaveDCOffset[0])*nGain*height()/100/nDataRange*15/2);
        QPoint p2(i,yPos-offset);
        paint->drawLine(p1,p2);
        p1=p2;
    }

    paint->end();
    update();
}


void WaveShow::Draw_R(short *a,short *R2)
{
    int i;
    for(i=2;i<249;i++)
    {
        if (R2[i]==1)
        {
            draw_square(i,a[i],3,0);
        }
    }
}

void WaveShow::draw_square(int x, short t, int l,int k)
{
    QPainter *paint = new QPainter(Wavemap);
    nWaveChanColor[k].setWidth(2);
    paint->setPen(nWaveChanColor[k]);

    int yPos= (y()+height())*dispChanPos[0]/dispChanGross[1];
    float offset = float((t-nWaveDCOffset[0])*nGain*height()/100/nDataRange*15/2);

    paint->drawLine(QPoint(x-l,yPos-offset+l),QPoint(x-l,yPos-offset-l));
    paint->drawLine(QPoint(x-l,yPos-offset-l),QPoint(x+l,yPos-offset-l));
    paint->drawLine(QPoint(x+l,yPos-offset-l),QPoint(x+l,yPos-offset+l));
    paint->drawLine(QPoint(x+l,yPos-offset+l),QPoint(x-l,yPos-offset+l));

    paint->end();
    update();
}

void WaveShow::Draw_QRS(short *a, short *Q, short *S,short *R2)
{

    QPainter *paint = new QPainter(Wavemap);
    nWaveChanColor[5].setWidth(1);
    paint->setPen(nWaveChanColor[5]);

    int yPos= (y()+height())*dispChanPos[0]/dispChanGross[1];


    int QRS,k;
    float offset;
    QRS=1;
    for(k=1;k<249;k++)
    {
        if(R2[k]==1)
        {
            offset = float((a[k]-nWaveDCOffset[0])*nGain*height()/100/nDataRange*15/2);
            paint->drawLine(QPoint(Q[QRS], yPos-float((a[Q[QRS]]-nWaveDCOffset[0])*nGain*height()/100/nDataRange*15/2)),QPoint(Q[QRS], yPos-offset-30));
            paint->drawLine(QPoint(Q[QRS], yPos-offset-30),QPoint(S[QRS], yPos-offset-30));
            paint->drawLine(QPoint(S[QRS], yPos-offset-30),QPoint(S[QRS], yPos-float((a[S[QRS]]-nWaveDCOffset[0])*nGain*height()/100/nDataRange*15/2)));
            QRS=QRS+1;
        }
    }

    paint->end();
    update();
}

void WaveShow::Draw_Pwave(short *a, short *Pwave,short *P)
{
    int yPos= (y()+height())*dispChanPos[0]/dispChanGross[1];

    int pnum,i;
    float offset,offset1;
    for(i=2;i<249;i++)
    {
        if (P[i]==1)
        {
            draw_square(i,a[i],3,3);
        }
    }

    QPainter *paint = new QPainter(Wavemap);
    nWaveChanColor[7].setWidth(1);
    paint->setPen(nWaveChanColor[7]);
    pnum=1;
    for(i=2;i<249;i++)
    {
        if(Pwave[pnum]==i)
        {
            offset = float((a[Pwave[pnum]]-nWaveDCOffset[0])*nGain*height()/100/nDataRange*15/2);
            offset1 = float((a[Pwave[pnum+1]]-nWaveDCOffset[0])*nGain*height()/100/nDataRange*15/2);
            paint->drawLine(QPoint(Pwave[pnum],yPos-offset),QPoint(Pwave[pnum],yPos-offset-10));
            paint->drawLine(QPoint(Pwave[pnum],yPos-offset-10),QPoint(Pwave[pnum+1],yPos-offset-10));
            paint->drawLine(QPoint(Pwave[pnum+1],yPos-offset-10),QPoint(Pwave[pnum+1],yPos-offset1));
            pnum=pnum+2;
        }
    }

    paint->end();
    update();

}

void WaveShow::Draw_Twave(short *a, short *Twave,short *T)
{   
    int i,tnum;
    for(i=2;i<249;i++)
    {
        if (T[i]==1)
        {
            draw_square(i,a[i],5,5);
        }
    }
    int yPos= (y()+height())*dispChanPos[0]/dispChanGross[1];
    QPainter *paint = new QPainter(Wavemap);
    nWaveChanColor[7].setWidth(1);
    paint->setPen(nWaveChanColor[7]);

    tnum=1;
    float offset,offset1;
    for(i=2;i<249;i++)
    {
        if(Twave[tnum]==i)
        {
            offset = float((a[Twave[tnum]]-nWaveDCOffset[0])*nGain*height()/100/nDataRange*15/2);
            offset1 = float((a[Twave[tnum+1]]-nWaveDCOffset[0])*nGain*height()/100/nDataRange*15/2);
            paint->drawLine(QPoint(Twave[tnum],yPos-offset),QPoint(Twave[tnum],yPos-offset-10));
            paint->drawLine(QPoint(Twave[tnum],yPos-offset-10),QPoint(Twave[tnum+1],yPos-offset-10));
            paint->drawLine(QPoint(Twave[tnum+1],yPos-offset-10),QPoint(Twave[tnum+1],yPos-offset1));
            tnum=tnum+2;
        }
    }
    paint->end();
    update();
}


void WaveShow::ClearDisplayBuffer()
{
    if(pDispBuffer) 
        memset(pDispBuffer, 0, 600*10*MAX_CHANNEL*sizeof(float));
    nPBuffer = 0;
    nMapCount = 0;   
}

void WaveShow::setGain(int gain)
{
    nGain = gain;
}

void WaveShow::setFreqRatio(int ratio)
{
    freqRatio = ratio;
    nXratio = int (5000./freqRatio/(TYPICAL_SCAN_SPEED*nScanSpeed/1000.));
}

void WaveShow::setWaveChannel(int channelNum)
{
    nWaveChannel = channelNum;
}

void WaveShow::setDataRange(int dataRang)
{
    nDataRange = dataRang;
}




