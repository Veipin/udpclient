//=====================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <pthread.h>
#include <sys/wait.h>
#include <stdint.h>

#include "udpclient.h"
#include "LibLog.h"

static int gRun = 1;

/*F InitLogInfo()
��ʼ����־*/
void InitLogInfo()
{
    InitPublic((char *)PROJ_DIR);
    InitLog(APP_NAME,LOG_DAYLIY);
}

/*F Signal_hander()
�źŴ����� */
void Signal_hander(int siga)
{
    gRun = 0;
}
void Signal_Pipe(int siga)
{
    //PrintSignal(siga);
}

void InitSignalProcess()
{
    signal(SIGINT, (void(*)(int))Signal_hander);//SIGINT 2 (�ж�) ���û�����ʱ,֪ͨǰ̨��������ֹ����
    signal(SIGQUIT,(void(*)(int))Signal_hander);//SIGQUIT 3 (�˳�) �û����»�ʱ֪ͨ���̣�ʹ������ֹ
    signal(SIGTERM,(void(*)(int))Signal_hander);//SIGTERM 15 (���ж�) ʹ�ò���������kill����ʱ��ֹ����
    signal(SIGABRT,(void(*)(int))Signal_hander);//SIGABRT 6 (�쳣��ֹ) ����abort�������ɵ��ź�
    signal(SIGPWR, (void(*)(int))Signal_hander);//SIGPWR 30 ��ԴʧЧ/������

    sigignore(SIGHUP); //SIGHUP 1 (����) �����н��̵��û�ע��ʱ֪ͨ�ý��̣�ʹ������ֹ
    signal(SIGPIPE,(void(*)(int))Signal_Pipe);  //SIGPIPE 13 д���޶����̵Ĺܵ�, ����Socketͨ��SOCT_STREAM�Ķ������Ѿ���ֹ������д��
}

// ./udpclient 5000 192.168.17.10:5001 100
// ./udpclient 5001 192.168.17.10:5000 100
int main(int argc,char *argv[])
{
    InitLogInfo();
    InitSignalProcess();
    
    SetAppLogPrint(true);
    SetAppLogLogGroup(false);
    AppLog(LOG_BASE, "udpclient start\n");

    if (argc != 4)
    {
        AppLog(LOG_BASE, "param err, please input: ./udpclient localport desIp:port sendTimes\n");
        return 0;
    }

    int localport = (int)atoi(argv[1]);
    UdpSocket sock;
    sock.Create(localport);
    
    SocketAddress to;
    to.SetIpAndPort(argv[2]);
    SocketAddress from;
        
    char body[128];
    int  dataLen = sprintf(body, "%s", "hello world"); 
    char buf[128];
    int recvflag, recvDataLen;
    int sendTimes = (int)atoi(argv[3]);
    sock.Send(body, dataLen+1, to);  

    while (gRun && (sendTimes > 0))
    {
        recvflag = sock.WaitInput(2);
        if (recvflag)
        {
            recvDataLen = sock.Recv(buf, 128, from);
            if (recvDataLen > 0)
            {
                AppLog(LOG_BASE, "recv len:%d from:%s [sendTimes:%d] data:%s\n", recvDataLen, from.ToString().data(), sendTimes, buf);
            }
            
            sock.Send(body, dataLen+1, to);  
            -- sendTimes;
        }
        
        usleep(5*1000); 
    }
    
    AppLog(LOG_BASE, "udpclient over\n");
    return 0;
}













