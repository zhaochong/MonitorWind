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


#include "MonitorThread.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>

MonitorThread::MonitorThread(QObject *parent)
{
    stopped = FALSE;
    PacketCnt=0;
    pbuffer = buffer;
}


void MonitorThread::run()
{
    /*  int byteread; */
    //atmel = open("/dev/atmel0",O_RDWR);
    //printf("atmel = %d\n",atmel);

    while(!stopped)
    {
        /* if(atmeldev)
        { 
        byteread = read(atmeldev,pbuffer,64);
        pbuffer +=64;

         }

       if(++PacketCnt==3)
         {
           PacketCnt =0;
           pbuffer = buffer;
           emit sShowWave(buffer, byteread*3);
          }   */
        /*   for(int i=0;i<byteread;i++)
              printf("data=%d\n",buffer[i]); */ 

        readFileData();
    }

    
}

void MonitorThread::stop()
{
    stopped = TRUE;

    close(atmel);
}

void MonitorThread::readFilesInfo()
{
    int byteread;

    if(atmel)
    {
        ioctl(atmel,0x81,0);

        do{
            byteread = read(atmel,buffer,64);
            /* 在这里对数据包作处理 */

        }while(byteread==64);
    }

}

void MonitorThread::readFileData()
{   
    int byteread;
    unsigned long index=0;

    file.setFileName("/home/cust/data/100.dat");
    if(file.open(QIODevice::ReadOnly)){
        int i =0;
        do{
            byteread = file.read(pbuffer,64);
            pbuffer += 64;

            QThread::msleep(100);

            i++;

            if(++PacketCnt==3){
                PacketCnt = 0;
                pbuffer = buffer;
                emit sShowWave(buffer, byteread*3);
            }
        }while(byteread==64);

        stop();
    }
}
