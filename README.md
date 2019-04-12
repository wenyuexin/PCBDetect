# PCBDetect
基于Qt5和OpenCV3的PCB缺陷检测系统

## 开发环境  

* Visual Studio 2017 Professional  
* Qt 5.11.1  
* OpenCV contrib 3.4.3

## 主要模块  
运动控制、相机控制、人机交互界面（主界面、参数设置界面、模板提取界面、检测界面、序号识别界面等）、核心算法（模板提取算法、缺陷检测算法）、字符识别（OCR）  

## 文件说明

| 文件 | 类型 | 说明 |
| :---: | :---: | ----- |
| PCBDetect | .h / .cpp | 程序主界面 |
| PCBDetect | .ui | 程序主界面的外观设计和控件布局 |
| PCBDetect | .qrc | Qt界面的资源配置文件 |
| Configurator | .h / .cpp | 参数配置器，用于实现对参数配置文件的读写，<br>同时内部定义了参数类DetectConfig和AdminConfig等 |
| - |  |  |
| LaunchUI | .h / .cpp | 启动界面 |
| LaunchUI | .ui | 启动界面的外观设计和控件布局 |
| SysInitThread | .h / .cpp | 系统初始化线程，启动过程中在后台执行初始化操作 |
|  |  |  |
| SettingUI | .h / .cpp | 参数设置界面 |
| SettingUI | .ui | 参数设置界面的外观设计和控件布局 |
| PassWordUI | .h / .cpp | 系统参数登录界面 |
| PassWordUI | .ui | 系统参数登录界面的外观设计和控件布局 |
| AdminSettingUI | .h / .cpp | 系统参数设置界面 |
| AdminSettingUI | .ui | 系统参数设置界面的外观设计和控件布局 |
| - |  |  |
| MotionControler | .h / .cpp | 运动控制模块，用于向控制板发送控制指令，<br/>进而控制机械运动结构的相关动作（初始化、前进、归零、复位） |
| AMC98C | .h | 用于和控制板通信的若干内容 |
| CameraControler | .h / .cpp | 相机控制器，用于控制相机的相关动作（初始化、拍照） |
| - |  |  |
| SerialNumberUI | .h / .cpp | 产品序号识别界面，内部集成OCR模块，用于识别产品序号 |
| SerialNumberUI | .ui | 产品序号识别界面的外观设计和控件布局 |
| - |  |  |
| ImgConvertThread | .h / .cpp | 图像转换线程，管理若干图像类型转换器，<br> 能使实现对一组分图做图像类型转换类型 |
| ImageConverter | .h / .cpp | 图像类型转换器，启用单独的线程对一个分图做图像类型转换 |
| - |  |  |
| TemplateUI | .h / .cpp | 模板提取界面 |
| TemplateUI | .ui | 模板提取界面的外观设计和控件布局 |
| TemplateThread | .h / .cpp | 模板提取线程 |
| TemplateExtractor | .h / .cpp | 模板提取器 |
| TemplFunc | .h / .cpp |  模板提取器的辅助类 |
| - |  |  |
| DetectUI | .h / .cpp | 检测界面 |
| DetectUI | .ui | 检测界面的外观设计和控件布局 |
| DetectThread | .h / .cpp | 检测线程 |
| DetectCore | .h / .cpp | 检测核心 |
| DetectFunc | .h / .cpp | 检测核心的辅助类 |
|                   |           |                                                              |

## 错误代码  



| 模块 | 说明 | 代码 |
| :---:| :---: | :---: |
| AdminConfig | 系统参数 | 0x100 ~ 0x1FF |
| DetectConfig | 用户参数 | 0x200 ~ 0x2FF |
| DetectParams | 运行参数 | 0x300 ~ 0x3FF |
| MotionControler | 运动控制 | 0x400 ~ 0x4FF |
| CameraControler | 相机控制 | 0x500 ~ 0x5FF |
| ImgConvertThread <br/>ImageConverter | 图像类型转换 | 0x600 ~ 0x6FF |
| SerialNumberUI | 产品序号识别 | 0x700 ~ 0x7FF |
| TemplateUI<br/>TemplateThread<br/>TemplateExtractor<br/>TemplFunc | 模板提取 | 0x800 ~ 0x8FF |
| DetectUI<br/>DetectThread<br/>DetectCore<br/>DetectFunc | 检测 | 0x900 ~ 0x9FF |
|                                                              |              |               |
|  |  |  |



