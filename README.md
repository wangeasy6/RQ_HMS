# RQ_HMS
## 采用树莓派3B和QT做的一个智能家居监控系统

* /Linux 目录存放服务器（树莓派）的代码
* /QT 目录存放客户端（PC）的代码

## 物理连接

* 树莓派 GPIO_1 连接 DHT11（使用 WiringPi库）
* 树莓派 GPIO_4 连接 红外
* 树莓派 GPIO_5 连接 蜂鸣器
* 树莓派 USB2.0 连接 摄像头
* 客户端 通过网线或无线连接树莓派（服务器端）

### 已完成：

* 读取dht11
* 读取门禁
* 多连接支持，pthread & mutex
* QT基本界面完成

### 未完成：

* 用户登录
* 视频 + 优化
* 报警声
* 日志（双向传输 & 数据库）
* 常用IP下拉栏 + IP输入格式限制
* 录制视频 + 回看功能
* 温湿度值方差校准
* 短信 + 设置主机号码
* UI美化

## 可升级

* 红外传感器受环境光影响，可替换为微波感应开关/人体接近传感器
* USB摄像头换为专业摄像头，使采集点变多，且传输距离更远
* app客户端

# RQ_HMS
## Made a home monitoring system with Raspberry Pi and QT.

* /linux store the code on the Raspberry Pi.
* /QT shore the code on the QT.

## Raspberry Pi physical connection:

* DTH11 sensor monitor temperature and humidity,Connected to WiringPi_GPIO_1
* Infrared sensor monitor pass and into,Connected to WiringPi_GPIO_4
* Camera connected to USB.

## The system is still under development
