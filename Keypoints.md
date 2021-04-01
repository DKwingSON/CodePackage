# 分布式架构与PHP

## 无状态服务于有状态服务

**无状态服务（Stateless Service）：**

是指该服务运行的实例不会在本地存储需要持久化的数据，并且多个实例对于同一个请求响应的结果是完全一致的。这类服务在网易蜂巢云平台创建后，借助k8s内部的负载均衡，当访问该服务的请求到达服务一端后，负载均衡会随机找到一个实例来完成该请求的响应（目前为轮询）。这类服务的实例可能会因为一些原因停止或者重新创建（如扩容时），这时，这些停止的实例里的所有信息（除日志和监控数据外）都将丢失(重启容器即会丢失)。因此如果您的容器实例里需要保留重要的信息，并希望随时可以备份以便于以后可以恢复的话，那么建议您创建有状态服务。

**有状态服务（Stateful Service）:**

是指该服务的实例可以将一部分数据随时进行备份，并且在创建一个新的有状态服务时，可以通过备份恢复这些数据，以达到数据持久化的目的。有状态服务只能有一个实例，因此不支持“自动服务容量调节”。一般来说，数据库服务或者需要在本地文件系统存储配置文件或其它永久数据的应用程序可以创建使用有状态服务。

## 分布式一致性算法 Raft

<https://zhuanlan.zhihu.com/p/32052223>

## 怎么设计一个高可用系统

1.  流量切分（集群化与灾备）

    1.  垂直切分

    2.  水平切分

    3.  服务状态考虑

        1.  多机无状态

        2.  有状态

2.  分层隔离

    1.  模块化（高内聚，低耦合）

    2.  读写分离

    3.  轻重分离

3.  系统自我保护

    1.  缓存

    2.  降级

    3.  过载保护

## 消费幂等

### 什么是消息幂等

当出现消费者对某条消息重复消费的情况时，重复消费的结果与消费一次的结果是相同的，并且多次消费并未对业务系统产生任何负面影响，那么这整个过程就可实现消息幂等。

例如，在支付场景下，消费者消费扣款消息，对一笔订单执行扣款操作，扣款金额为 100
元。如果因网络不稳定等原因导致扣款消息重复投递，消费者重复消费了该扣款消息，但最终的业务结果是只扣款一次，扣费
100
元，且用户的扣款记录中对应的订单只有一条扣款流水，不会多次扣除费用。那么这次扣款操作是符合要求的，整个消费过程实现了消费幂等。

## 什么是服务注册与服务发现？

**服务注册**，就是将提供某个服务的模块信息(通常是这个服务的IP和端口)注册到1个公共的组件上去（比如:
zookeeper\\consul）。

**服务发现**，就是新注册的这个服务模块能够及时的被其他调用者发现。不管是服务新增和服务删减都能实现自动发现。

你可以理解为：

//服务注册

NameServer-\>register(newServer);

//服务发现

NameServer-\>getAllServer();

应用场景：**各个微服务相互独立，每个微服务，由多台机器或者单机器不同的实例组成，各个微服务之间错综复杂的相互关联调用**。

任何情况下，处理器检测到event发生，通过异常表（exception
table）跳转到专门处理这类事件的操作系统子程序（exception handler）。

字节跳动后台：tcp连接在网络不好的情况下一直重传怎么办（采用同步队列，发一个确认一个）；

![](media/b43b1cb18da44ec34007d6937aff06bf.png)

客户端还必须等待 **2MSL**
个时间，这里为什么客户端还不能直接跑路呢？主要是为了防止发送出去的 **ACK**
服务端没有收到，服务端重发 **FIN**
再次来询问，如果客户端发完就跑路了，那么服务端重发的时候就没人理他了。这个等待的时间长度也很讲究。

主动关闭的一方发出 **FIN** 包（Client），被动关闭（Server）的一方响应 **ACK**
包，此时，被动关闭的一方就进入了 **CLOSE_WAIT**
状态。如果一切正常，稍后被动关闭的一方也会发出 **FIN** 包，然后迁移到
**LAST_ACK** 状态。

## 分布一致性算法：

Raft， Paxos

## Sentinel系统

### **Sentinel集群故障检测**

1) 每1秒，向主服务器，从服务器，其他sentinel实例发送ping命令  
有效回复：+PONG, -Loading,+MASTERDOWN三种回复一种  
无效回复：除以上三种回复之外的回复，或者在指定时限内没有返回的回复  
Sentinel.conf -\> Sentinel down-master-millsenconds master 50000  
(当连续50秒，sentinel都接收到无效请求或者无回复时，就会将master标记为主观下线)  
2)
主观下线之后，向其他sentinel发送询问命令，如果达到配置中指定的数量时，则标记master为客观下线

### **Sentinel集群故障转移**

1）选出一台Sentinel-leader，来进行故障转移操作（raft协议，过半选举）  
if (winner && (max_votes \< voters_quorum \|\| max_votes \< master-\>quorum))  
2）领头sentinel在已下线的从服务器里面，挑选一个从服务器，并将其转换为主服务器  
3）让已下线主服务器属下的所有从服务器改为复制新的主服务器  
4）将已下线主服务器设置为新的主服务器的从服务器，当这个旧的主服务器重新上线

如何选择？服务器优先级（取最高），复制偏移量（取最大），运行ID（取最小）

## 脑裂（Split-Brain）

集群中部分节点之间不可达,不同分裂的小集群会自主的选择出master节点，造成原本的集群会同时存在多个master节点。广义地解决Split-Brain的问题，一般有3种方式:

1\. Quorums，过半选举的方式

2\. 采用Redundant
communications，冗余通信的方式，集群中采用多种通信方式，防止一种通信方式失效导致集群中的节点无法通信。

3\. Fencing,
共享资源的方式，比如能看到共享资源就表示在集群中，能够获得共享资源的锁的就是Leader，看不到共享资源的，就表示本身结点出现问题，则停止服务。

<https://leetcode-cn.com/problems/create-maximum-number/solution/yi-zhao-chi-bian-li-kou-si-dao-ti-ma-ma-zai-ye-b-7/>

## 静态资源上传CDN后，向CDN访问该资源怎么做？

资源上传CDN之后，当用户访问CDN的资源地址之后会经历下面的步骤：

首先经过本地的DNS解析，请求cname指向的那台CDN专用的DNS服务器。

DNS服务器返回全局负载均衡的服务器IP给用户

用户请求全局负载均衡服务器，服务器根据IP返回所在区域的负载均衡服务器IP给用户

用户请求区域负载均衡服务器，负载均衡服务器根据用户IP选择距离近的，并且存在用户所需内容的，负载比较合适的一台缓存服务器IP给用户。当没有对应内容的时候，会去上一级缓存服务器去找，直到找到资源所在的源站服务器，并且缓存在缓存服务器中。用户下一次在请求该资源，就可以就近拿缓存了。

## Nginx模式的常见复杂均衡方式

1.  轮询（默认）   
    每个请求按时间顺序逐一分配到不同的后端服务器，如果后端服务器down掉，能自动剔除。

    1.  weight   
        指定轮询几率，weight和访问比率成正比，用于后端服务器性能不均的情况。

    2.  hash分配 参照一致性哈希

    3.  低响应时间优先

## PHP魔术方法

\_\_construct()

\_\_desctruct()

\_\_get()

\_\_set()

\_\_toString()

\_\_clone()

\_\_call()

# C++

## 泛型困境

我们以 C、C++ 和 Java 为例，介绍它们在设计上的不同考量：

-   C
    语言是系统级的编程语言，它没有支持泛型，本身提供的抽象能力非常有限。这样做的结果是牺牲了程序员的开发效率，与
    Go
    语言目前的做法一样，它们都需要手动实现不同类型的相同逻辑。但是不引入泛型的好处也显而易见
    —— 降低了编译器实现的复杂度，也能保证源代码的编译速度；

-   C++ 与 C
    语言的选择完全不同，它使用编译期间类型特化实现泛型，提供了非常强大的抽象能力。虽然提高了程序员的开发效率，不再需要手写同一逻辑的相似实现，但是编译器的实现变得非常复杂，泛型展开会生成的大量重复代码也会导致最终的二进制文件膨胀和编译缓慢，我们往往需要链接器来解决代码重复的问题；

