#include "ble.h"
//The BLE device
int ble_fd = 0;

//For controller to check
//blueteeth中断
//extern int flag_bt;                        //for controller to check
int head4ble, rear4ble;
int head2ble, rear2ble;
char buf4ble[MAX_BUF_SIZE][BLE_SIZE];
char buf2ble[MAX_BUF_SIZE][BLE_SIZE];
struct termios stNew;
struct termios stOld;
//Open Port & Set Port
int serial_init_ble()
{
    ble_fd = open(BLE_DEVICE, O_RDWR|O_NOCTTY|O_NDELAY);
    if(-1 == ble_fd)
    {
        perror("Open Serial Port Error!\n");
        return -1;
    }
    if( (fcntl(ble_fd, F_SETFL, 0)) < 0 )
    {
        perror("Fcntl F_SETFL Error!\n");
        return -1;
    }
    if(tcgetattr(ble_fd, &stOld) != 0)
    {
        perror("tcgetattr error!\n");
        return -1;
    }
    stNew = stOld;
    cfmakeraw(&stNew);//将终端设置为原始模式，该模式下所有的输入数据以字节为单位被处理
    //set speed
    cfsetispeed(&stNew, BLE_BAUDRATE);//9600
    cfsetospeed(&stNew, BLE_BAUDRATE);
    //set databits
    stNew.c_cflag |= (CLOCAL|CREAD);
    stNew.c_cflag &= ~CSIZE;
    stNew.c_cflag |= CS8;
    //set parity
    stNew.c_cflag &= ~PARENB;
    stNew.c_iflag &= ~INPCK;
    //set stopbits
    stNew.c_cflag &= ~CSTOPB;
    stNew.c_cc[VTIME]=0;    //指定所要读取字符的最小数量
    stNew.c_cc[VMIN]=1; //指定读取第一个字符的等待时间，时间的单位为n*100ms
                //如果设置VTIME=0，则无字符输入时read（）操作无限期的阻塞
    tcflush(ble_fd,TCIFLUSH);  //清空终端未完成的输入/输出请求及数据。
    if( tcsetattr(ble_fd,TCSANOW,&stNew) != 0 )
    {
        perror("tcsetattr Error!\n");
        return -1;
    }
    return ble_fd;
}

//return the len of the data
int ble_read(int ble_fd)
{
    int nRet=0;
    int flag=1;
    char str[BLE_SIZE];
    while(flag||nRet!=0)
    {
        printf("wait\n");
        nRet = read(ble_fd, str, BLE_SIZE);
        if(-1 == nRet)
        {
            perror("Read Data Error!\n");
            break;
        }
        if(0 < nRet)
        {
            str[nRet]=0;
            if(strcmp("exit",str)==0)
                break;
            strcpy(buf4ble[head4ble], str);
            printf("str Data: %s\n", str);
            if(head4ble==MAX_BUF_SIZE)
                head4ble=0;
            else
                head4ble++;
            if(head4ble==rear4ble)
                rear4ble++;
        }
    }
}

//write to the serial port
int ble_write(int ble_fd, int len)
{
    return write(ble_fd, buf2ble, len);
}


//BLE module
int BLE_init()
{
    bzero(buf4ble, BLE_SIZE);
    bzero(buf2ble, BLE_SIZE);
    return serial_init_ble();
}

//send data to ble
void send2ble()
{

}

//get data from ble
int get4ble(char* tmp)
{
    if(rear4ble!=head4ble)
    {
        strcpy(*tmp, buf4ble[rear4ble++]);
        if(rear4ble==MAX_BUF_SIZE)
            rear4ble=0;
        return 1;
    }
    else
        return 0;
}

void print_buf()
{
    int i=0;
    for(i=0;i<BLE_SIZE;i++)
    {
        printf("[%d]:%s\n", i, buf4ble[i]);
    }
}

/*
int main(int argc, char **argv)
{
    ble_fd=BLE_init();
    if( ble_fd == -1 )
    {
        perror("SerialInit Error!\n");
        return -1;
    }

    ble_read(ble_fd);

    print_buf();

    close(ble_fd);
    return 0;
}*/
