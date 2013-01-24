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
#include "GroupContainer.h"

GroupContainer::GroupContainer(QWidget *parent)
{
    setPalette(QPalette(Qt::black));
    setWindowFlags(Qt::FramelessWindowHint);
    setAutoFillBackground(true);
}

void GroupContainer::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);     
}

void GroupContainer::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setPen(QPen(Qt::white));/*,1,Qt::DashDotLine */
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}