-   Java 在 1.5 版本引入了泛型，它的泛型是用类型擦除实现的。Java
    的泛型只是在编译期间用于检查类型的正确，为了保证与旧版本 JVM
    的兼容，类型擦除会删除泛型的相关信息，导致其在运行时不可用。编译器会插入额外的类型转换指令，与
    C 语言和 C++ 在运行前就已经实现或者生成代码相比，Java
    类型的装箱和拆箱会降低程序的执行效率6；

## 在一台内存为2G的机器上，malloc(20G) 会怎样？如果是new(20G) 会怎样？

-   首先，malloc和new申请的都是虚拟内存

-   malloc函数的实质，有一个将可用的内存块连接起来的链表，调用malloc的时候，会沿着链表找一个满足用户需求的内存块。然后将这个内存块一分为二，一块和用户所申请的内存大小相同，另一块返回到链表中。如果用户申请一个大的内存块，空闲链表上可能没有可以满足用户要求的片段，这个时候malloc函数就会请求延时，对链表上的内存进行整理。如果还是不可以的话，
    内存申请失败，返回NULL

-   每个进程允许的虚拟内存是4G，如果是20G的话，已经超出了**4G**这个上限，无法申请。

-   new的话，底层实现还是malloc，在分配失败的时候会抛出bad_alloc类型的异常

## Functor 仿函数

所谓仿函数，是定义了operator()的对象，下面这个例子：

FunctionObject fo;

fo();

其中表达式fo()是调用仿函数fo的operator()。而非调用函数fo().

1.  仿函数是对象，可以拥有成员函数和成员变量，即仿函数拥有状态(states)

2.  每个仿函数都有自己的类型

3.  仿函数通常比一般函数快（很多信息编译期确定）

**函数指针**是指向函数的指针变量，本质是一个指针。而**指针函数**是返回值为指针的一个函数，本质是一个函数。**仿函数**实质上是一个重载了（）运算符的对象；

**Lambda函数：编译器会把一个lambda表达式生成一个匿名类的匿名对象，并在类中重载函数调用运算符。（实质上是一个functor）**

## New, Delete：

1.  调用指向对象的析构函数，对打开的文件进行关闭。

2.  通过上面提到的标准库函数 operator delete
    来释放该对象的内存，传入函数的参数为对象地址，（e.g. 0x007d290）。

3.  NEW关键字反向思考，先调用申请内存，然后在内存上调用构造函数

## C++的fork()? Unix下的函数

-   fork()系统调用是Unix下以自身进程创建子进程的系统调用，一次调用，两次返回，如果返回是0，则是子进程，如果返回值\>0，则是父进程（返回值是子进程的pid），这是众为周知的。

-   还有一个很重要的东西是，在fork()的调用处，整个父进程空间会原模原样地复制到子进程中，包括指令，变量值，程序调用栈，环境变量，缓冲区，等等。本质上复制了**页表**

## C++ 如何申请和释放一个数组？

我们经常要用到动态分配一个数组，也许是这样的：

**string \*psa = new string[10]; //array of 10 empty strings**

**int \*pia = new int[10]; //array of 10 uninitialized ints**

上面在申请一个数组时都用到了 new [] 这个表达式来完成，按照我们上面讲到的 new 和
delete 知识，第一个数组是 string 类型，分配了保存对象的内存空间之后，将调用
string
类型的默认构造函数依次初始化数组中每个元素；第二个是申请具有内置类型的数组，分配了存储
10 个 int 对象的内存空间，但并没有初始化。

如果我们想释放空间了，可以用下面两条语句：

**delete [] psa;**

**delete [] pia;**

都用到 delete [] 表达式，注意这地方的 [] 一般情况下不能漏掉,
用来去内置类型数组长度！我们也可以想象这两个语句分别干了什么：第一个对 10 个
string
对象分别调用析构函数，然后再释放掉为对象分配的所有内存空间；第二个因为是内置类型不存在析构函数，直接释放为
10 个 int 型分配的所有内存空间。

-   **new/delete 、new []/delete[] 要配对使用**

## [C++的四种cast操作符的](https://www.cnblogs.com/welfare/articles/336091.html)

### C风格转换

改变一个变量的类型为别的类型从而改变该变量的表示方式。为了类型转换一个简单对象为另一个对象你会使用传统的类型转换操作符。比如，为了转换一个类型为doubole的浮点数的指针到整型,代码:

int i;

double d;

i = (int) d; //或者：

i = int (d);

### C++风格转换

ANSI-C++标准定义了四个新的转换符：'reinterpret_cast', 'static_cast',
'dynamic_cast' 和 'const_cast'，目的在于控制类(class)之间的类型转换。

| 转换符                        | 定义                                                                                                                                                                                                | 应用场景                                                                                                                                                                                                                 |
|-------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| reinterpret_cast\<new_type\>  | 'reinterpret_cast'转换一个指针为其它类型的指针。它也允许从一个指针转换为整数类型。反之亦然。                                                                                                        | 用于底层的强制转型。这样的强制转型在底层代码以外应该极为罕见。                                                                                                                                                           |
| dynamic_cast\<new_type\>      | 'static_cast'允许执行任意的隐式转换和相反转换动作。（即使它是不允许隐式的） 在这例子里，被转换的父类没有被检查是否与目的类型相一致。代码：                                                          | 主要用于执行“安全的向下转型（safe downcasting）”，也就是说，要确定一个对象是否是一个继承体系中的一个特定类型。它是唯一不能用旧风格语法执行的强制转型，也是唯一可能有重大运行时代价的强制转型。                           |
| static_cast\<new_type\>       | 'dynamic_cast'只用于对象的指针和引用。当用于多态类型时，它允许任意的隐式类型转换以及相反过程。不过，与static_cast不同，在后一种情况里（注：即隐式转换的相反过程），dynamic_cast会检查操作是否有效。 | 可以被用于强制隐型转换（例如，non-const 对象转型为 const 对象，int 转型为 double，等等），它还可以用于很多这样的转换的反向转换（例如，void\* 指针转型为有类型指针，基类指针转型为派生类指针），它最接近于C-style的转换。 |
| const_cast\<new_type\>        | 这个转换类型操纵传递对象的const属性，或者是设置或者是移除                                                                                                                                           | 强制消除对象的常量性。它是唯一能做到这一点的 C++ 风格的强制转型                                                                                                                                                          |

## C++程序内存布局

![](media/dee288ce82c1b184d6405f986e86ae3e.png)

Sizeof单位：byte

## C++ STL allocator：

要点：存在两级配置器，分别是malloc/free组合 （\>128bytes）, 以及自由从堆中申请

![](media/5d5174bb6c9e242642602c69295285bf.jpeg)

## C++中的null与nullptr

-   \#define NULL ((void \*)0)

虽然上面我们说明了0比NULL可以让我们更加警觉，但是我们并没有避免这个问题。这个时候C++
11的nullptr就很好的解决了这个问题，我们在C++
11中使用nullptr来表示空指针，这样最早的代码是这样的，

## C++类三种方式控制权限（与Java中的三种对应权限一样）

**public、protected、private三种继承方式，相当于把父类的public访问权限在子类中变成了对应的权限。**
如protected继承，把父类中的public成员在本类中变成了protected的访问控制权限；private继承，把父类的public成员和protected成员在本类中变成了private访问控制权。

| **访问权限** | **public** | **protected** | **private** |
|--------------|------------|---------------|-------------|
| 对本类       | 可见       | 可见          | 可见        |
| 对子类       | 可见       | 可见          | 不可见      |
| 外部(调用方) | 可见       | 不可见        | 不可见      |

## C++11 自动推断类型特性

decltype和auto都可以用来推断类型，但是二者有几处明显的差异：1.auto忽略顶层const，decltype保留顶层const；2.对引用操作，auto推断出原有类型，decltype推断出引用；3.对解引用操作，auto推断出原有类型，decltype推断出引用；4.auto推断时会实际执行，decltype不会执行，只做分析。总之在使用中过程中和const、引用和指针结合时需要特别小心。

## C++的String深入讨论（百度2021秋招）

