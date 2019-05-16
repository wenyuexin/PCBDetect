// AMC98C.h


#if !defined(AFX_AMC98C_H__25A92AAB_7A68_4C71_854B_44E2624F7E58__INCLUDED_)
#define AFX_AMC98C_H__25A92AAB_7A68_4C71_854B_44E2624F7E58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	//#error include 'stdafx.h' before including this file for PCH
#endif

#define WeizhiType_XD       1//相对位置
#define WeizhiType_JD       3//绝对位置
//========================================================
//数据类型定义
#if !defined(S8)
    typedef signed char             S8, *PS8;
#endif

#if !defined(U8)
    typedef unsigned char           U8, *PU8;
#endif

#if !defined(S16)
    typedef signed short            I16, S16, *PS16;
#endif

#if !defined(U16)
    typedef unsigned short          U16, *PU16;
#endif

#if !defined(S32)
    typedef signed long             I32, S32, *PS32;
#endif

#if !defined(U32)
    typedef unsigned long           U32, *PU32;
#endif

#if !defined(LONGLONG)
    typedef signed _int64           LONGLONG;
#endif

#if !defined(ULONGLONG)
    typedef unsigned _int64         ULONGLONG;
#endif

typedef float F32;
typedef double F64;

//=======================================================================
#define DllExport		__declspec(dllexport)

#ifndef WINVER
#define WINVER 0x0502
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0502
#endif
//#include <sdkddkver.h>

#include<afx.h>
#include "windows.h"



////////////////////////////////////////////////////////////////
//程序获取数据成功的函数
#define xieyi_txt		1//协议是文本
#define xieyi_byte		2//二进制
#define xieyi_modbus	3//modebus
#define xieyi_amc		4//amc格式

typedef int (*Uart_AMC98_GetNewDataType)(int,unsigned char *,int);  //函数指针类型
DllExport void Uart_AMC98_GetNewData(Uart_AMC98_GetNewDataType fun);

/////////////////////////////////////////////////
//获取工作路径  path存储路径
DllExport I16 WINAPI AMC98_GetCurrWorkingDir1(LPSTR path);//VB用
//返回值是路径
DllExport LPSTR WINAPI AMC98_GetCurrWorkingDir(void);//VC用

/////////////////////////////////////////////////
//获取CNC参数指针地址
//卡1
DllExport int* WINAPI AMC98_GetCNCDataBuffer(void);
//卡2
DllExport int* WINAPI AMC98_GetCNCDataTBuffer(void);

/////////////////////////////////////////////////
//获取错误提示
//ErrorNum 错误编码 [0-100]
//Infor ErrorNum对饮的错误信息
DllExport LPSTR WINAPI AMC98_GetErrString(int ErrorNum);
DllExport I16 WINAPI AMC98_GetErrString1(int ErrorNum,LPSTR Infor);
/////////////////////////////////////////////////
//获取信息提示
//InfNum 信息编码 [0-100]
//Infor InfNum对饮的提示信息
DllExport LPSTR WINAPI AMC98_GetInfString(int InfNum); 
DllExport I16 WINAPI AMC98_GetInfString1(int InfNum,LPSTR Infor);


//open close
/////////////////////////////////////////////////
//设置控制器
//nKZQ 控制器的名称
//nFlag 参数名 0控制器类型1设置2端口3协议
//setdata 参数值
DllExport I16 WINAPI AMC98_KZQSet(UINT nKZQ, UINT nFlag, LPCTSTR setdata); 
/////////////////////////////////////////////////
//建立连接
//hWnd 得到消息的窗口
//nHostI 控制器序号0,1,2,3
DllExport I16 WINAPI AMC98_Connect(HWND hWnd, UINT nHostI);
/////////////////////////////////////////////////
//关闭连接
DllExport I16 WINAPI AMC98_close();



/////////////////////////////////////////////////
//发送指令给控制板
//Comd 指令码
//data 参数
DllExport I16 WINAPI AMC98_Comand(I16 Comd, F64 *data);
/////////////////////////////////////////////////
//发送参数到控制板
DllExport I16 WINAPI AMC98_ParamPC2CNC(void);


//发送接收参数到控制器
/////////////////////////////////////////////////
//从计算机刷新数据到板子
//ParamNm 需要设置的参数的序号 d[ParamNm]=data就是想要做到事情
DllExport I16 WINAPI AMC98_AddParamPC2CNC(int ParamNm,int data);

/////////////////////////////////////////////////
//设置输出口状态  该函数直接赋值给控制板
//OutputIndex 端口号 1-32
//pStatus 状态 0/1
DllExport I16 WINAPI AMC98_set_io_status(I16 OutputIndex, I16 pStatus);


/////////////////////////////////////////////////
//对电机的停止控制-急停
//AxisNo 轴的号码1-12
DllExport I16 WINAPI AMC98_emg_stop(I16 AxisNo);
//对电机的停止控制-减速停止
//AxisNo 轴的号码1-12
//Tdec 减速时间 0-60000 单位ms
DllExport I16 WINAPI AMC98_sd_stop(I16 AxisNo,F64 Tdec);

//移动电机
/////////////////////////////////////////////////
//singal axis position
//CardNo:0,1,2
//AxisNo:0,1,2,3
//Dist:-200000到200000个单位
//PosType:1相对位置3绝对位置
//StrVel:0-200000开始速度
//MaxVel:0-200000运行速度
//Tacc:0-250 加速时间
//Tdec:0-250 减速时间
//返回0此功能 其他值错误
DllExport I16 WINAPI AMC98_start_sr_move(I16 CardNo,
			I16 AxisNo, F64 Dist,  I16 PosType,
			F64 StrVel, F64 MaxVel,F64 Tacc, F64 Tdec, F64 SVacc, F64 SVdec);
#endif // !defined(AFX_AMC98C_H__25A92AAB_7A68_4C71_854B_44E2624F7E58__INCLUDED_)
