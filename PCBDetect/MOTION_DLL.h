// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 MOTIONDLL_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// MOTIONDLL_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef MOTIONDLL_EXPORTS
#define MOTIONDLL_API __declspec(dllexport)
#else
#define MOTIONDLL_API __declspec(dllimport)
#endif

#include "AMC98C.h"
// 此类是从 dll 导出的
class MOTIONDLL_API CMOTIONDLL {
public:
	CMOTIONDLL(void);
	// TODO: 在此处添加方法。
};

extern MOTIONDLL_API int nMOTIONDLL;

MOTIONDLL_API int fnMOTIONDLL(void);

MOTIONDLL_API void Uart_AMC98_GetNewData_v2(Uart_AMC98_GetNewDataType fun);

/////////////////////////////////////////////////
//获取工作路径  path存储路径
MOTIONDLL_API I16 WINAPI AMC98_GetCurrWorkingDir1_v2(char * path);//VB用
//返回值是路径
MOTIONDLL_API char * WINAPI AMC98_GetCurrWorkingDir_v2(void);//VC用

/////////////////////////////////////////////////
//获取CNC参数指针地址
//卡1
MOTIONDLL_API int* WINAPI AMC98_GetCNCDataBuffer_v2(void);
//卡2
MOTIONDLL_API int* WINAPI AMC98_GetCNCDataTBuffer_v2(void);

/////////////////////////////////////////////////
//获取错误提示
//ErrorNum 错误编码 [0-100]
//Infor ErrorNum对饮的错误信息
MOTIONDLL_API char * WINAPI AMC98_GetErrString_v2(int ErrorNum);
MOTIONDLL_API I16 WINAPI AMC98_GetErrString1_v2(int ErrorNum, char * Infor);
/////////////////////////////////////////////////
//获取信息提示
//InfNum 信息编码 [0-100]
//Infor InfNum对饮的提示信息
MOTIONDLL_API char * WINAPI AMC98_GetInfString_v2(int InfNum);
MOTIONDLL_API I16 WINAPI AMC98_GetInfString1_v2(int InfNum, char * Infor);


//open close
/////////////////////////////////////////////////
//设置控制器
//nKZQ 控制器的名称
//nFlag 参数名 0控制器类型1设置2端口3协议
//setdata 参数值
MOTIONDLL_API I16 WINAPI AMC98_KZQSet_v2(UINT nKZQ, UINT nFlag, const char * setdata);
/////////////////////////////////////////////////
//建立连接
//hWnd 得到消息的窗口
//nHostI 控制器序号0,1,2,3
MOTIONDLL_API I16 WINAPI AMC98_Connect_v2(HWND hWnd, UINT nHostI);
/////////////////////////////////////////////////
//关闭连接
MOTIONDLL_API I16 WINAPI AMC98_close_v2();



/////////////////////////////////////////////////
//发送指令给控制板
//Comd 指令码
//data 参数
MOTIONDLL_API I16 WINAPI AMC98_Comand_v2(I16 Comd, F64 *data);
/////////////////////////////////////////////////
//发送参数到控制板
MOTIONDLL_API I16 WINAPI AMC98_ParamPC2CNC_v2(void);


//发送接收参数到控制器
/////////////////////////////////////////////////
//从计算机刷新数据到板子
//ParamNm 需要设置的参数的序号 d[ParamNm]=data就是想要做到事情
MOTIONDLL_API I16 WINAPI AMC98_AddParamPC2CNC_v2(int ParamNm, int data);

/////////////////////////////////////////////////
//设置输出口状态  该函数直接赋值给控制板
//OutputIndex 端口号 1-32
//pStatus 状态 0/1
MOTIONDLL_API I16 WINAPI AMC98_set_io_status_v2(I16 OutputIndex, I16 pStatus);


/////////////////////////////////////////////////
//对电机的停止控制-急停
//AxisNo 轴的号码1-12
MOTIONDLL_API I16 WINAPI AMC98_emg_stop_v2(I16 AxisNo);
//对电机的停止控制-减速停止
//AxisNo 轴的号码1-12
//Tdec 减速时间 0-60000 单位ms
MOTIONDLL_API I16 WINAPI AMC98_sd_stop_v2(I16 AxisNo, F64 Tdec);

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
MOTIONDLL_API I16 WINAPI AMC98_start_sr_move_v2(I16 CardNo,
	I16 AxisNo, F64 Dist, I16 PosType,
	F64 StrVel, F64 MaxVel, F64 Tacc, F64 Tdec, F64 SVacc, F64 SVdec);
