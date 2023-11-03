# PIM-RoCC-Lib
API and ISA for calling PIM functions using RoCC-like instructions in C/C++



## 基本介绍
这篇文档用于定义了在C语言中调用ICRG PIM进行通用计算的ISA及编程接口。


## ISA
这里是RoCC 32位指令的定义
```
31      25  24   20  19   15   14        12   11  7    6      0
  funct7      rs2      rs1        funct3        rd      opcode          R-type
  funct7      rs2      rs1      xd xs1 xs2      rd      opcode          RoCC
```


### 指令分类
RoCC共16条指令，分为3类。
- 基本指令：最基本的指令，直接对应PIM当中的one cycle操作
- 状态机指令：较为复杂的计算指令，有固定执行pattern能够通过状态机实现的multi cycles操作
- scratchpad指令：较为复杂的指令，可编程的计算任务，需要通过PIMLC编译出的模块动态加载到scratchpad中读取的指令

### 指令定义

指令分为两大类：
直接控制执行的指令和通过scratchpad间接控制执行的指令

直接控制执行的指令分为功能型指令和计算指令。
功能型指令包括获取硬件基本规格参数，申请、释放，加载、存储，复制（移动），问询共7条指令
计算指令包括非、与、或、异或这些基本逻辑计算指令、加、减、乘（除和取余待定）、比较这些状态机（or简单scratchpad指令？）计算指令，共计8-10条指令

每条直接控制指令的对象是一条指令槽及其所控制的计算阵列

通过scratchpad间接控制执行的指令则是将内存中的复杂指令加载到scratchpad的指定位置当中并加以调用，基本可以被视为一个黑盒子函数



对于opcode，我们设定 Custom0 ~ Custom3 分别代表 8 bits, 16 bits, 32 bits, 64 bits操作数

funct7共 7 bits，其中 31-28 共 4 bits 负责定义指令，27-25 共 3 bits 控制调用的阵列数量(2^i)，可以是1 - 128个阵列

| funct7前四位 | 指令名 | 指令类型 | 指令含义 |
| :----: |:----:| :----: | :----: |
| 0000 | rccp | 基本指令 | 复制 |
| 0001 | rcnot | 基本指令 | 非 |
| 0010 | rcand | 基本指令 | 与 |
| 0011 | rcor | 基本指令 | 或 |
| 0100 | rcxor | 基本指令 | 异或 |
| 0101 | rcadd | 状态机指令 | 相加 |
| 0110 | rcsub | 状态机指令 | 相减 |
| 0111 | rcmul | 状态机指令 | 相乘 |
| 1000 | rcinst0 | scratchpad指令 | 执行scrachpad 0号位中缓存的指令 |
| 1001 | rcinst1 | scratchpad指令 | 执行scrachpad 1号位中缓存的指令 |
| 1010 | rcinst2 | scratchpad指令 | 执行scrachpad 2号位中缓存的指令 |
| 1011 | rcinst3 | scratchpad指令 | 执行scrachpad 3号位中缓存的指令 |
| 1100 | rcinst4 | scratchpad指令 | 执行scrachpad 4号位中缓存的指令 |
| 1101 | rcinst5 | scratchpad指令 | 执行scrachpad 5号位中缓存的指令 |
| 1110 | rcinst6 | scratchpad指令 | 执行scrachpad 6号位中缓存的指令 |
| 1111 | rcld | scratchpad指令 | 将指令加载到scrachpad当中 |



## PIM Library Interface
这里定义了一系列接口函数，这些函数通过内联汇编实现调用RoCC格式的汇编代码，提供基于向量计算的PIM调用接口。