1.  string共享内存：本质上来说，string返回一个const
    char\*，用户记录从堆上分配内存的地址，其在构造时分配内存，在析构时释放内存，想更改该地址只能通过string提供的接口；string
    a=b;其实是共享了同一个指针，从而共享同一块内存；

2.  写时拷贝Copy-On-Write：

![](media/810d81003d7a692d67555d27fb981961.png)

底层如何实现？维护一个引用计数，来决定发生Copy-On-Write的同时应不应该释放旧的内存空间；

## C++中STL的map不同插入模式的性能对比

1．使用中括号插入m[key] = value; //本质先置key对应value为默认再置为实际值

2．使用insert，直接插入，效率高一些

## STL中的unordered\_map怎么避免冲突？

**链地址法**

## C++智能指针：

| **指针类别** | **支持**     | **备注**                              |
|--------------|--------------|---------------------------------------|
| unique_ptr   | C++ 11       | 拥有独有对象所有权语义的智能指针      |
| shared_ptr   | C++ 11       | 拥有共享对象所有权语义的智能指针      |
| weak_ptr     | C++ 11       | 到 std::shared_ptr 所管理对象的弱引用 |
| auto_ptr     | C++ 17中移除 | 拥有严格对象所有权语义的智能指针      |

主要职能指针分为两个unique_ptr和shared_ptr

std::unique_ptr 是通过指针占有并管理另一对象，并在 unique_ptr
离开作用域时释放该对象的智能指针。在下列两者之一发生时用关联的删除器释放对象：

-   销毁了管理的 unique_ptr 对象

-   通过 operator= 或 reset() 赋值另一指针给管理的 unique_ptr 对象。

    unique_ptr可以用=号赋值吗？ 不可。

std::shared_ptr 是通过指针保持对象共享所有权的智能指针。多个 shared_ptr
对象可占有同一对象。下列情况之一出现时销毁对象并解分配其内存：

-   最后剩下的占有对象的 shared_ptr 被销毁；

-   最后剩下的占有对象的 shared_ptr 被通过 operator= 或 reset() 赋值为另一指针。

主要解决内存泄漏

shared_ptr都会问维护的对象是不是线程安全的，然后问引用计数是不是线程安全的。引用计数源码使用原子操作实现，肯定是线程安全的。

### C++引用计数（智能指针相关）

![](media/950849bfe8a36e1d3af350bd8239ce1b.png)

## C++ 中的存储周期：

automatic

static

dynamic

thread

thread_local
关键字修饰的变量具有线程（thread）周期，这些变量在线程开始的时候被生成，在线程结束的时候被销毁，并且每一个线程都拥有一个独立的变量实例。

## C++函数模板的实现

编译器并不是把函数模板处理成能够处理任意类的函数；编译器从函数模板通过具体类型产生不同的函数；编译器会对函数模板进行两次编译：在声明的地方对模板代码本身进行编译，在调用的地方对参数替换后的代码进行编译。

## C++内存布局

![](media/f3189547daa4548bc4ab6fdd13eeea6d.png)

# 计算机网络

## Http与Https的区别

HTTPS和HTTP的区别主要如下：

1、https协议需要到ca申请证书，一般免费证书较少，因而需要一定费用。

2、http是超文本传输协议，信息是明文传输，https则是具有安全性的ssl加密传输协议。

3、http和https使用的是完全不同的连接方式，用的端口也不一样，前者是80，后者是443。

4、http的连接很简单，是无状态的；HTTPS协议是由SSL+HTTP协议构建的可进行加密传输、身份认证的网络协议，比http协议安全。

## 路由与交换的不同

路由作用于不同的子网之间，交换作用于同一个子网内。路由作用于网络层，交换作用于数据链路层

## HTTP-断点续传实现原理

断点续传是下载文件经常要用到的功能。主要原理是是HTTP1.1（RFC2616）中定义header中定义的*Range*和*contentRange*字段。

Range

用于请求头中，指定第一个字节的位置和最后一个字节的位置，一般格式：

Range:(unit=first byte pos)-[last byte pos]

Content-Range

用于响应头，指定整个实体中的一部分的插入位置，他也指示了整个实体的长度。在服务器向客户返回一个部分响应，它必须描述响应覆盖的范围和整个实体长度。一般格式：

Content-Range: bytes (unit first byte pos) - [last byte pos]/[entity legth]

## 为什么DNS协议使用UDP协议

DNS 使用了 UDP 协议来获取域名对应的 IP
地址，这个观点虽然没错，但是还是有一些片面，更加准确的说法其实是 DNS
**查询**在刚设计时主要使用 UDP 协议进行通信，而 TCP 协议也是在 DNS
的演进和发展中被加入到规范的：

1.  DNS 在设计之初就在区域传输中引入了 TCP 协议，在查询中使用 UDP 协议；

2.  当 DNS 超过了 512 字节的限制，我们第一次在 DNS 协议中明确了『当 DNS
    查询被截断时，应该使用 TCP 协议进行重试』这一规范；

3.  随后引入的 EDNS 机制允许我们使用 UDP 最多传输 4096 字节的数据，但是由于 MTU
    的限制导致的数据分片以及丢失，使得这一特性不够可靠；

4.  在最近的几年，我们重新规定了 DNS 应该同时支持 UDP 和 TCP 协议，TCP
    协议也不再只是重试时的选择；

## 为什么 TCP 建立连接需要三次握手

-   通过三次握手才能阻止重复历史连接的初始化；

-   通过三次握手才能对通信双方的初始序列号进行初始化；

-   讨论其他次数握手建立连接的可能性；

不使用『两次握手』和『四次握手』的原因已经非常清楚了：

-   『两次握手』：无法避免历史错误连接的初始化，浪费接收方的资源；

-   『四次握手』：TCP 协议的设计可以让我们同时传递 ACK 和 SYN
    两个控制信息，减少了通信次数，所以不需要使用更多的通信次数传输相同的信息；

## 网络地址分类

![](media/8afe558cb0c8fc7b44275a71cb52fe90.png)

其中ABC是日常使用，D类用于组播，E类用于研究

## 网络掩码

Q:网络中想分配n台主机，怎么设定子网掩码？

A:首先计算N = log(n)向上取整，子网掩码需要最后N位0，其他位为1

Q:已知子网掩码，如何算容纳多少台主机？

A:看子网掩码二进制最后多少连续0（e.g.
N），容纳2\^N-2（除去**网络地址**和**广播地址**）

## Nagle 算法

**Nagle 算法**是一种通过减少数据包的方式提高 TCP 传输性能的算法。因为网络
带宽有限，它不会将小的数据块直接发送到目的主机，而是会在本地缓冲区中等待更多待发送的数据，这种批量发送数据的策略虽然会影响实时性和网络延迟，但是能够降低网络拥堵的可能性并减少额外开销。

## TCP 协议粘包问题

**TCP 协议粘包问题**是因为应用层协议开发者的错误设计导致的，他们忽略了 TCP
协议数据传输的核心机制 —
基于字节流，其本身不包含消息、数据包等概念，所有数据的传输都是流式的，需要应用层协议自己设计消息的边界，即消息帧（Message
Framing），我们重新回顾一下粘包问题出现的核心原因：

1.  TCP 协议是基于字节流的传输层协议，其中不存在消息和数据包的概念；

2.  应用层协议没有使用基于长度或者基于终结符的消息边界，导致多个消息的粘连；

## TCP第三次握手失败后怎么办？

当客户端收到服务端的SYN+ACK应答后，其状态变为ESTABLISHED，并会发送ACK包给服务端，准备发送数据了。如果此时ACK在网络中丢失，过了超时计时器后，那么Server端会重新发送SYN+ACK包，重传次数根/proc/sys/net/ipv4/tcp_synack_retries来指定，默认是5次。如果重传指定次数到了后，仍然未收到ACK应答，那么一段时间后，Server自动关闭这个连接。但是Client认为这个连接已经建立，如果Client端向Server写数据，Server端将以RST包响应，方能感知到Server的错误。

（RST 抵御 syn flood）

## TCP的自动重传机制（ARQ）

### stop-and-wait

停止并等待协议的工作原理如下：

