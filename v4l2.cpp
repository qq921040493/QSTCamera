#include "v4l2.h"

V4L2::V4L2()
{
}

V4L2::~V4L2()
{
}

bool V4L2::V4l2_Init(void)
{
if((fd=open(Video_path,O_RDWR)) == -1) //读写方 式打开摄像头
{
qDebug()<<"Error opening V4L interface"; //打印错误 信息
return false;
}
if (ioctl(fd, VIDIOC_QUERYCAP, &cap) == -1) //获取摄 像头详细参数
{
qDebug()<<"Error opening device "<<Video_path<<": unable to querydevice.";
return false;
}
else //打印 cap 信息
{
qDebug()<<"driver:\t\t" <<QString::fromLatin1((char *)cap.driver); //摄像头的驱动名称
qDebug()<<"card:\t\t" <<QString::fromLatin1((char *)cap.card); //摄像头产商信息
qDebug()<<"bus_info:\t\t" <<QString::fromLatin1((char *)cap.bus_info);//摄像头总线信息
qDebug()<<"version:\t\t" <<cap.version; //摄像头的版本
qDebug()<<"capabilities:\t"<<cap.capabilities; //摄像头容量信息
}



fmtdesc.index=0; //获取 摄像头支持的格式
fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
qDebug()<<"Support format:";
while(ioctl(fd,VIDIOC_ENUM_FMT,&fmtdesc)!=-1)
{
qDebug()<<"\t\t"<<fmtdesc.index+1<<QString::fromLatin1((char
*)fmtdesc.description);
fmtdesc.index++;
}
fmt.type =V4L2_BUF_TYPE_VIDEO_CAPTURE; //设置像素格式
fmt.fmt.pix.pixelformat =V4L2_PIX_FMT_YUYV; // 使 用 YUYV 格式输出
fmt.fmt.pix.height =Image_high; //设置图 像尺寸
 fmt.fmt.pix.width =Image_width;  fmt.fmt.pix.field =V4L2_FIELD_INTERLACED; //设 置扫描方式
if(ioctl(fd, VIDIOC_S_FMT, &fmt) == -1)
{
qDebug()<<"Unable to set format";
return false;
}
if(ioctl(fd, VIDIOC_G_FMT, &fmt) == -1) //重新读取 结构体，以确认完成设置
{
qDebug()<<"Unable to get format";
return false;
}
else
{
qDebug()<<"fmt.type:\t\t"<<fmt.type; //摄像头数 据流类型
qDebug()<<"pix.height:\t" <<fmt.fmt.pix.height; //摄像头数据 流分辨率
qDebug()<<"pix.width:\t\t" <<fmt.fmt.pix.width;
}
setfps.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; // 设置摄像头的帧率，这里一般设置为 30fps
setfps.parm.capture.timeperframe.denominator = 30; //fps=30/1=30


setfps.parm.capture.timeperframe.numerator = 1;
if(ioctl(fd, VIDIOC_S_PARM, &setfps)==-1)
{
qDebug()<<"Unable to set fps";
return false;
}
if(ioctl(fd, VIDIOC_G_PARM, &setfps)==-1) //重新读取结构 体，以确认完成设置
{
qDebug()<<"Unable to get fps";
return false;
}
else
{
qDebug()<<"fps:\t\t"<<setfps.parm.capture.timeperframe.denominator/setfps.parm.capture.timeperframe.numerator;
}
qDebug()<<"init "<<Video_path<<" \t[OK]\n";
return true;
}

/* 申请缓存区 ：申请若干视频采集的帧缓冲区，并将这些帧缓冲区从内核空间
便于应用程序读取/处理视频数据 */

bool V4L2::V4l2_Malloc(void)
{
req.count=Video_count; //申请 2 个 缓存区
req.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
//Buffer 的类型，此处设置为 V4L2_BUF_TYPE_VIDEO_CAPTURE
req.memory=V4L2_MEMORY_MMAP;
//Memory Mapping 模式，则此处设置为：V4L2_MEMORY_MMAP
if(ioctl(fd,VIDIOC_REQBUFS,&req)==-1)
{
qDebug()<<"request for buffers error";
return false;
}
int n_buffers;
buffers = (buffer *)malloc(req.count*sizeof (*buffers)); //malloc 缓冲区
if (!buffers)
{
qDebug()<<"Out of memory";
return false ;
}
for (n_buffers = 0; n_buffers < Video_count; n_buffers++) //mmap2 个缓冲区
{
buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; //类 型
buf.memory = V4L2_MEMORY_MMAP; //Memory Mapping 模式，则此处设置为：V4L2_MEMORY_MMAP
buf.index = n_buffers; //个数
if (ioctl (fd, VIDIOC_QUERYBUF, &buf) == -1) //querybuffers
{
qDebug()<<"query buffer error";
return false;
}
buffers[n_buffers].length = buf.length;
buffers[n_buffers].start = mmap(NULL,buf.length,PROT_READ
|PROT_WRITE, MAP_SHARED, fd, buf.m.offset); //map
if (buffers[n_buffers].start == MAP_FAILED)
{
qDebug()<<"buffer map error";
return false;
}
}
return true;
}



