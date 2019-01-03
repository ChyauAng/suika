# suika

## v1: muduo++
* a step-by-step muduo-like web server implementation 

## v2: suika
* a little step beyond muduo, add several modifications on v1 

## v3: suika with efficient buffer design and connection pool
* performance is significantly improved over Webbench compared with [moduo](https://github.com/chenshuo/muduo), look [here](https://github.com/ChyauAng/suika/blob/master/v3/v3性能评估.md)

* one eventloop per thread + thread pool
* Nignx-like connection pool
* more efficient buffer design: sds(simple dynamic string, Redis-like) + scatter IO
* more in process...look [here](https://github.com/ChyauAng/suika/blob/master/v3/README.md)
