MakeVulkan！

VulkanSDK: 1.2.131.2

新增vs子工程
复制startup
改名字
重新build

新增android子工程：
复制startup
删除.cxx和build
删除startup.iml
修改cmakelist ：set(name startup)
修改gradle task copyTask
同步

//todo
//SetWindowText(global::windowHandle, "Triangle");

ConfigGlobalSettings();


#define MaxObjectsCount 100
#define MaxMaterialsCount 100

tiny_obj_loader 的索引有点奇怪，0 1 2 3 4 5 6 。。。。。这样的，待验证