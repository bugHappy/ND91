========================================================================
    助手底层库 : AssistantDll 
========================================================================

 除了默认库外，本DLL调用的库还有：
 
 zLib.DLL
 SevenZip.lib		7z.dll的COM接口的C++封装
 NdEnCr.dll			守护通信加密
 NdIphoneNet.dll	守护wifi通信（依赖 pthread.dll)
 MobileSync.lib		ios 5音乐同步库

安卓的这3个文件必须包含在安装包中 
AdbWinApi.dll
AdbWinUsbApi.dll
AndroidInterface.exe
并设定exe的路径到Option

 请设置环境变量PATH路径包含此目录，让测试用例能够找到DLL
 （为了让VS取得新的环境变量，需要重启VS）
 

========================================================================
gtest测试项目
========================================================================

在里面可以直接对类进行测试。

========================================================================
test_dll测试项目
========================================================================

使用DLL方式测试，请在debug_DLL和release_DLL模式下使用

在里面模仿界面调用DLL，只能对DLL export的类进行测试

注：Core、Data目录下的类是公开的
	Device和Command下的类是不公开的
	common目录下的类部分公开，如Exception类


========================================================================
工具类
========================================================================

GlobalOption
	选项类，单例, 使用方法参考TestLog中的测试用例
	目前支持存取int, double, wstring数据, 如需添加其他数据类型请通知我，或自行添加
	需要增加选项的，请增加enum OPTION_TYPE中增加项目，并遵守命名约定

Log
	日志类，单例, 使用用法请参考TestLog.cpp中的测试用例

Exception
	包括SystemException和AssistantException两个类
	若Exception类定义中没有需要的错误代码，请在类定义中增加enum类型

========================================================================
改进方案
========================================================================