1.  发送点对接收点发送数据包，然后等待接收点回复ACK并且开始计时。

2.  在等待过程中，发送点停止发送新的数据包。

3.  当数据包没有成功被接收点接收时候，接收点不会发送ACK.这样发送点在等待一定时间后，重新发送数据包。

4.  反复以上步骤直到收到从接收点发送的ACK.

发送点的等待时间应当至少大于传输点数据包发送时间（数据包容量除以发送点传输速度），接收点ACK接收时间（ACK容量除以接收点传输速度），数据在连接上的传送时间，接收点检验接收数据是否正确的时间之和。在实际应用当中，等待时间是这个和的**2到3倍**。
（只有一个计时器！！！）

这个协议的缺点是较长的等待时间导致低的数据传输速度。在**低速**传输时，对**连接频道的利用率**比较好，但是在高速传输时，频道的利用率会显著下降。

### Continuous ARQ

#### 回退N重传(Go-Back-N)

-   接收点丢弃从第一个没有收到的数据包开始的所有数据包。

-   发送点收到NACK后，从NACK中指明的数据包开始重新发送。

#### 选择重传(Selective Repeat)

-   发送点连续发送数据包但对每个数据包都设有个一个计时器。（维护多个计时器！！！）

-   当在一定时间内没有收到某个数据包的ACK时，发送点只重新发送那个没有ACK的数据包。

## TCP滑动窗口结构

![](media/9ffbd01c432a202f20518dc1b3d0f38d.png)

接收窗口的结构

**1. Received and ACK Not Send to
Process：这部分数据属于接收了数据但是还没有被上层的应用程序接收，也是被缓存在窗口内**

**2. Received Not ACK: 已经接收并，但是还没有回复ACK，这些包可能输属于Delay
ACK的范畴了**

**3. Not Received：有空位，还没有被接收的数据。**

接收端的Window
Size通告也是会变化的，接收端根据这个值来确定何时及发送多少数据，从对数据流进行流控

### 拥塞控制和流量控制的区别

拥塞控制：拥塞控制是作用于网络的，它是防止过多的数据注入到网络中，避免出现网络负载过大的情况；常用的方法就是：（
1 ）慢开始、拥塞避免（ 2 ）快重传、快恢复。

流量控制：流量控制是作用于接收者的，它是控制发送者的发送速度从而使接收者来得及接收，防止分组丢失的。

## 浏览器是如何对CA证书进行验证的？

首先我们先要知道CA证书是如何产生的，首先是申请证书的服务方S需要向第三方CA机构提供CSR（Certificate
Signing Request
证书签名请求），第三方机构收到该请求之后，会对该请求中的信息进行核对，线上线下都会进行验证，比如，提供的域名是否属于服务方，地址是否正确等等，只有第三方机构认为你提供的信息都是准确的，它就会根据（你提供的信息+CA机构的信息+公钥）生成一个信息，然后会对该信息进行一个信息摘要，然后使用CA私钥对该信息摘要进行一个数字签名。然后再将生成的信息、数字签名一起生成一个数字证书发布给服务方。

## 常见的网络协议报头

### TCP报头

![](media/aeafaedc863819d67508ecf5aa70f7dc.png)

### UDP报头

![](media/ca27e8c56eb7a19e7d8ff4680f721a96.jpeg)

### IP数据包报头

![](media/4868a2b0c9f1691bb45cf6268d2503dc.jpeg)

### **以太网的帧格式**

![image](media/d882c76c8b2c813fa4c395493ff4affa.png)

中间的46-1500字节的最终可负载长度受控于MTU

## HTTPS握手

![](media/67569009af692f757fc372b2f68ed479.png)

## TCP拥塞避免一图流

![在这里插入图片描述](media/cd5bcc48d31db168cb87d6d8e1b2f1d4.png)

关键就是SSHResh和Cwnd

## 网络协议中的缩写

| 名称 | 含义                     | 单位 |
|------|--------------------------|------|
| TTL  | 生存时间                 | 跳数 |
| MTU  | 最大传输单元，负载的上限 | Byte |
| MSL  | 报文最大生存时间         | 秒   |
| RTT  | 客户端到服务到往返时间   | 秒   |
|      |                          |      |

## Get与Post的对比

1.  Get产生一个TCP， Post产生两个TCP

    1.  Get中的参数通过url传递，Post的参数在request body里头

## HTTP Header的内容

-   公共头：地址，域名，get/post请求的标识，请求返回状态

-   请求头

-   返回头

# 操作系统

## 动态链接与静态链接的区别

把DLL看成一种仓库，它提供给你一些可以直接拿来用的变量、函数或类。

静态链接库与动态链接库都是共享代码的方式，如果采用静态链接库，则无论你愿不愿意，lib中的指令都被直接包含在最终生成的EXE文件中了。但是若使用DLL，该DLL不必被包含在最终的EXE文件中，EXE文件执行时可以“动态”地引用和卸载这个与EXE独立的DLL文件。

采用动态链接库的优点：（1）更加节省内存；（2）DLL文件与EXE文件独立，只要输出接口不变，更换DLL文件不会对EXE文件造成任何影响，因而极大地提高了**可维护性**和动态链接库的两种链接方法：

