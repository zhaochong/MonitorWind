/****************************************************************************
**
** Copyright (C) 2006-2006 Trolltech ASA. All rights reserved.
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

#include "SetupWindow.h"


SetupWindow::SetupWindow(QWidget *parent ,Qt::WindowFlags f)
{
    pParent = (MonitorWind *)parent;
    createSetupGroupBox();

    pButton = new QPushButton(tr("确定"));
    pButton->setDefault(true);
    pButton->setFont(QFont("wenquanyi",14,QFont::Bold));

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(GroupBox,0,0,4,2);
    mainLayout->addWidget(pButton,5,1);
    setLayout(mainLayout);

    channelComboBox->setCurrentIndex(1);
    scanComboBox->setCurrentIndex(7);
    amplifyComboBox->setCurrentIndex(9);
    adbitComboBox->setCurrentIndex(2);

    connect(pButton, SIGNAL(clicked()), this, SLOT(setUpOk()));

    setWindowTitle(tr("SetUp"));
    resize(250, 320);
}

void SetupWindow::closeEvent(QCloseEvent *event)
{


}

void SetupWindow::setUpOk()
{

    pParent->channel = channelComboBox->currentText().toInt();
    pParent->gain = amplifyComboBox->currentText().toInt();
    pParent->scan = scanComboBox->currentText().toInt();
    pParent->adbit = adbitComboBox->currentText().toInt();

    done(QDialog::Accepted);

}

void SetupWindow::createSetupGroupBox()
{
    GroupBox = new QGroupBox(tr("监护仪设置"));
    GroupBox->setFont(QFont("wenquanyi",14,QFont::Bold));

    channelLabel = new QLabel("通道数:");
    channelLabel->setFont(QFont("wenquanyi",14,QFont::Bold));
    scanFreqLabel = new QLabel("扫描频率:");
    scanFreqLabel->setFont(QFont("wenquanyi",14,QFont::Bold));
    amplifyLabel = new QLabel("放大倍数:");
    amplifyLabel->setFont(QFont("wenquanyi",14,QFont::Bold));
    adBitLabel = new QLabel("AD位数:");
    adBitLabel->setFont(QFont("wenquanyi",14,QFont::Bold));

    channelComboBox = new QComboBox;
    channelComboBox->addItem(tr("1"));
    channelComboBox->addItem(tr("2"));
    channelComboBox->addItem(tr("3"));
    channelComboBox->addItem(tr("4"));
    channelComboBox->addItem(tr("5"));
    channelComboBox->addItem(tr("6"));
    channelComboBox->addItem(tr("7"));
    channelComboBox->addItem(tr("8"));
    channelComboBox->addItem(tr("9"));
    channelComboBox->addItem(tr("10"));
    channelComboBox->addItem(tr("11"));
    channelComboBox->addItem(tr("12"));

    scanComboBox = new QComboBox;
    scanComboBox->addItem(tr("20"));
    scanComboBox->addItem(tr("50"));
    scanComboBox->addItem(tr("75"));
    scanComboBox->addItem(tr("100"));
    scanComboBox->addItem(tr("125"));
    scanComboBox->addItem(tr("150"));
    scanComboBox->addItem(tr("175"));
    scanComboBox->addItem(tr("200"));
    

    amplifyComboBox  = new QComboBox;
    amplifyComboBox->addItem(tr("5"));
    amplifyComboBox->addItem(tr("10"));
    amplifyComboBox->addItem(tr("15"));
    amplifyComboBox->addItem(tr("20"));
    amplifyComboBox->addItem(tr("25"));
    amplifyComboBox->addItem(tr("30"));
    amplifyComboBox->addItem(tr("35"));
    amplifyComboBox->addItem(tr("40"));
    amplifyComboBox->addItem(tr("45"));
    amplifyComboBox->addItem(tr("50"));
    amplifyComboBox->addItem(tr("75"));
    amplifyComboBox->addItem(tr("100"));

    adbitComboBox = new QComboBox;
    adbitComboBox->addItem(tr("8"));
    adbitComboBox->addItem(tr("10"));
    adbitComboBox->addItem(tr("12"));
    adbitComboBox->addItem(tr("14"));
    adbitComboBox->addItem(tr("16"));
    adbitComboBox->addItem(tr("18"));
    adbitComboBox->addItem(tr("24"));


    QGridLayout *gLayout = new QGridLayout;
    gLayout->addWidget(channelLabel,0,0);
    gLayout->addWidget(channelComboBox,0,1);
    gLayout->addWidget(scanFreqLabel,1,0);
    gLayout->addWidget(scanComboBox,1,1);
    gLayout->addWidget(amplifyLabel,2,0);
    gLayout->addWidget(amplifyComboBox,2,1);
    gLayout->addWidget(adBitLabel,3,0);
    gLayout->addWidget(adbitComboBox,3,1);
    

    GroupBox->setLayout(gLayout);
}