### 接口函数一览
| 功能 | 函数定义 | 实现优先级 | 输入数量 | 输入类型 | 结果类型 | 详细介绍 |
| :----: |:----:| :----: |:----:|:----:|:----:| :----: |
| 非（取反）    |  ROCC_NOT     | 优先  | 1 | 任意 | 任意 |  对输入的向量中的每个元素取反 |
| 整型转浮点*   |  ROCC_INT2FLOAT    | 次优  | 1 | 整型 | 浮点型 |  将整型向量转为浮点型向量 |
| 浮点转整型*   |  ROCC_FLOAT2INT    | 次优  | 1 | 浮点型 | 整型 |  将浮点型向量转为整型向量 |
| 整型位宽转换* |  ROCC_ADJUSTINTWIDTH  |   次优  | 1 | 任意 | 任意 |  改变输入的整型向量位宽 |
| 平方          | ROCC_SQUARE_INT    | 优先  | 1 | 整型 | 整型 |  对输入的整型向量求平方 |0
| 平方根        | ROCC_SQUAREROOT_INT   | 优先  | 1 | 整型 | 整型 |  对输入的整型向量求平方根 |
| 三角函数      | ROCC_SIN    | 补充 | 1 | 浮点型 | 浮点型 |  对输入的整型/浮点型向量求三角函数 |
| 对数          | ROCC_LOG    | 补充  | 1 | 浮点型 | 浮点型 |  对输入的整型/浮点型向量求对数函数 |
| n次方         | ROCC_POWER      | 补充  | 2 | 整型/浮点型+整型 | 任意 |  对输入的整型/浮点型向量求n次方 |
| 移位          | ROCC_SHIFT      | 次优  | 2 | 任意+整型 | 任意 |  将输入的向量移位 |
| 与            | ROCC_AND    | 优先  | 2 | 任意 | 任意 |  将输入的两对向量相与 |
| 或            | ROCC_OR     | 优先  | 2 | 任意 | 任意 |  将输入的两对向量相或 |
| 异或          | ROCC_XOR    | 优先  | 2 | 任意 | 任意 |  将输入的两对向量相异或 |
| 较大          | ROCC_MAX_INT    | 优先  | 2 | 整型 | 整型 |  在输入的两对整型向量中取较大值 |
| 较小          | ROCC_MIN_INT    | 优先  | 2 | 整型 | 整型 |  在输入的两对整型向量中取较小值 |
| 比较          | ROCC_COMP_INT   | 优先  | 2 | 整型 | 整型 |  对输入的两对整型向量逐个比较 |
| 加法          | ROCC_ADD_INT    | 优先  | 2 | 整型 | 整型 |  将输入的两对整型向量相加 |
| 减法          | ROCC_SUB_INT    | 优先  | 2 | 整型 | 整型 |  将输入的两对整型向量相减 |
| 乘法          | ROCC_MUL_INT    | 优先  | 2 | 整型 | 整型 |  将输入的两对整型向量相乘 |
| 除法          | ROCC_DIV_INT    | 优先  | 2 | 整型 | 整型 |  将输入的两对整型向量相除 |
| 取余          | ROCC_MOD_INT    | 优先  | 2 | 整型 | 整型 |  将输入的两对整型向量取余 |
| 浮点比较      | ROCC_COMP_FLOAT     | 补充  | 2 | 浮点型 | 浮点型 |  对输入的两对浮点型向量逐个比较 |
| 浮点加法      | ROCC_ADD_FLOAT      | 补充  | 2 | 浮点型 | 浮点型 |  将输入的两对浮点型向量相加 |
| 浮点减法      | ROCC_SUB_FLOAT      | 补充  | 2 | 浮点型 | 浮点型 |  将输入的两对浮点型向量相减 |
| 浮点乘法      | ROCC_MUL_FLOAT      | 补充  | 2 | 浮点型 | 浮点型 |  将输入的两对浮点型向量相乘 |
| 浮点除法      | ROCC_DIV_FLOAT      | 补充  | 2 | 浮点型 | 浮点型 |  将输入的两对浮点型向量相除 |


### 接口函数分类

从输入向量的个数来说，可以分为单输入和两输入两种；
从数据类型来看，可以分为无视类型的计算、整型计算和浮点型计算和类型转换四种。无视类型的计算主要为逻辑运算，对任意类型的数据的比特位做相同处理，不在意输入或输出的具体类型；而整型计算一般以整型数据为输入和输出；浮点型计算可以同时以整型及浮点型为输入，整型输入会默认被转换为浮点型输入，统一为以浮点型为输入且以浮点型为输出的计算；最后，类型转换类函数一般设计数据类型和数据宽度的变化。

目前，整型固定为32位int型，浮点型固定为32位float型。


### 函数详解

#### ROCC_NOT

ROCC_NOT(src_addr, dest_addr, length, bits)
- src_addr:     输入向量起始地址
- dest_addr:    输出向量起始地址
- length:       向量长度
- bits:         向量中单个元素位宽


#### ROCC_INT2FLOAT

ROCC_INT2FLOAT(src_addr, dest_addr, length)
- src_addr:     输入整型向量起始地址
- dest_addr:    输出浮点型向量起始地址
- length:       向量长度


#### ROCC_FLOAT2INT

ROCC_FLOAT2INT(src_addr, dest_addr, length)
- src_addr:     输入浮点型向量起始地址
- dest_addr:    输出整型向量起始地址
- length:       向量长度


#### ROCC_ADJUSTINTWIDTH

ROCC_ADJUSTINTWIDTH(src_addr, dest_addr, length, src_bits, dest_bits)
- src_addr:     输入整型向量起始地址
- dest_addr:    输出整型向量起始地址
- length:       向量长度
- src_bits:     输入向量中单个元素位宽
- dest_bits:    输出向量中单个元素位宽


