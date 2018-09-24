# myReactorServer: A C++ Reactor Web Server

## v1: myReactor
* a step-by-step muduo-like web server implementation 

## v2: myReactorServer
* a little step beyond muduo, add several modifications on v1 

## v3: myReactorServer with efficient buffer design and connection pool
* performance increased 1.5-fold in terms of nonpersistent connection, 8.5-fold in terms of persistent connection, comparing with [moduo](https://github.com/chenshuo/muduo), look [here](https://github.com/ChyauAng/myReactorServer/blob/master/v3性能改进评估.md)

* one eventloop per thread + thread pool
* Nignx-like connection pool
* more efficient buffer design: sds(simple dynamic string, Redis-like) + scatter IO
* more in process...look [here](https://github.com/ChyauAng/myReactorServer/blob/master/v3性能优化.md)
