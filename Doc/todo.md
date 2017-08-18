## Roadmap

1.reflection/serialization

2.terrain->csm->vsm

3.**undo/redo**



UnityEditor::DragAndDrop

模拟imgui的时候expand属性有问题

**去掉QSlider/QCombox(windows)的wheel响应**

**Asset**

**EditorSceneManager**

**SceneManager**

GameObjectInspector



Hierarchy中drag&drop在debug模式下会挂掉

asset cache里面path大小写的问题

ProjectView中的icon垂直居中，label靠下。

subclass QStyledItemDelegate之后发现icon不能居中。。。https://bugreports.qt.io/browse/QTBUG-13177 10年开始的bug一直没修过



渲染第一帧的时候，shadow是最先开始的，而SkinnedMeshRenderer.matrixPalette还没有生成，drawElements的时候会挂掉。解决方法：SkinnedMeshRenderer.setSharedMesh的时候resize matrixPalette



加载脚本的dll的时候，MD/MT不匹配会挂，GameObject清理的时候也会挂



Animation retargeting

GPU skinning



## Engine

- [x] 数学库换成左手系

- [x] 最小化窗口是aspect是变成nan，glm::perspective会abort掉

- [x] 改成左手系之后cubemap反了

- [ ] GLSL中TBN，cross的问题（cross是手系问题的）

- [x] 如何减少transform::update的调用次数？简单使用脏属性来标记是不正确的，因为不知道父节点上的信息是不是更新了（那么父节点变化时通知所有子节点？或者脏属性像GameObject::active一样拿（这样也有个问题，就是父节点可能变脏了，但是在子节点check脏属性之前父节点update了）？）。

      ​	暂时的解决方案：节点变化（第一次变脏）时将所有子节点标记为脏（递归过程，会把子孙所有节点标记为脏）。

- [ ] SceneView按F/Hierarchy中双击选中的物体会自动居中，并且按boundingbox调整相机距离

- [x] 高分屏的处理（尤其是高分屏移动到普通屏）

- [x] **cmake将qt项目移到xcode/VS**

- [x] 动态添加/删除Component（删除搞定，script和component分开；添加要用到反射？）

- [x] 将资源loader和Shader、Mesh、Texture之类的具体类分离（Model已经分离，动画导入WIP）

- [ ] depth \in (-1, 1)，下次换成D3D的时候小心

- [ ] glm四元数的-0问题，eulerangle的数字不太合适(暂时OK)

- [ ] GameObject构造函数改成非public的(需要解决make_shared调用非public ctor，貌似没有特别优雅的解决方案，http://stackoverflow.com/questions/8147027/how-do-i-call-stdmake-shared-on-a-class-with-only-protected-or-private-const)

- [x] 将Scene里面的Script和Component合并到一起去（~~需要在运行时知道是不是某个类的子类~~，搞定，用enable_if)

- [x] 不依赖代码生成和外部工具的（自动）反射可行吗？

- [ ] 把不该开放的API隐藏掉（尤其是FishEngine）

- [x] transform父子关系改变时的操作，应该维持子节点在world space下的T、R、S都不变

- [x] assimp导入的fbx动画有冗余的（骨骼？）结点（~~done，导入时加参数~~），解析每个节点的local transformation。

- [x] ~~basic model用NEF代替~~：http://assimp.sourceforge.net/howtoBasicShapes.html。从Unity导出

- [x] eulerangles数值不自然（暂时解决）

- [ ] 用bsp或八叉树之类的结构来组织场景，相交测试，鼠标选取物体等等

- [ ] const的位置

- [x] class Graphics

- [x] class CommandBuffer

- [ ] class UnityEditor.DockArea

- [x] 我需要一个动态的反射操作

      1: "MeshFilter" ==> make_shared<MeshFilter>() 而且要运行期动态生成或者变成dll动态加载

      2：classname="MeshFilter" mesh="..." -==> 

      auto mf = make_shared<MeshFilter>();

      mf->setMesh(...)

- [x] selection 换成像Unreal和Unity 5.5中一样的outline

## Editor

- [ ] GameObject inactive时在Hierarchy中颜色变灰色​

- [x] skybox遮挡了（写depth buffer的问题？）

- [x] 相机旋转时的万向锁的问题

- [x] SceneGizmo：遮挡问题（clear depth buffer解决），鼠标hover事件，点击后旋转相机、切换相机的透视模式

- [x] ~~鼠标等的事件被Editor处理后不再被分发到场景中（wantCapture...或者说判断鼠标在不在scene window中）~~

- [x] Inspector窗口中的Headers的缩进（OK，用imgui::indent()），一行显示不全的问题（暂时解决，Unity也没有解决label多长的问题）

- [x] Editor的帧率改到30

- [x] 相机旋转时transform反应不正常，会越退越远

- [ ] 场景中物体的平移（完成，相机需要在任何scale下面都有一样的translation，selection改变时清空selected axis暂时搞定，用球代替Bounds求交？）、旋转(改成半圆, 绘制扫过的扇形等)、缩放等操作

      区分global和local

- [ ] GUI样式美化

- [ ] 资源管理 WIP

- [x] 场景中的网格平面

- [x] 场景的序列化和反序列化（~~要依赖反射~~，内部的component可以不依赖反射，直接手写序列化，但是user defined的类需要反射来做自动序列化和反序列化）

- [x] Docking GUI

- [ ] ~~hierarchy中的方向键的移动~~

- [ ] hierarchy中过滤器（名字，类型。。。）

- [ ] ToolBar

- [ ] 区分read-only的属性 ~~ImGuiInputTextFlags_ReadOnly~~

- [x] undo/redo

- [ ] ~~鼠标样式，比如改变dock尺寸的时候~~

- [ ] hierarchy 里面go的顺序问题，现在不能调整问题

      ​



## 渲染

- [x] PBR

- [x] Shadow: standard shadow map with PCF => CSM/~~VSM~~, Screen space shadow map

      CSM+ (blurred) Screen space shadow map

- [x] expose更多的shader uniform给Editor

- [ ] 渲染优化：渲染前按父子关系将transform更新完，渲染时按批次重组draw call batching

- [x] uniform buffer

- [x] Gbuffer结构和deferred rendering

- [ ] tone mapping

- [x] 复杂场景的渲染，如sponza

- [x] 更完善的shader include，现在是单纯的查找替换，没有语义，会把注释里面的替换掉，甚至中间不能加空格。区分<>和""，系统目录和当前目录。~~(boost::wave)~~

- [ ] 考虑用bison做shader代码生成

- [x] skinned mesh运动时的线框显示问题（以后考虑用transform feedback加速动画）

- [x] Directional Light 抖动的问题

- [ ] Terrain/height map

- [x] builtinMaterial每次都会make_shaderd新的

- [ ] InternalUniform和普通的uniform区分开来，固定某些texture的binding point(GL4.1没有这个特性)

- [x] shader compiler中去掉boost::wave，依赖太多了，boost::thread,boost::system,boost::spirit...



## 物理

- [ ] PhysX（WIP）
- [ ] ​



## Asset

- [x] Texture里面加入sampler2D/samplerCube之类的类型标识




## Audio

- [ ] 基本的声音
- [ ] 3D音效


## 性能

Editor在macOS上已经有点慢了