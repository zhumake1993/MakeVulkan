MakeVulkan！

E001BasicCube：一个顶点着色的cube，包含大部分基本功能，一些注释也写在这里

E002Texture：一个使用纹理的cube，没有mipmap

E003DragonLit：实现龙书的光照，使用SpecializationConstant控制每种灯的数量，平行光，点光和聚光各一个

E004Mipmap：mipmap，使用lodbias来控制采样的mip层

E005Batch：dynamic uniform buffer，实现batch，一种简化版的srp batcher

E006Instancing：GPUInstancing，使用SpecializationConstant控制instance的数量

E007TextureArray：纹理数组

E008Cubemap：Cubemap和CubemapArray

E009InputAttachments：实现fetch
参考https://www.saschawillems.de/blog/2018/07/19/vulkan-input-attachments-and-sub-passes/

E010SubPasses：一个renderpass里面实现deferred shading。感觉透明物体的实现有点多余，不能直接画么？

E999Test：测试用


新增vs子工程：
复制BasicCube
改名字
重新build

新增android子工程：
复制BasicCube
删除.cxx和build
删除assets
删除BasicCube.iml
修改cmakelist ：set(name BasicCube)
修改gradle task copyTask
同步

查看安卓上写的文件：
View -> Tool Windows -> Device File Explorer -> /data/data/com.example.MakeVulkan/GPUProfiler.txt