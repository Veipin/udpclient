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
初始化日志*/
void InitLogInfo()
{
    InitPublic((char *)PROJ_DIR);
    InitLog(APP_NAME,LOG_DAYLIY);
}

/*F Signal_hander()
信号处理函数 */
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
    signal(SIGINT, (void(*)(int))Signal_hander);//SIGINT 2 (中断) 当用户按下时,通知前台进程组终止进程
    signal(SIGQUIT,(void(*)(int))Signal_hander);//SIGQUIT 3 (退出) 用户按下或时通知进程，使进程终止
    signal(SIGTERM,(void(*)(int))Signal_hander);//SIGTERM 15 (软中断) 使用不带参数的kill命令时终止进程
    signal(SIGABRT,(void(*)(int))Signal_hander);//SIGABRT 6 (异常中止) 调用abort函数生成的信号
    signal(SIGPWR, (void(*)(int))Signal_hander);//SIGPWR 30 电源失效/重启动

    sigignore(SIGHUP); //SIGHUP 1 (挂起) 当运行进程的用户注销时通知该进程，使进程终止
    signal(SIGPIPE,(void(*)(int))Signal_Pipe);  //SIGPIPE 13 写至无读进程的管道, 或者Socket通信SOCT_STREAM的读进程已经终止，而再写入
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