/* 采集视频数据 ：将将申请到的帧缓冲区在视频采集输入队列排队，并启动视频 采集 */
bool V4L2::V4l2_capturing(void)
{
unsigned int i;
for(i = 0; i < Video_count; ++i) //申请到的帧缓冲 区在视频采集输入队列排队
{
v4l2_buffer buf;
buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE; //类型
buf.memory =V4L2_MEMORY_MMAP;
//Memory Mapping 模式，则此处设置为：V4L2_MEMORY_MMAP
buf.index = i; //个数
if(-1 == ioctl(fd, VIDIOC_QBUF, &buf)) //把 buf 排成一 列
{
qDebug()<<"VIDIOC_QBUF error";
return false;
}
}
v4l2_buf_type type;
type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
if(-1 == ioctl(fd, VIDIOC_STREAMON, &type)) //启动视频采 集
{
qDebug()<<"VIDIOC_STREAMON error";
return false;
}
return true;
}



/* 拍照：驱动开始视频数据的采集，应用程序从视频采集输出队列取出帧缓冲 区，然后写入文件中，写完之后将帧缓冲区重新放入视频采集输入队列 */
bool V4L2::Take_photo(void)
{
QDateTime localTime = QDateTime::currentDateTime();//获取系统时 间
QString currentTime = localTime.toString("yyyyMMddhhmmss");//格式转换
char* LocalTime = qstringToChar(currentTime); //QString 转char*
char* filename = new char[50];
strcpy(filename, SAVEPICTURE); //拼接字 符串
strcat(filename, LocalTime);
strcat(filename, PICTURETAIL);
FILE* fp = fopen(filename, "wb"); //打开文件， 以写格式
if(fp == NULL)
{
qDebug()<< "create yuv file failure ";
return false;
}
Get_Frame(); //从视频采集输出队列取出帧缓
 fwrite(buffers[buf.index].start, buf.bytesused, 1,fp); //写入文件中  Free_Frame(); //将帧缓 冲区重新放入视频采集输入队列
delete []filename;
fclose(fp);
}


/* 获取视频图像帧 */
bool V4L2::Get_Frame(void)
{
if(ioctl(fd, VIDIOC_DQBUF, &buf) == -1)
{
return false;
}
return true;
}


/* 更新视频图像帧 */
bool V4L2::Free_Frame(void)
{
if(ioctl(fd, VIDIOC_QBUF, &buf) == -1)
{
return false;
}
return true;
}


/* 关闭:停止视频采集 */
bool V4L2::V4l2_Close(void)
{
stop_capturing(); //停止采集 uninit_device(); //断开映射，释放内存 close_device(); //关闭设备
}


/* 断开映射，释放内存 */
void V4L2::uninit_device(void)
{
unsigned int i;
for(i = 0; i < Video_count; ++i)
{
if(-1 == munmap(buffers[i].start, buffers[i].length))
{
qDebug()<<"close_device close error : " <<QString(strerror(errno));
return;
}
}
free(buffers);
}

/* 关闭设备 */
void V4L2::close_device(void)
{
if(-1 == close(fd))
{
qDebug()<<"close_device close error : " << QString(strerror(errno));
return;
}
}

/* 停止采集 */
void V4L2::stop_capturing(void)
{
v4l2_buf_type type;
type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
if(-1 == ioctl(fd, VIDIOC_STREAMOFF, &type))
{
qDebug()<<"VIDIOC_STREAMOFF : " << QString(strerror(errno));
return;
}
}

/* QString 转 char* */
char* V4L2::qstringToChar(QString sourceTmp)
{
if(sourceTmp.isEmpty())
{
return NULL;
}
QByteArray ba = sourceTmp.toLatin1();
char* targetTmp;
targetTmp = ba.data();
return targetTmp;
}










