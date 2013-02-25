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

#ifndef WAVESHOW_H
#define WAVESHOW_H


#include <QWidget>
#include <QPixmap>
#include <QPainterPath>
#include <QPainter>

#define MAX_CHANNEL    12
#define TYPICAL_SCAN_SPEED    75
/*class QCheckBox;
class QGridLayout;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QSpinBox;
class QVBoxLayout; */

class WaveShow : public QWidget
{
    Q_OBJECT

public:
    WaveShow(QWidget *parent = 0);
    ~WaveShow();


    QPixmap *Wavemap;

    long WaveDisplay(short *pDataBuffer, long nDataBytes);
    void ClearDisplayBuffer();
    void setGain(int gain);
    void setFreqRatio(int ratio);
    void setWaveChannel(int channelNum);
    void setDataRange(int dataRang);

    void Draw_R(short *a,short *R2,int offset);
    void draw_square(int x, short y, int l,int k);
    void Draw_QRS(short *a, short *Q, short *S,short *R2);
    void Draw_Pwave(short *a, short *Pwave,short *P);
    void Draw_Twave(short *a, short *Twave,short *T);
    void Draw_Ecgwave(short *sig,int Num);

protected:
    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *event);
private slots:


private:
    int nWaveChannel;
    int nGain;
    int nWaveDCOffset[MAX_CHANNEL];
    QPen nWaveChanColor[MAX_CHANNEL];
    int nXratio;
    int nScanSpeed;
    int nPBuffer;
    int nMapCount;
    int nDataRange;/*default 12-bit ADC*/
    float *pDispBuffer;
    int freqRatio;

};

#endif
