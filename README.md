[TOC]

- [一、程序介绍](## 一、程序介绍)
  - [1、概述](### 1、概述)

![SPP-Practice](https://pic-bed-1316053657.cos.ap-nanjing.myqcloud.com/img/SPP-Practice.png)

## 一、程序介绍

### 1、概述

之前看过一些开源代码（https://github.com/LiZhengXiao99/Navigation-Learning），在自己开始写之前，先基于 RTKLIB 写一套后处理伪距单点定位练练手，熟悉一下 VScode+WSL 的 C++ 开发环境、Eigen、RTKLIB、glog、yaml-cpp 库的使用，为这套程序设计的结构、写的函数也可以沿用到以后正经的项目。

### 2、设计思路

* 程序单线程后处理，主函数要写的清晰易懂，最好只看主函数就能了解程序执行的大致流程。
* 所有类都通过智能指针来操作，每个类定义之后都用 `using xxxPtr = std::shared_ptr<xxx>` 给其智能指针类型起别名，方便使用。
* 出于可读性考虑，类的构造函数不要写太复杂，只做字段的初始化，其它操作用单独的成员函数来实现。
* 尽量不改 RTKLIB 原来的代码，要不别人看咱们写的代码不知道哪里改了哪里没改，还按原本 RTKLIB 的内容来理解会问题。需要拓展  RTKLIB 代码就自己重新写，注释里写清楚参考了哪个原函数，出于什么目的改动了什么。
* 用子类重写虚函数来实现相同操作的不同选项（比如不同对流层、电离层模型），在对应操作位置调用基类的虚函数，根据选项传对应的子类，避免太多的 `if-else` 和 `switch-case`。如果操作比较简单，一个函数几十行就能实现，写成仿函数（重载类的括号运算符）。
* 

### 3、第三方库

* **RTKLIB**：开源 GNSS 软件包，由一个程序库和多个应用程序工具库组成。
* **yaml-cpp**：C++ 的 YAML 库，用于 yaml 格式的解析和生成。
* **glog**：Google 的日志库。
* **Eigen**：线性代数库，用于处理矩阵和向量的计算，它提供了许多线性代数运算的功能，包括矩阵运算、向量运算、特征值分解、奇异值分解、矩阵求逆等。

### 4、类型设计

* **Option**：存所有配置信息，实现所有配置读取。内部存三个选项类型
  * **FileOption**：存文件路径
  * **SolutionOption**：存结果选项
  * **ProcessOption**：存解算选项
* **FileReaderBase**：文件读取基类，不同的文件对应不同的 FileReaderBase 子类，比如 ObsReader、NavReader。
* **SppEstimater**：SPP 估计器，实现 SPP 解算。
* **KalmanFilter**：卡尔曼滤波器实现。



## 二、程序结构、CMake

### 1、文件结构

* include 和 src 文件夹分别存头文件、源文件
* 3rd 文件夹存第三方库，本项目只有 RTKLIB
* app 文件夹存应用程序主文件
* config 文件夹存配置文件
* data 文件夹存示例数据
* build 文件夹存 cmake、make 的中间文件
* doc 文件夹存文档，manual 介绍软件的编译使用、algorithm 介绍涉及到的模型算法

### 2、根目录 CMake

* 指定最小 CMake 版本、项目名

  ```cmake
  cmake_minimum_required(VERSION 3.0)
  project(SPP_PRACTICE)
  ```

* 设置编译标志，用 C++ 11 标准，忽略不太严重的错误

  ```cmake
  set(CMAKE_CXX_FLAGS "-std=c++11" )
  set(CMAKE_CXX_FLAGS "-fpermissive")
  if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O3 -fsee -fomit-frame-pointer -fno-signed-zeros -fno-math-errno -funroll-loops")
  endif()
  ```

* 指定可执行文件的输出路径为：app/bin、库文件的输出路径为：app/lib。这样的设置可以帮助组织和管理项目的编译输出，使得可执行文件和库文件在同一个相对目录中，方便链接和使用。

  ```cmake
  set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/app/bin)
  set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/app/lib)
  ```

* 添加 RTKLIB 库，加上宏定义（开启BDS、GLONASS、GALIEO，三频，动态链接库）

  ```cmake
  add_definitions(-DENAGLO -DENACMP -DENAGAL -DNFREQ=3 -DNEXOBS=3 -DDLL)
  ```

  告诉 CMake 进入 `3rd/rtklib` 目录并执行其中的 CMakeLists.txt 文件。这会在该子目录下配置并生成构建目标。这可以引用其他库或者模块的构建过程，并将其集成到当前项目的构建中。

  ```cmake
  add_subdirectory(3rd/rtklib)
  ```

* 添加 Eigen、Yaml-cpp、Glog 库

  ```cmake
  find_package(Eigen3 REQUIRED NO_MODULE)
  include_directories(${EIGEN3_INCLUDE_DIRS})
  
  find_package(yaml-cpp REQUIRED)
  
  find_package(glog REQUIRED)
  ```

* 获取 src 目录下的源文件，并将它们添加到列表 DIR_ALL 中

  ```cmake
  aux_source_directory(src/utility DIR_utility)
  aux_source_directory(src/gnss    DIR_gnss)
  list(APPEND DIR_ALL ${DIR_utility}
                      ${DIR_gnss})
  ```

* 把核心代码库编译成动态库，链接上 `rtklib` ：

  ```cmake
  add_library(${PROJECT_NAME} SHARED ${DIR_ALL})
  target_link_libraries(${PROJECT_NAME} rtklib)
  target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include)
  ```

* 根据 app/spp_main.cc 生成可执行文件 SPP，并链接上核心库：

  ```cmake
  add_executable(SPP app/SPP_main.cc)
  target_link_libraries(SPP ${PROJECT_NAME})
  ```

### 3、RTKLIB 目录 CMake

* 指定最小 CMake 版本、子项目名

  ```cmake
  cmake_minimum_required(VERSION 3.0)
  project(rtklib)
  ```

* 设置编译时 gcc 参数：

  ```cmake
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -Wall -O3 -ansi -pedantic")
  set(CMAKE_C_FLAGS "-Wno-unused-but-set-variable -Wno-format-overflow -Wno-unused-result -Wpointer-to-int-cast")
  ```

* 指定头文件目录：

  ```cmake
  include_directories(include)
  ```

* 指定可执行文件的输出路径为：rtklib/bin、库文件的输出路径为：rtklib/lib：

  ```cmake
  set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
  set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)
  ```

* 将 src、src/rcv 目录下源文件加到 DIR_SRCS 列表：

  ```cmake
  aux_source_directory(src DIR_SRCS_RTKLIB)
  aux_source_directory(src/rcv DIR_SRCS_RTKLIB_RCV)
  list(APPEND DIR_SRCS ${DIR_SRCS_RTKLIB} ${DIR_SRCS_RTKLIB_RCV})
  ```

* 把代码编译成动态库，链接上 pthread m 库：

  ```cmake
  add_library(${PROJECT_NAME} SHARED ${DIR_SRCS})
  target_link_libraries(${PROJECT_NAME} pthread m)
  target_include_directories(${PROJECT_NAME}
      PUBLIC ${PROJECT_SOURCE_DIR}/include
  )
  ```

* 如果是 WIN32 还有链接上 wsock32 ws2_32 winmm 库，加上宏定义 -DWIN_DLL：

  ```cmake
  if(WIN32)
    target_link_libraries(${PROJECT_NAME} wsock32 ws2_32 winmm)
    add_definitions(-DWIN_DLL)
  endif()
  ```

### 4、.gitignore

> 参考这篇博客：[Git忽略文件.gitignore详解](https://blog.csdn.net/ThinkWon/article/details/101447866)

本项目并不是所有文件都需要保存到  Github 的，例如“build”目录及目录下的文件就可以忽略。在 Git 工作区的根目录下创建一个特殊的 .gitignore 文件，然后把要忽略的文件名填进去，Git 就会自动忽略这些文件或目录。直接用给的常用 C++ .gitignore：

```python
# Prerequisites
*.d

# Compiled Object files
*.slo
*.lo
*.o
*.obj

# Precompiled Headers
*.gch
*.pch

# Compiled Dynamic libraries
*.so
*.dylib
*.dll

# Fortran module files
*.mod
*.smod

# Compiled Static libraries
*.lai
*.la
*.a
*.lib

# Executables
*.exe
*.out
*.app

build/
.vscode/
```

**使用方式**：

* 在根目录创建 .gitignore 文本文件，把上面语句复制进来，保存。

* .gitignore 不能立刻生效，因为要忽略的文件之前已经被追踪了，需要先清空缓存：

  ```bash
  git rm -r --cached .
  ```

* 把 .gitignore 加上：

  ```bash
  git add .gitignore
  ```

* 之后看一下 VScode 的文件列表，忽略的文件是灰色，说明 .gitignore 生效了。

## 三、glog

### 1、glog 介绍

glog 即 Google Log ，是一个 Google 开源的日志库，它提供了一个轻量级的、可扩展的、跨平台的日志系统。 glog 的用法包括：



### 2、在 C++ 项目中引入glog





### 3、glog 基本使用

- **引入头文件**：需要包含 glog 的头文件：

  ```c++
  include <glog/logging.h>
  ```

- **初始化库**：在开始使用 glog 之前， 初始化库，例如：

  ```c++
  google::InitGoogleLogging(argv[0])
  ```

- **配置日志**：可以通过配置文件或代码来配置 glog 的参数，例如：

  ```c++
  google::SetLogDestination(LOG_TO_FILE, "/path/to/logfile.log")
  ```

- **输出日志**：使用 `LOG(level)` 宏函数来输出日志。`level` 表示日志的严重程度，可以是以下几个级别之一：`INFO`：一般信息、`WARNING`：警告信息、`ERROR`：错误信息、`FATAL`：致命错误信息，输出后会终止程序。例如输出一般信息：

  ```c++
  LOG(INFO) << "This is an informational message."
  ```

- **条件输出日志**：使用 `LOG_IF()`、`LOG_EVERY_N()` 和 `LOG_FIRST_N()`  宏函数来条件输出日志。例如：

  ```c++
  LOG_IF(INFO, num_cookies > 10) << "Got lots of cookies"
  ```

- **关闭日志**：在程序结束之前，关闭 glog：

  ```c++
  google::ShutdownGoogleLogging()
  ```



## 四、YMAL 

### 1、YAML 简介

YAML（YAML Ain't Markup Language）是一种轻量级的数据序列化格式，可以用于配置文件、数据交换、API请求等多种场景。它是一种简单易用的数据序列化格式，使得数据可以以人类可读的方式进行存储和传输。YAML的语法非常简单，它使用缩进和符号来表示数据结构。以下是一些YAML的基本语法：

1. **字符串**：用引号括起来的文本，例如："hello world"。

2. **数字**：没有引号的数字，例如：42。

3. **布尔值**：用 true 或 false 表示的真或假。

4. **缩进**：YAML使用缩进来表示嵌套关系，每个缩进级别用空格数表示。例如，下面的代码段表示一个包含两个列表的字典：

5. **字典/对象/键值对**：用短横线 `-` 或中括号`[]`表示的键值对的集合。例如：`{name: John, age: 30}`或`- name: John age: 30`。多层对象可表示为：

   ```yaml
   key: {key1: value1, key2: value2}
   ```

   或者

   ```yaml
   key:
     key1: value1
     key2: value2
   ```

6. **数组/列表**：用短横线 `-` 或中括号 `[]` 表示的值的列表。例如：`[apple, banana, orange]` 或 `- apple - banana - orange`。复杂一点的如：

   ```yaml
   streamers:
       - streamer:
           tag: str_gnss_rov
           output_tags: [fmt_gnss_rov]
           type: file
           path: <data-directory>/gnss_rover.bin
       - streamer:
           tag: str_gnss_ref
           output_tags: [fmt_gnss_ref]
           type: file
           path: <data-directory>/gnss_reference.bin
   ```

7. **引用**：`&` 用来建立锚点，`<<` 表示合并到当前数据，`*` 用来引用锚点。 

8. **注释**：在YAML中，使用 `#` 表示注释。

> YAML 需要特别注意的几个点：
>
> - ==大小写敏感。==
> - ==缩进不允许使用 tab，只允许空格。==
> - ==缩进的空格数不重要，只要相同层级的元素左对齐即可。==



### 2、在 C++ 项目中引入 yaml-cpp





> 链接时找不到 yaml-cpp，可以参考博客：[error while loading shared libraries的解决方案](https://blog.csdn.net/weixin_42310458/article/details/125180410)，在 `/etc/ld.so.conf` 文件中加上 `/usr/local/lib`

### 3、读取 YMAL 语法

1. YMAL 在 C++ 中以 Node 类表示。

2. **LoadFile()**：从文件中加载 YAMl 到 C++ 中 Node 对象：

   ```c++
   yaml_node = YAML::LoadFile(文件名); 
   ```

3. **[]**：Node 对象可以理解为是树形的，用中括号可以取出里面的子（树）节点，创建一个新的 Node （树）对象：

   ```c++
   YAML::Node logging_node = yaml_node["logging"];
   ```

4. **safeGet()**：第一个参数为 Node，第二个参数为关键字，判断配置文件的 Node 里有没有你要的那个关键字，有的话再把对应的值作为第三个参数返回。

   ```c++
   option_tools::safeGet(logging_node, "log_to_stderr", &FLAGS_logtostderr);
   ```

5. **checkSubOption()**：第一个参数为 Node、第二个参数为子配置选项，检查参一中是否存在参二子配置选项，如果不存在有两种处理：

   - 当参三为 true 时，LOG(FATAL) 退出程序。
   - 当参三为 false 时，LOG(INFO) 输出错误到日志文件。





## 五、Eigen

### 1、Eigen 简介





### 2、在 C++ 项目中引入 Eigen





### 3、Eigen 语法













































<!-- START doctoc -->  

<!-- END doctoc -->
