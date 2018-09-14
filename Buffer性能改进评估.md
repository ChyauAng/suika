## 评估环境
* OS：Ubuntu 14.04
* 内存：8G
* CPU：I7-4930K

## 评估工具
* [WebBench](https://github.com/linyacool/WebServer/tree/master/WebBench)


## [评估方法](https://github.com/linyacool/WebServer/blob/master/%E6%B5%8B%E8%AF%95%E5%8F%8A%E6%94%B9%E8%BF%9B.md)


## 评估结果
测试截图放在最后  

| 服务器 | 短连接QPS | 长连接QPS | 
| - | :-: | -: | 
| myReactorServer | 44869| 1992132 | 
| muduo | 44771 | 1437426 | 


## 评估结果截图
* myReactorServer短连接  
![shortMyServer](https://github.com/ChyauAng/myReactorServer/blob/master/resources/buffer/myServer-nk.png)
* muduo短连接  
![shortMuduo](https://github.com/ChyauAng/myReactorServer/blob/master/resources/buffer/muduo-nk.png)
* myReactorServer长连接  
![keepMyServer](https://github.com/ChyauAng/myReactorServer/blob/master/resources/buffer/myServer-k.png)
* muduo长连接  
![keepMuduo](https://github.com/ChyauAng/myReactorServer/blob/master/resources/buffer/muduo-k.png)
