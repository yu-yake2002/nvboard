# NVBoard

## 介绍

NVBoard(NJU Virtual Board)是基于SDL开发的虚拟FPGA开发板，可以在Verilator仿真环境中模拟FPGA，支持LED，七段数码管，开关，键盘和VGA。

## 项目文件说明

```
.
├── LICENSE
├── README.md
├── board
│   └── N4
│       ├── board           # 引脚说明文件
│       ├── config.json     # 设置文件
│       └── pic             # N4图片资源
│           └── ...
├── example                 # 示例项目
│   └── ...
├── include
│   ├── at_scancode.h
│   ├── component.h
│   ├── constrs.h
│   ├── factory.h
│   ├── keyboard.h
│   ├── macro.h
│   ├── nvboard.h
│   ├── vga.h
│   └── view.h
├── scripts
│   ├── auto_pin_bind.py    # 生成引脚绑定代码的脚本
│   └── nvboard.mk          # NVBoard构建规则
└── src                     # NVBoard源码
    ├── component.cpp
    ├── event.cpp
    ├── factory.cpp
    ├── keyboard.cpp
    ├── nvboard.cpp
    ├── vga.cpp
    └── view.cpp
```

## 安装教程

1. 将项目拷贝到本地，`git clone https://github.com/yu-yake2002/nvboard.git`
2. 通过`apt-get install libsdl2-dev libsdl2-image-dev`安装SDL2和SDL2-image
3. 通过`apt-get install libjsoncpp-dev`安装jsoncpp
4. 把本项目的目录设置成环境变量`NVBOARD_HOME`

## 示例

`example`目录下包含一个示例项目，在该目录下通过 `make run` 命令可运行该项目。

## 接入verilator步骤

### API说明

NVBoard::NVBoardController提供了以下几组API

- `NVBoardController(std::string board = "N4")`: `NVBoardController`的构造函数，参数`board`的缺省值是`"N4"`，它应当是`board`目录下的一个子目录，构造函数将会从这个子目录中读取开发板的配置信息
- `~NVBoardController()`: 析构函数
- `void NVBoardBindPin(void *signal, bool is_rt, bool is_output, int len, ...)`: 将HDL的信号signal连接到NVBoard里的引脚上，具体地
  - `is_rt`为`true`时，表示该信号为实时信号，每个周期都要更新才能正确工作，如键盘和VGA相关信号；
    `is_rt`为`false`时，表示该信号为普通信号，可以在NVBoard更新画面时才更新，从而提升NVBoard的性能，如拨码开关和LED灯等，无需每个周期都更新
  - `is_output`为`true`时，表示该信号方向为输出方向(从RTL代码到NVBoard)；否则为输入方向(从NVBoard到RTL代码)
  - `len`为信号的长度，大于1时为向量信号
  - 可变参数列表`...`为引脚编号列表，编号为整数；绑定向量信号时，引脚编号列表从MSB到LSB排列
- `void NVBoardUpdate()`: 更新NVBoard中各组件的状态，每当电路状态发生改变时都需要调用该函数

### 引脚绑定

手动调用`NVBoardBindPin()`来绑定所有引脚较为繁琐。
为了方便进行信号的绑定，NVBoard项目提供了一套从自定义约束文件生成绑定代码的流程。具体地
1. 编写一个自定义格式的约束文件，其格式为
```
top=top_name

# Line comment inside nxdc
signal pin
signal (pin1, pin2, ..., pink)
```
在约束文件的第一行，需要指定顶层模块名（上文中为`top_name`）。
约束文件支持两种信号绑定方式，`signal pin`表示将顶层模块的`signal`端口信号绑定到引脚`pin`上，
`signal (pin1, pin2, ..., pink)`表示将顶层模块的`signal`信号的每一位从高到低依次绑定到`pin1, pin2, ..., pink`上。
约束文件支持空行与行注释。

2. 通过命令`python $(NVBOARD_HOME)/scripts/auto_pin_bind.py nxdc约束文件路径 auto_bind.cpp输出路径`来生成C++文件。

调用该文件中的`nvboard_bind_all_pins(controller, dut)`函数即可完成所有信号的绑定。

注意，该脚本的错误处理并不完善，若自定义约束文件中存在错误，则可能无法生成正确的报错信息与C++文件。
~~如果发现脚本中的错误也可以尝试修复一下然后丢pr~~

可以在`board`目录下的引脚说明文件中查看引脚信息。

### 调用API

在C++仿真代码中调用NVBoard提供的API
```c++
#include <nvboard.h>

// ...
NVBoard::NVBoardController *controller = new NVBoard::NVBoardController();
nvboard_bind_all_pins(controller, &dut);

while (1) {
  // ...
  controller->NVBoardUpdate();
}

delete controller;
```
具体地
* 在进入verilator仿真的循环前，对NVBoard进行初始化，然后对引脚进行绑定
* 在verilator仿真的循环中更新NVBoard各组件的状态
* 退出verilator仿真的循环后，销毁NVBoard的相关资源

### 编译链接

在Makefile中
* 将生成的上述引脚绑定的C++文件加入源文件列表
* 将NVBoard的构建脚本包含进来
```
include $(NVBOARD_HOME)/scripts/nvboard.mk
```
* 通过`make nvboard-archive`生成NVBoard的库文件
* 在生成verilator仿真可执行文件(即`$(NVBOARD_ARCHIVE)`)将这个库文件加入链接过程，并添加链接选项`-lSDL2 -lSDL2_image -ljsoncpp`

可以参考示例项目中的Makefile文件，即`example/Makefile`

### 自定义开发板

N4开发板的配置信息如下：

```
.
├── board
├── config.json
└── pic
    ├── vbg_1.png
    ├── vbg_2.png
    ├── vbtn_off.png
    ├── vbtn_on.png
    ├── vsegled_bg.png
    ├── vsw_off.png
    └── vsw_on.png
```

- `board`文件描述了N4的引脚
- `config.json`文件描述了NVBoard的启动配置，N4的贴图、外设位置、外设数量
- `pic`目录下是N4的贴图

#### 配置文件规约

`config.json`必须包含以下所有 keys 和对应的 values：

- `name`，开发板名称
- `shape`，窗口大小，包含了开发板与VGA（如果有）
- `background`，背景贴图，一个 JSON 数组，描述了若干个贴图，依次输出在窗口中，后面的贴图会覆盖前面的贴图，每个贴图有以下所有 keys 和对应的 values：
  - `pic`，文件名
  - `location`，贴图左上角在窗口中的所在位置
  - `shape`，贴图的width和height
- `components`，外设组件，一个 JSON 数组，数组中的每个元素至少要有以下的 keys 和对应的 values：
  - `rt`，是否实时更新，`true` 或 `false`
  - `class`，调用哪个类的工厂方法
  - 其他的 keys 与 values 视 `class` 而定，可以参考 N4 的 `config.json`，结合 NVBoard 代码理解

还有一些 keys 和对应的 values 并不是必须的：

- `version`，版本号，用字符串表示
- `vsync`，NVBoard是否开启垂直同步，`true` 或 `false`，缺省值是 `false`
- `hardware_acc`，NVBoard是否开启硬件加速，`true` 或 `false`，缺省值是 `false`

如果你在运行NVBoard时出现了某些神秘问题，可以试着关掉垂直同步与硬件加速。
