开发记录

5.17

参考Cowgod's Chip-8 Technical Reference v1.0和csdn一些教程（链接附在下面），写了chip8的基础架构和部分功能函数。

暂定显示用控制台吧。。。后期有空可能换成windowsAPI（或者直接用SFML）。

按键处理还没研究明白。

参考资料：

[https://blog.csdn.net/korekara88730/article/details/50987930](手把手教你编写游戏模拟器%252520-%252520Chip8篇%2525281%252529)

[https://blog.csdn.net/2401\_85828611/article/details/148959565](CHIP8模拟器开源项目的使用和技术文档解读)

5.20

添加了读取rom和渲染函数，渲染函数使用了ds辅助，完善了部分指令

