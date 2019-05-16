// AMC98C.h


#if !defined(AFX_AMC98C_H__25A92AAB_7A68_4C71_854B_44E2624F7E58__INCLUDED_)
#define AFX_AMC98C_H__25A92AAB_7A68_4C71_854B_44E2624F7E58__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	//#error include 'stdafx.h' before including this file for PCH
#endif

#define WeizhiType_XD       1//���λ��
#define WeizhiType_JD       3//����λ��
//========================================================
//�������Ͷ���
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
//�����ȡ���ݳɹ��ĺ���
#define xieyi_txt		1//Э�����ı�
#define xieyi_byte		2//������
#define xieyi_modbus	3//modebus
#define xieyi_amc		4//amc��ʽ

typedef int (*Uart_AMC98_GetNewDataType)(int,unsigned char *,int);  //����ָ������
DllExport void Uart_AMC98_GetNewData(Uart_AMC98_GetNewDataType fun);

/////////////////////////////////////////////////
//��ȡ����·��  path�洢·��
DllExport I16 WINAPI AMC98_GetCurrWorkingDir1(LPSTR path);//VB��
//����ֵ��·��
DllExport LPSTR WINAPI AMC98_GetCurrWorkingDir(void);//VC��

/////////////////////////////////////////////////
//��ȡCNC����ָ���ַ
//��1
DllExport int* WINAPI AMC98_GetCNCDataBuffer(void);
//��2
DllExport int* WINAPI AMC98_GetCNCDataTBuffer(void);

/////////////////////////////////////////////////
//��ȡ������ʾ
//ErrorNum ������� [0-100]
//Infor ErrorNum�����Ĵ�����Ϣ
DllExport LPSTR WINAPI AMC98_GetErrString(int ErrorNum);
DllExport I16 WINAPI AMC98_GetErrString1(int ErrorNum,LPSTR Infor);
/////////////////////////////////////////////////
//��ȡ��Ϣ��ʾ
//InfNum ��Ϣ���� [0-100]
//Infor InfNum��������ʾ��Ϣ
DllExport LPSTR WINAPI AMC98_GetInfString(int InfNum); 
DllExport I16 WINAPI AMC98_GetInfString1(int InfNum,LPSTR Infor);


//open close
/////////////////////////////////////////////////
//���ÿ�����
//nKZQ ������������
//nFlag ������ 0����������1����2�˿�3Э��
//setdata ����ֵ
DllExport I16 WINAPI AMC98_KZQSet(UINT nKZQ, UINT nFlag, LPCTSTR setdata); 
/////////////////////////////////////////////////
//��������
//hWnd �õ���Ϣ�Ĵ���
//nHostI ���������0,1,2,3
DllExport I16 WINAPI AMC98_Connect(HWND hWnd, UINT nHostI);
/////////////////////////////////////////////////
//�ر�����
DllExport I16 WINAPI AMC98_close();



/////////////////////////////////////////////////
//����ָ������ư�
//Comd ָ����
//data ����
DllExport I16 WINAPI AMC98_Comand(I16 Comd, F64 *data);
/////////////////////////////////////////////////
//���Ͳ��������ư�
DllExport I16 WINAPI AMC98_ParamPC2CNC(void);


//���ͽ��ղ�����������
/////////////////////////////////////////////////
//�Ӽ����ˢ�����ݵ�����
//ParamNm ��Ҫ���õĲ�������� d[ParamNm]=data������Ҫ��������
DllExport I16 WINAPI AMC98_AddParamPC2CNC(int ParamNm,int data);

/////////////////////////////////////////////////
//���������״̬  �ú���ֱ�Ӹ�ֵ�����ư�
//OutputIndex �˿ں� 1-32
//pStatus ״̬ 0/1
DllExport I16 WINAPI AMC98_set_io_status(I16 OutputIndex, I16 pStatus);


/////////////////////////////////////////////////
//�Ե����ֹͣ����-��ͣ
//AxisNo ��ĺ���1-12
DllExport I16 WINAPI AMC98_emg_stop(I16 AxisNo);
//�Ե����ֹͣ����-����ֹͣ
//AxisNo ��ĺ���1-12
//Tdec ����ʱ�� 0-60000 ��λms
DllExport I16 WINAPI AMC98_sd_stop(I16 AxisNo,F64 Tdec);

//�ƶ����
/////////////////////////////////////////////////
//singal axis position
//CardNo:0,1,2
//AxisNo:0,1,2,3
//Dist:-200000��200000����λ
//PosType:1���λ��3����λ��
//StrVel:0-200000��ʼ�ٶ�
//MaxVel:0-200000�����ٶ�
//Tacc:0-250 ����ʱ��
//Tdec:0-250 ����ʱ��
//����0�˹��� ����ֵ����
DllExport I16 WINAPI AMC98_start_sr_move(I16 CardNo,
			I16 AxisNo, F64 Dist,  I16 PosType,
			F64 StrVel, F64 MaxVel,F64 Tacc, F64 Tdec, F64 SVacc, F64 SVdec);
#endif // !defined(AFX_AMC98C_H__25A92AAB_7A68_4C71_854B_44E2624F7E58__INCLUDED_)
