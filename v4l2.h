#ifndef V4L2_H
#define V4L2_H

#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <linux/videodev2.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <QDebug>
#include <sys/ioctl.h>
#include <QDateTime>
#include <errno.h>

#define Video_path	"/dev/video0"	//摄像头挂载路径
#define Image_high	480	//分辨率
#define Image_width	640	//分辨率
#define Video_count	2	//缓冲帧数
#define SAVEPICTURE	"/home/www/"	//图片保存的位置
#define PICTURETAIL	".yuv"	//图片格式

typedef struct _buffer  //定义缓冲区结构体
{

    void *start;
    unsigned int length;
}buffer;

class V4L2
{
public:
    V4L2();
    ~V4L2();

public:
    int fd;             //驱动文件句柄
    bool state;         //是否打开成功
    buffer *buffers;    //原始数据
    int n_buffer;

    struct v4l2_capability	cap;        //V4l2 参数结构体
    struct v4l2_fmtdesc fmtdesc;        //V4L2 枚举格式结构体
    struct v4l2_format      fmt;        //V4L2 数据流格式结构体
    struct v4l2_streamparm	setfps;     //V4L2 流类型相关参数
    struct v4l2_requestbuffers	req;	//V4L2 内存映射缓冲区结构体
    struct v4l2_buffer	buf;            //V4L2 视频缓冲区信息结构体
    enum	v4l2_buf_type	type;       //V4L2 缓冲区类型

    bool V4l2_Init(void);               //V4l2 初始化
    bool V4l2_Malloc(void);             //申请缓存区
    bool V4l2_capturing(void);          //采集视频数据
    bool Take_photo(void);              //拍照
    bool V4l2_Close(void);              //关闭

private:
    void uninit_device(void);	//断开映射，释放内存
    void close_device(void);	//关闭设备
    void stop_capturing(void);	//停止采集
    bool Get_Frame(void);	//获取视频图像帧
    bool Free_Frame(void);	//更新视频图像帧
    char* qstringToChar(QString sourceTmp);//QString 转 char*

};

#endif // V4L2_H