(1) 装载时动态链接(Load-time Dynamic
Linking)：这种用法的前提是在编译之前已经明确知道要调用DLL中的哪几个函数，编译时在目标文件中只保留必要的链接信息，而不含DLL函数的代码；当程序执行时，调用函数的时候利用链接信息加载DLL函数代码并在内存中将其链接入调用程序的执行空间中(全部函数加载进内存），其主要目的是便于代码共享。（动态加载程序，处在加载阶段，主要为了共享代码，共享代码内存）

(2) 运行时动态链接(Run-time Dynamic
Linking)：这种方式是指在编译之前并不知道将会调用哪些DLL函数，完全是在运行过程中根据需要决定应调用哪个函数，将其加载到内存中（只加载调用的函数进内存），并标识内存地址，其他程序也可以使用该程序，并用LoadLibrary和GetProcAddress动态获得DLL函数的入口地址。（dll在内存中只存在一份，处在运行阶段）

。

## 动态链接库的两种链接方法：

(1) 装载时动态链接(Load-time Dynamic
Linking)：这种用法的前提是在编译之前已经明确知道要调用DLL中的哪几个函数，编译时在目标文件中只保留必要的链接信息，而不含DLL函数的代码；当程序执行时，调用函数的时候利用链接信息加载DLL函数代码并在内存中将其链接入调用程序的执行空间中(全部函数加载进内存），其主要目的是便于代码共享。（动态加载程序，处在加载阶段，主要为了共享代码，共享代码内存）

(2) 运行时动态链接(Run-time Dynamic
Linking)：这种方式是指在编译之前并不知道将会调用哪些DLL函数，完全是在运行过程中根据需要决定应调用哪个函数，将其加载到内存中（只加载调用的函数进内存），并标识内存地址，其他程序也可以使用该程序，并用LoadLibrary和GetProcAddress动态获得DLL函数的入口地址。（dll在内存中只存在一份，处在运行阶段）

## linux中的根文件系统（rootfs的原理和介绍）

定义：根文件系统首先是内核启动时所mount的第一个文件系统，内核代码映像文件保存在根文件系统中，而系统引导启动程序会在根文件系统挂载之后从中把一些基本的初始化脚本和服务等加载到内存中去运行。

根文件系统包含系统启动时所必须的目录和关键性的文件，以及使其他文件系统得以挂载（mount）所必要的文件。例如：

1.  init进程的应用程序必须运行在根文件系统上；

2.  根文件系统提供了根目录“/”；

3.  linux挂载分区时所依赖的信息存放于根文件系统/etc/fstab这个文件中；

4.  shell命令程序必须运行在根文件系统上，譬如ls、cd等命令；

## 虚拟内存和swap分区的关系

**windows：虚拟内存**

**linux：swap分区**

swap类似于windows的虚拟内存，不同之处在于，Windows可以设置在windows的任何盘符下面，默认是在C盘，可以和系统文件放在一个分区里。而linux则是独立占用一个分区，方便由于内存需求不够的情况下，把一部分内容放在swap分区里，待内存有空余的情况下再继续执行，也称之为交换分区，交换空间是其中的部分

windows的虚拟内存是电脑自动设置的

Linux的swap分区是你装系统的时候分好的

## 磁盘调度的访问模型

### **FCFS–先来先服务调度算法**

原理：磁盘页面请求排成一个队列，先请求的我们先读它，后请求的我们后来再读它。

优点：公平  
缺点：效率太低

### **SSTF–最短寻道时间算法**

原理：优先选择当前距离磁头最近的请求，离我最近的我肯定访问最快。

优点：改善了平均耗时  
缺点：某些请求比如距离磁头比较远的请求可能长时间得不到响应。

### **SCAN–电梯调度算法**

原理：当有访问请求时，磁头按一个方向移动，此方向服务完毕后，转换方向，类似电梯，一次从底层到顶曾送完人，在从顶层向底层送，不会乎高乎低。磁盘也是高速旋转设备，让它停止并转换方向也是非常耗时的。

优点：寻道性能好，可避免饥饿现象。

## *磁盘预读和磁盘缓冲*

这二者都是为了提高性能和效率，如何衡量性能和效率呢，就是磁盘IO的次数，次数越少说明性能和效率越高。

**磁盘预读：**每次磁盘读取数据时，总会多读几页数据，因为存储是顺序存储的，用到此块数据很大可能会用到相邻的后几块数据，提前从磁盘读出可以减少磁盘IO次数。预读的页一般是证书呗

**磁盘缓冲：**磁盘缓冲也是，我们每次写磁盘文件时并不会立刻写入到磁盘中去，比如你给文件中一个字母a，它如果立刻写入磁盘那效率可就太低了，实际上是先写到缓冲中，等到缓冲满或者有必要时才刷新到磁盘中去。

## 僵尸进程与孤儿进程的总结

**孤儿进程：**一个父进程退出，而它的一个或多个子进程还在运行，那么那些子进程将成为孤儿进程。孤儿进程将被init进程(进程号为1)所收养，并由init进程对它们完成状态收集工作。

**僵尸进程：**一个进程使用fork创建子进程，如果子进程退出，而父进程并没有调用wait或waitpid获取子进程的状态信息，那么子进程的进程描述符仍然保存在系统中。这种进程称之为僵死进程。

Linux下查看僵尸进程：

ps -A -ostat,ppid,pid,cmd \| grep -e '\^[Zz]'

## 怎样理解阻塞非阻塞与同步异步的区别？

1.  **同步与异步**  
    同步和异步关注的是**消息通信机制** (synchronous communication/ asynchronous
    communication)

2.  **阻塞与非阻塞**  
    阻塞和非阻塞关注的是**程序在等待调用结果（消息，返回值）时的状态.**阻塞调用是指调用结果返回之前，当前线程会被挂起。调用线程只有在得到结果之后才会返回。非阻塞调用指在不能立刻得到结果之前，该调用不会阻塞当前线程。

## 惊群效应是什么

惊群效应（thundering
herd）是指多进程（多线程）在同时阻塞等待同一个事件的时候（休眠状态），如果等待的这个事件发生，那么他就会唤醒等待的所有进程（或者线程），但是最终却只能有一个进程（线程）获得这个时间的“控制权”，对该事件进行处理，而其他进程（线程）获取“控制权”失败，只能重新进入休眠状态，这种现象和性能浪费就叫做惊群效应。

单机情况下惊群效应通常发生在Linux，Nginx等系统里多个进程等待fd同时被唤醒的情况，解决办法一般是在内核或者系统里，一般不需要上层开发者去处理。分布式系统因为很高的自由度，惊群效应就需要开发者去了解和提出解决办法。

**Solve:** most unix/linux kernels serialize response to accept(2)s, in other
words, only one thread is waken up if more than one are blocking on accept(2)
against a single open file descriptor.

## **什么是线程安全？**

（2012年5月百度实习生面试）

如果多线程的程序运行结果是可预期的，而且与单线程的程序运行结果一样，那么说明是“线程安全”的。

## OS的分级保护域

![](media/42588187ec08bb993569777b8958b26e.png)

## 操作系统怎么管理，组织内存，申请一块内存的时候究竟发生了什么？

在 64 位的操作系统上，每个进程都会拥有 256 TiB
的内存空间，内核空间和用户空间分别占 128 TiB

CPU将一个虚拟内存空间中的地址转换为物理地址，需要进行两步：首先将给定一个逻辑地址（其实是段内偏移量，这个一定要理解！！！），CPU要利用其段式内存管理单元，先将为个逻辑地址转换成一个线性地址，再利用其页式内存管理单元，转换为最终物理地址。

线性地址：是CPU所能寻址的空间或者范围。  
物理地址：是机器中实际的内存地址。换言之，是机器中的内存容量范围。  
逻辑地址：是对程序而言的。一般以Seg:Offset来表示。（程序员自己看到的地址）  
因此，三者最准确的关系是：逻辑地址通过线性地址完成物理地址的映射，线性地址在三者之中完全是充当"桥"的作用。

## 操作系统从一个文件名，读取到具体的文件内容，实际干了什么事情？★★★★

>   Unix/Linux系统内部不使用文件名，而使用inode号码来识别文件。对于系统来说，文件名只是inode号码便于识别的别称或者绰号。

>   表面上，用户通过文件名，打开文件。实际上，系统内部这个过程分成三步：首先，系统找到这个文件名对应的inode号码；其次，通过inode号码，获取inode信息；最后，根据inode信息，找到文件数据所在的block，读出数据。

## 硬链接与软链接

>   一般情况下，文件名和inode号码是"一一对应"关系，每个inode号码对应一个文件名。但是，Unix/Linux系统允许，多个文件名指向同一个inode号码。

>   这意味着，可以用不同的文件名访问同样的内容；对文件内容进行修改，会影响到所有文件名；但是，删除一个文件名，不影响另一个文件名的访问。这种情况就被称为"硬链接"（hard
>   link）。

>   文件A和文件B的inode号码虽然不一样，但是文件A的内容是文件B的路径。读取文件A时，系统会自动将访问者导向文件B。因此，无论打开哪一个文件，最终读取的都是文件B。这时，文件A就称为文件B的"软链接"（soft
>   link）或者"符号链接（symbolic link）。

>   这意味着，文件A依赖于文件B而存在，如果删除了文件B，打开文件A就会报错："No
>   such file or
>   directory"。这是软链接与硬链接最大的不同：文件A指向文件B的文件名，而不是文件B的inode号码，文件B的inode"链接数"不会因此发生变化。

## **异步异常**

由事件产生，同步异常是执行一条指令的直接产物。  
类别包含**中断（异步）**，**陷阱（同步）**，**故障（同步）**，**终止（同步）**。

-   中断——异步发生，处理器IO设备信号的结果。

-   陷阱——有意的异常。最重要的用途是在用户程序和内核之间提供一个像过程一样的接口，叫做系统调用。

-   故障——潜在可恢复的错误造成的结果。如果能被修复，则重新执行引起故障的指令，否则终止。

-   终止——不可恢复的致命错误造成的结果。

## Linux系统之IO多路复用

在Linux系统中，IO多路复用有多种方法，包括select、poll和epoll，其各自的调用形式如下：

**int select(int maxfdp, fd_set \*readfds, fd_set \*writefds, fd_set \*errorfds,
struct timeval \*timeout);**

**int poll(struct pollfd \*fdarray, unsigned long nfds, int timeout);**

select方法和poll方法的性能会线性下降，因为select和poll方法会对所监控的文件句柄列表进行遍历；epoll方法依靠每个被监控的文件句柄fd上的回调函数来工作的，只有活跃的文件句柄才会调用回调函数，所以在活跃的文件句柄较少的情况下，epoll方法没有前面方法的性能急剧下降的问题，但当绝大部分被监控的文件句柄都是活跃的时候，由于每个活跃的文件句柄都会调用回调函数，因此其效率反而比不上前面的方法。

epoll方法:

**int epoll\_create(int size);**

**int epoll\_ctl(int epfd, int op, int fd, struct epoll_event \*event);**

**int epoll\_wait(int epfd, struct epoll_event \* events, int maxevents, int
timeout);**

epoll对fd的管理采用红黑树，只有当fd真的增删前才会调用epoll_ctl

## 堆与栈的区别

管理方式不同。栈OS自动分配释放；堆的申请和释放工作由程序员控制（malloc free）;

每个进程拥有的栈的大小要远远小于堆的大小。理论上，程序员可申请的堆大小为虚拟内存的大小，
64bits 的 Linux 默认 10MB；

生长方向不同。堆的生长方向向上，内存地址由低到高；栈的生长方向向下，内存地址由高到低。WHY?
这是高地址，每次压一个数据入栈，栈指针esp减去4（32位系统下），所以栈顶是向着内存低地址方向生长的。
堆一般是数据段，包括全局变量和常量等，自然和正常的代码段类似，从低地址往高地址写了;

## 内存碎片产生原因及终极解决办法

内存碎片通常分为内部碎片和外部碎片：  
1.
内部碎片是由于采用固定大小的内存分区，当一个进程不能完全使用分给它的固定内存区域时就产生了内部碎片，通常内部碎片难以完全避免；

2.
外部碎片是由于某些未分配的连续内存区域太小，以至于不能满足任意进程的内存分配请求，从而不能被进程利用的内存区域。

解决方法？

-   尽可能申请2的幂次的内存空间；

-   尽可能少申请内存；

-   尽量少使用堆上的内存空间；

使用内存池来优化内存

# 算法与数据结构

## **回调函数和钩子函数的区别**

根本上，他们都是为了捕获消息而生的，但是钩子函数在捕获消息的第一时间就会执行，而回调函数是在整个捕获过程结束时，最后一个被执行的。

## AVL树，红黑树，B树，B+树，Trie树都分别应用在哪些现实场景中？

AVL:
最早的平衡二叉树之一。应用相对其他数据结构比较少。windows对进程地址空间的管理用到了AVL树。

红黑树: 平衡二叉树，广泛用在C++的STL中。如map和set都是用红黑树实现的。

B树， B+树：用在磁盘文件组织 数据索引和数据库索引。

Trie：用在统计和排序大量字符串，如自动机。

## 海量数据排序——如果有1TB的数据需要排序，但只有32GB的内存如何排序处理？

**思路：不能内排序，那我就外排序，同时优化思路是IO尽可能小。**

**解决方案：分块，块内做内排序（qsort），产生n块有序块，然后每块按序读到内存，做块间数据归并，最后1TB有序**

扩展：海量数据排序，内存够大，怎么做？

1.  如果是无重复整型，果断位排序，（编程珠玑有介绍）。

2.  如果有重复的整型，果断计数排序，

3.  如果是字符串，果断字典树来排序

## *布隆过滤器*的原理，使用场景和注意事项

布隆过滤器是一个 bit 向量或者说 bit 数组，长这样：

![](media/ac07f0907802183ec5c94be4da15cca3.jpeg)

果我们要映射一个值到布隆过滤器中，我们需要使用**多个不同的哈希函数**生成**多个哈希值**

![](media/c1c641822c5db3b1bf06f267e41bb263.jpeg)

![](media/72657f85f84dc52044fffbf12615388c.jpeg)

传统的布隆过滤器并**不支持**删除操作

### 最佳实践

常见的适用常见有，利用布隆过滤器减少磁盘 IO
或者网络请求，因为一旦一个值必定不存在的话，我们可以不用进行后续昂贵的查询请求。另外，既然你使用布隆过滤器来加速查找和判断是否存在，那么性能很低的哈希函数不是个好选择，推荐
MurmurHash、Fnv 这些。

为什么不用hash表？hash表在海量数据情况下可能要频繁处理冲突，捞啊。

## LRU高效率算法

put 和 get 方法的时间复杂度为 O(1)，我们可以总结出 cache
这个数据结构必要的条件：查找快，插入快，删除快，有顺序之分。

Get用hash， put用链表（登记hash）

![](media/b09ffcf992c1a8f2d4a892ff08467f6a.png)

## 最小生成树算法

1.  Kruskal Algo. 对edge排序，每次选最小切不在树中的，loop

2.  Prim Algo.
    把vertex分为已选与未选两部分，开始已选={}，未选={all}，贪心选择未选vertex中距离已选vertex最近的，loop…；

## 图的最短路径算法

1.  Dijstra：迪杰斯特拉(Dijkstra)算法是典型最短路径算法，用于计算一个节点到其他节点的最短路径。
    它的主要特点是以起始点为中心向外层层扩展(广度优先搜索思想)，直到扩展到终点为止。

2.  Floyd

## 排序算法总结：

### 十种常见算法的归类

![](media/2790411914052818fd4acde9fa9e0400.png)

### 十种常见算法的复杂度对比

![](media/3f76dd789dc0b743c790c5c666a7affe.png)

# Redis

## Are Redis on-disk-snapshots atomic?

Yes, Redis background saving process is always forked when the server is outside
of the execution of a command, so every command reported to be atomic in RAM is
also atomic from the point of view of the disk snapshot.

## 缓存雪崩

缓存正常从Redis中获取，示意图如下：

![这里写图片描述](media/d3f703207c7d97a698088c1321d1bb0c.png)

缓存失效瞬间示意图如下：

![这里写图片描述](media/35a45cfad58d90852a2b213f3a4b2428.png)

还有一个简单方案就时讲缓存失效时间分散开，比如我们可以在原有的失效时间基础上增加一个随机值，比如1-5分钟随机，这样每一个缓存的过期时间的重复率就会降低，就很难引发集体失效的事件。

## 缓存穿透

缓存穿透是指用户查询数据，在数据库没有，自然在缓存中也不会有。这样就导致用户查询的时候，在缓存中找不到，每次都要去数据库再查询一遍，然后返回空（相当于进行了两次无用的查询）。这样请求就绕过缓存直接查数据库，这也是经常提的缓存命中率问题。

有很多种方法可以有效地解决缓存穿透问题，最常见的则是采用布隆过滤器，将所有可能存在的数据哈希到一个足够大的bitmap中，一个一定不存在的数据会被这个bitmap拦截掉，从而避免了对底层存储系统的查询压力。

## 归纳：底层数据结构与对应实现

**redis底层实现的8种数据结构：**

| 数据结构   | 解析                                                                      |
|------------|---------------------------------------------------------------------------|
| SDS        | 支持自动动态扩容的字节数组                                                |
| list       | 链表                                                                      |
| dict       | 使用双哈希表实现的, 支持平滑扩容的字典                                    |
| zskiplist  | 附加了后向指针的跳跃表                                                    |
| intset     | 用于存储整数数值集合的自有结构                                            |
| ziplist    | 一种实现上类似于TLV, 但比TLV复杂的, 用于存储任意数据的有序序列的数据结构  |
| quicklist  | 一种以ziplist作为结点的双链表结构, 实现的非常不错                         |
| zipmap     | 一种用于在小规模场合使用的轻量级字典结构                                  |

**Redis顶层数据结构对应的底层实现：**每种对象的底层数据结构实现都可能不止一种

| 数据结构 | 底层实现                                    |
|----------|---------------------------------------------|
| 字符串   | SDS，或int（long int整数）                  |
| List     | Ziplist或简单双端链表                       |
| 哈希     | Ziplist或hashtable（底层是dict）            |
| 集合     | Intset或hashtable（底层是dict）             |
| 有序集合 | 一种是ziplist，另一种是skiplist与dict的结合 |

## redis和memchache 的 一些区别，使用总结

1、Redis和Memcache都是将数据存放在内存中。不过memcache还可用于缓存其他东西，例如图片、视频等等

2、Redis不仅仅支持简单的k/v类型的数据，同时还提供list，set，hash等数据结构的存储。

3、虚拟内存--Redis当物理内存用完时，可以将一些很久没用到的value 交换到磁盘 .

4、过期策略--memcache在set时就指定，例如set key1 0 0
8,即永不过期。Redis可以通过例如expire 设定，例如expire name 10

5、分布式--设定memcache集群，利用magent做一主多从;redis可以做一主多从。都可以一主一从

6、存储数据安全--memcache挂掉后，数据没了；redis可以定期保存到磁盘（持久化）,支持持久化，服务器重启，缓存依然可以获取。(这个很重要哦)

7、灾难恢复--memcache挂掉后，数据不可恢复; redis数据丢失后可以通过aof恢复

8、Redis支持数据的备份，即master-slave模式的数据备份。

## hot key与big key在redis中的问题

Hot key，即热点 key，一段时间内，该 key 的访问量远远高于其他的 redis key，
导致大部分的访问流量在经过 proxy 分片之后，都集中访问到某一个 redis 实例上。hot
key 通常在不同业务中，存储着不同的热点信息。比如

1.  新闻应用中的热点新闻内容；

2.  活动系统中某个用户疯狂参与的活动的活动配置；

3.  商城秒杀系统中，最吸引用户眼球，性价比最高的商品信息；

4.  ……

解决方案

1.  本地缓存

2.  利用分片算法的特性，对key进行打散处理：hot
    key加上前缀或者后缀，经过分片分布到不同的实例上，将访问量均摊到所有实例

big key即数据量大的 key
，由于其数据大小远大于其他key，导致经过分片之后，某个具体存储这个 big key
的实例内存使用量远大于其他实例，造成，内存不足，拖累整个集群的使用。big key
在不同业务上，通常体现为不同的数据，比如

**对 big key 进行拆分**

可以拆大json，大list

上面写的对 big key 存储的数据 （big value）进行拆分，变成value1，value2…
valueN，是相应的原来这个key也要拆分成多个key么？获取时再分别获取这几个key/value后组装在一起？YES

## redis-cluster设计

Redis-Cluster采用无中心结构
（可以部署主从模式），每个节点保存数据和整个集群状态,每个节点都和其他所有节点连接。

Redis
Cluster中节点负责存储数据，记录集群状态，集群节点能自动发现其他节点，检测出节点的状态，并在需要时剔除故障节点，提升新的主节点

PING-PONG机制彼此互联，

Redis Cluster在设计中没有使用一致性哈希（Consistency
Hashing），而是使用数据分片（Sharding）引入哈希槽（hash slot）来实现

## Redis-cluster的同步？

分两种：全量同步（RDB文件同步）与命令广播（命令输入缓冲区）+增量同步；

**如果某一个子节点A短暂的断连了T秒，那么A再次恢复连接之后该如何同步数据呢？**Redis选择的做法是开辟一个缓冲区（默认大小是1M)，每次处理完命令之后，先写入缓冲区repl_backlog,
然后再发送给子节点。这就是增量复制（也叫部分复制）；

涉及到数据过期时，Redis采用的做法是当Master节点判断某个key过期了之后会向子节点发送DEL命令删除掉数据。

## 跳表（Skiplist）与其他数据结构的对比

跳表都可以达到平衡树的效率（查询节点支持平均O（lgN），最坏O（N）的复杂度），但实现和维护起来却比平衡树简单很多。

## Zset

zset底层的存储结构包括ziplist或skiplist，在同时满足以下两个条件的时候使用ziplist，其他时候使用skiplist

![](media/8d552ce12ad936aa833cb0b0e09650fd.png)

## Redis定义了几种策略用来处理内存满，需要淘汰的情况？

（20字节抖音提前批）

**noeviction(默认策略)**：对于写请求不再提供服务，直接返回错误（DEL请求和部分特殊请求除外）

**allkeys-lru**：从所有key中使用LRU算法进行淘汰

**volatile-lru**：从设置了过期时间的key中使用LRU算法进行淘汰

**allkeys-random**：从所有key中随机淘汰数据

**volatile-random**：从设置了过期时间的key中随机淘汰

**volatile-ttl**：在设置了过期时间的key中，根据key的过期时间进行淘汰，越早过期的越优先被淘汰

## Redis里的HASH

先了解下 hash 的内部结构.第一维是数组,第二维是链表.组成一个 **hashtable**.

![](media/8fadca3e4ef67c6a1b1eb8fb2fe100b4.png)

### 渐进式rehash

在 Java 中 HashMap 扩容是个很耗时的操作,需要去申请新的数组,为了追求高性能,Redis
采用了**渐进式 rehash** 策略.这也是 hash 中最重要的部分.

扩容的时候 rehash 策略会保留新旧两个 hashtable 结构,查询时也会同时查询两个
hashtable.Redis会将旧 hashtable 中的内容一点一点的迁移到新的 hashtable

| 触发条件                             | 执行动作                                                     |
|--------------------------------------|--------------------------------------------------------------|
| 来自客户端对 hash 的指令操作         | 数据搬迁                                                     |
| hashtable 中元素的个数等于数组的长度 | 开始扩容,扩容的新数组是原数组大小的 2 倍，会受 bgsave 的影响 |
| 元素的个数达到了数组长度的 5 倍时    | Redis 会强制扩容                                             |
| 元素个数少于数组长度的 10%.          | 缩容来减少空间占用,并且缩容不会受 bgsave 的影响              |

## Redis持久化策略（RDB，AOF）

主要分为RDB AOF两种。

RDB又分为Save和bgsave两种，Save会阻塞当前Redis进程中所有客户端的访问，bgsave通过fork()创建一个子进程，在子进程中备份，两个重点：

-   Fork()的子进程能访问父进程的内存？会复制，不直接访问

-   Fock()的子进程复制内存表现为Copy-on-Write

### AoF重写

>   解决AOF过大的问题，主要是对写指令进行重写。

>   不读取AOF文件，而是直接从内存中获取数据

>   默认达到原aof文件大小的一倍，最小的开始重写大小为64M

## Redis 使用 sds 而不是传统 C 字符串的原因

因为 char\* 类型的功能单一， 抽象层次低， 并且不能高效地支持一些 Redis
常用的操作（比如追加操作和长度计算操作）， 所以在 Redis 程序内部，
绝大部分情况下都会使用 sds 而不是 char\* 来表示字符串。

![digraph {     label = "\\n 图 2-1    SDS 示例";     rankdir = LR;     node
[shape = record];     //     sdshdr [label = "sdshdr \| free \\n 0 \| len \\n 5
\| \<buf\> buf"];     buf [label = "{ 'R' \| 'e' \| 'd' \| 'i' \| 's' \| '\\\\0'
}"];     //     sdshdr:buf -\> buf;
}](media/a807c8909d42d2942d771afba97325d6.png)

# MySQL与DB

## MySQL体系架构

MySQL逻辑系统架构分为3层:

-   应用层

-   MySQL服务层

-   存储引擎层

<https://zhuanlan.zhihu.com/p/43736857>

## 什么是共享锁和排它锁

**共享锁**（SELECT ... LOCK IN SHARE
MODE）即一个事务获取一条记录共享锁的同时，其他事务也可以获得这条记录的共享锁，但是如果同时有多个事务获得这条记录的共享锁，谁也无法修改这条记录，直到都释放掉共享锁，只剩下一个事务拥有这条记录的锁为止。

**排它锁**（SELECT ... FOR
UPDATE）即一个事务获得了一条记录的排它锁的同时，其他事务就不能获得这条记录的共享锁和排它锁，也无法修改这条记录，直到这个事务释放掉锁为止。

## InnoDB底层怎么实现不同的事务隔离级别？

是通过MVCC,即多并发版本控制

## InnoDB和MyISAM的最大不同点有两个：

1.  InnoDB支持事务(transaction)；MyISAM不支持事务

2.  Innodb 默认采用行锁， MyISAM
    是默认采用表锁。加锁可以保证事务的一致性，可谓是有人(锁)的地方，就有江湖(事务)
    MyISAM不适合高并发。

## Why does InnoDB introduce B+ tree for indexes instead of B tree or hashmap?

哈希作为底层的数据结构，遇到上述的场景时，使用哈希构成的主键索引或者辅助索引可能就没有办法快速处理了，它对于处理范围查询或者排序性能会非常差，只能进行全表扫描并依次判断是否满足条件。

B 树与 B+ 树的最大区别就是，B 树可以在非叶结点中存储数据，但是 B+
树的所有数据其实都存储在叶子节点中。高度为 3 的 B+
树就能够存储千万级别的数据，实践中 B+ 树的高度最多也就 4 或者
5，所以这并不是影响性能的根本问题。

## MYSQL 里头的锁

**解决幻读？**一种是采用SERIALIZABLE 数据隔离级别

另一种方案是采用在RR数据隔离级别下，手动给select操作加上x锁（排它锁）或者s锁（共享锁）

**共享锁**（SELECT ... LOCK IN SHARE
MODE）即一个事务获取一条记录共享锁的同时，其他事务也可以获得这条记录的共享锁，但是如果同时有多个事务获得这条记录的共享锁，谁也无法修改这条记录，直到都释放掉共享锁，只剩下一个事务拥有这条记录的锁为止。

**排它锁**（SELECT ... FOR
UPDATE）即一个事务获得了一条记录的排它锁的同时，其他事务就不能获得这条记录的共享锁和排它锁，也无法修改这条记录，直到这个事务释放掉锁为止。

## 数据库的三范式：

-   1NF:原子列，

-   2NF：必须有主键，其他列依赖与主键；

-   3NF：消除传递

## 数据库视图

本质上是人为定义的一个虚拟概念；安全，方便易控制；

虚拟的，更新时间\*随时更新（或者说Insert与update的时候）

## 索引未命中可能原因的场景

-   **查询中带有or关键字**

-   如果列类型是**字符串**，那一定要在条件中将数据使用**引号引用起来**,否则不使用索引

-   范围查询后的索引失效

-   **like**查询是以**%**开头

-   **not in, not exist**

## MySQL中的删除语句

Delete 删除行，DROP删除表，表字段或库，TRUNCATE删除表，快速且无日志；

## MySQL的聚簇索引与非聚簇索引

聚簇索引的叶子节点就是数据节点，而非聚簇索引的叶子节点仍然是索引节点，只不过有指向对应数据块的指针。

Redis 使用 sds 而不是传统 C 字符串的原因。

![](media/6499051e75e1c80fdcf225b79b72e5fe.jpeg)

# 计算机组成原理

## 影响硬盘性能的因素

影响磁盘的关键因素是磁盘服务时间，即磁盘完成一个I/O请求所花费的时间，它由寻道时间、旋转延迟和数据传输时间三部分构成。

### 1. 寻道时间

Tseek是指将读写磁头移动至正确的磁道上所需要的时间。寻道时间越短，I/O操作越快，目前磁盘的平均寻道时间一般在3-15ms。

### 2. 旋转延迟

Trotation是指盘片旋转将请求数据所在的扇区移动到读写磁盘下方所需要的时间。旋转延迟取决于磁盘转速，通常用磁盘旋转一周所需时间的1/2表示。比如：7200rpm的磁盘平均旋转延迟大约为60\*1000/7200/2
= 4.17ms，而转速为15000rpm的磁盘其平均旋转延迟为2ms。

### 3. 数据传输时间

Ttransfer是指完成传输所请求的数据所需要的时间，它取决于数据传输率，其值等于数据大小除以数据传输率。目前IDE/ATA能达到133MB/s，SATA
II可达到300MB/s的接口数据传输率，数据传输时间通常远小于前两部分消耗时间。简单计算时可忽略。

# 云

## K8S: [Kubernetes](https://kubernetes.io/zh/docs/concepts/overview/what-is-kubernetes/)

### 什么是K8S？

**K8S是负责自动化运维管理多个Docker程序的集群，即自动化运维管理Docker（容器化）程序**

### K8S重要概念

#### 1. cluster

cluster是 计算、存储和网络资源的集合，k8s利用这些资源运行各种基于容器的应用。

#### 2.master

master是cluster的大脑，他的主要职责是调度，即决定将应用放在那里运行。master运行linux操作系统，可以是物理机或者虚拟机。为了实现高可用，可以运行多个master。

#### 3.node

node的职责是运行容器应用。node由master管理，node负责监控并汇报容器的状态，同时根据master的要求管理容器的生命周期。node运行在linux的操作系统上，可以是物理机或者是虚拟机。

#### 4.Pod

**Pod可以被理解成一群可以共享网络、存储和计算资源的容器化服务的集合**。再打个形象的比喻，在同一个Pod里的几个Docker服务/程序，好像被部署在同一台机器上，可以通过localhost互相访问，并且可以共用Pod里的存储资源（这里是指Docker可以挂载Pod内的数据卷，数据卷的概念，后文会详细讲述，暂时理解为“需要手动mount的磁盘”）。笔者总结Pod如下图，可以看到：**同一个Pod之间的Container可以通过localhost互相访问，并且可以挂载Pod内所有的数据卷；但是不同的Pod之间的Container不能用localhost访问，也不能挂载其他Pod的数据卷**。如下图

![](media/ad105a324a101672da9a04d8e2fcb4cd.png)

#### 5.Volume数据卷

**数据卷volume是Pod内部的磁盘资源**。

#### 6.Container 容器

**一个Pod内可以有多个容器container**。

在Pod中，容器也有分类，对这个感兴趣的同学欢迎自行阅读更多资料：

-   **标准容器 Application Container**。

-   **初始化容器 Init Container**。

-   **边车容器 Sidecar Container**。

-   **临时容器 Ephemeral Container**。

一般来说，我们部署的大多是**标准容器（ Application Container）**。

#### 7.Deployment 和 ReplicaSet（简称RS）

Deploy-\>RS-\>Pod的管理层次

![](media/09722a852d20968ce9fe5e5232edb1f9.png)

#### 8.Service何ingress

Deployment和Replication Controller,
RepliceSet主要管控Pod程序服务；另一方面，Service和ingress主要管控Pod网络服务。Service隐藏了服务细节，统一对外暴露服务接口，真正做到了微服务。

#### 9.Namespace

把一个K8S集群划分为若干个资源不可共享的虚拟集群而诞生的。

### 为什么K8S引入Pod这个概念？

Kubernetes 引入 Pod 主要基于下面两个目的：

1.  **可管理性。**  
    有些容器天生就是需要紧密联系，一起工作。Pod
    提供了比容器更高层次的抽象，将它们封装到一个部署单元中。Kubernetes 以 Pod
    为最小单位进行调度、扩展、共享资源、管理生命周期。

2.  **通信和资源共享。**  
    Pod 中的所有容器使用同一个网络 namespace，即相同的 IP 地址和 Port
    空间。它们可以直接用 localhost 通信。同样的，这些容器可以共享存储，当
    Kubernetes 挂载 volume 到 Pod，本质上是将 volume 挂载到 Pod 中的每一个容器。

## Docker

### Docker核心技术

![](media/2ec5a85cfe5e129b7d14caf61ab7e34b.png)

Chrootrootfs提供不同的环境（早期技术）

简单来说 **namespace** 是由 Linux
内核提供的，用于进程间资源隔离的一种技术，使得a,
b进程可以看到S资源；而c进程看不到。

**cgroups**是namespace的一种，是为了实现虚拟化而采取的资源管理机制，决定哪些分配给容器的资源可被我们管理，分配容器使用资源的多少。容器内的进程是运行在一个隔离的环境里，使用起来，就好像是在一个独立于宿主的系统下操作一样。这种特性使得容器封装的应用比直接在宿主运行更加安全。例如可以设定一个memory使用上限，一旦进程组（容器）使用的内存达到限额再申请内存，就会出发OOM（out
of memory），这样就不会因为某个进程消耗的内存过大而影响到其他进程的运行。

Image 是 Docker
部署的基本单位，它包含了程序文件，以及这个程序依赖的资源的环境。Docker
Image是以一个 mount 点挂载到容器内部的。

容器可以近似理解为镜像的运行时实例，默认情况下也算是在镜像层的基础上增加了一个可写层。所以，一般情况下如果你在容器内做出的修改，均包含在这个可写层中。

### Docker底层原理

![](media/15f5506a61f9dbc5d470eb8899d10fe1.png)

## 常用Docker命令

-   启动容器以后台方式运行(更通用的方式）：

\$docker run -d -it image\_name

-   进入正在运行的容器内部，同时运行bash(比attach更好用)

docker exec -t -i \<id/container_name\> /bin/bash

-   列出当前所有正在运行的container

\$docker ps

-   用一行列出所有正在运行的container（容器多的时候非常清晰）

\$docker ps \| less -S

退出容器 Ctrl + D

推出容器但不关闭容器 Ctrl + P + Q