#### ROCC_SQUARE_INT, ROCC_SQUAREROOT_INT

ROCC_SQUARE_INT(src_addr, dest_addr, length)
ROCC_SQUAREROOT_INT(src_addr, dest_addr, length)
- src_addr:     输入整型向量起始地址
- dest_addr:    输出整型向量起始地址
- length:       向量长度


#### ROCC_SIN, ROCC_LOG

ROCC_SIN(src_addr, dest_addr, length)
ROCC_LOG(src_addr, dest_addr, length)
- src_addr:     输入浮点型向量起始地址
- dest_addr:    输出浮点型向量起始地址
- length:       向量长度


#### ROCC_SHIFT

ROCC_POWER(src1_addr, src2, dest_addr, length)
- src1_addr:    输入1整型/浮点型向量起始地址
- src2:         输入2整型（每个元素n次方中的n相同）
- dest_addr:    输出整型/浮点型向量起始地址
- length:       向量长度

ROCC_POWER(src1_addr, src2_addr, dest_addr, length)
- src1_addr:    输入1整型/浮点型向量起始地址
- src2_addr:    输入2整型向量起始地址
- dest_addr:    输出整型/浮点型向量起始地址
- length:       向量长度


#### ROCC_SHIFT

ROCC_SHIFT(src1_addr, src2, dest_addr, length, direction, cyclic, bits)
- src1_addr:    输入1向量起始地址
- src2:         输入2整型（每个元素移动n位次方中的n相同）
- dest_addr:    输出向量起始地址
- length:       向量长度
- direction:    移位方向
- cyclic：      是移位否循环
- bits:         向量中单个元素位宽

ROCC_SHIFT(src1_addr, src2_addr, dest_addr, length, direction, cyclic, bits)
- src1_addr:    输入1向量起始地址
- src2_addr:    输入2向量起始地址
- dest_addr:    输出向量起始地址
- length:       向量长度
- direction:    移位方向
- cyclic：      是移位否循环
- bits:         向量中单个元素位宽


#### ROCC_AND, ROCC_OR, ROCC_XOR
ROCC_AND(src1_addr, src2_addr, dest_addr, length, bits)
ROCC_OR(src1_addr, src2_addr, dest_addr, length, bits)
ROCC_XOR(src1_addr, src2_addr, dest_addr, length, bits)

- src1_addr:    输入1向量起始地址
- src2_addr:    输入2向量起始地址
- dest_addr:    输出向量起始地址
- length:       向量长度
- bits:         向量中单个元素位宽


#### ROCC_MAX_INT, ROCC_MIN_INT, ROCC_ADD_INT, ROCC_SUB_INT, ROCC_ADD_INT, ROCC_MUL_INT, ROCC_DIV_INT, ROCC_MOD_INT

ROCC_MAX_INT(src1_addr, src2_addr, dest_addr, length)
ROCC_MIN_INT(src1_addr, src2_addr, dest_addr, length)
ROCC_ADD_INT(src1_addr, src2_addr, dest_addr, length)
ROCC_SUB_INT(src1_addr, src2_addr, dest_addr, length)
ROCC_ADD_INT(src1_addr, src2_addr, dest_addr, length)
ROCC_MUL_INT(src1_addr, src2_addr, dest_addr, length)
ROCC_DIV_INT(src1_addr, src2_addr, dest_addr, length)
ROCC_MOD_INT(src1_addr, src2_addr, dest_addr, length)
- src1_addr:    输入1整型向量起始地址
- src2_addr:    输入2整型向量起始地址
- dest_addr:    输出整型向量起始地址
- length:       向量长度


#### ROCC_MAX_FLOAT, ROCC_MIN_FLOAT, ROCC_ADD_FLOAT, ROCC_SUB_FLOAT, ROCC_ADD_FLOAT, ROCC_MUL_FLOAT, ROCC_DIV_FLOAT

ROCC_MAX_FLOAT(src1_addr, src2_addr, dest_addr, length)
ROCC_MIN_FLOAT(src1_addr, src2_addr, dest_addr, length)
ROCC_ADD_FLOAT(src1_addr, src2_addr, dest_addr, length)
ROCC_SUB_FLOAT(src1_addr, src2_addr, dest_addr, length)
ROCC_ADD_FLOAT(src1_addr, src2_addr, dest_addr, length)
ROCC_MUL_FLOAT(src1_addr, src2_addr, dest_addr, length)
ROCC_DIV_FLOAT(src1_addr, src2_addr, dest_addr, length)
ROCC_MOD_FLOAT(src1_addr, src2_addr, dest_addr, length)
- src1_addr:    输入1浮点型向量起始地址
- src2_addr:    输入2浮点型向量起始地址
- dest_addr:    输出浮点型向量起始地址
- length:       向量长度

