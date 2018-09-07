# myReactorServer: a web server based on myReactor
## myReactor: a reactor implementation

## 测试环境
* OS：Ubuntu 16.10
* 内存：8G
* CPU：I5-6300HQ

## 测试工具
* [WebBench](https://github.com/linyacool/WebServer/tree/master/WebBench)


## [测试方法](https://github.com/linyacool/WebServer/blob/master/%E6%B5%8B%E8%AF%95%E5%8F%8A%E6%94%B9%E8%BF%9B.md)


## 测试结果及分析
测试截图放在最后  

| 服务器 | 短连接QPS | 长连接QPS | 
| - | :-: | -: | 
| myReactorServer | 48845| 153570 | 
| Muduo | 46374 | 118139 | 

* analysis 1 
* analysis 2
* analysis 3


## 测试结果截图
* myReactorServer短连接测试  
![shortMyServer](https://github.com/ChyauAng/myReactor/blob/master/resources/myServer.png)
* muduo短连接测试  
![shortMuduo](https://github.com/ChyauAng/myReactor/blob/master/resources/muduo.png)
* myReactorServer长连接测试  
![keepMyServer](https://github.com/ChyauAng/myReactor/blob/master/resources/myServerK.png)
* muduo长连接测试  
![keepMuduo](https://github.com/ChyauAng/myReactor/blob/master/resources/muduoK.png)
* myReactorServer空闲负载  
* myReactorServer短连接CPU负载  
* myReactorServer长连接CPU负载  
