# C++ Note

## C++ 头文件中类的互相引用

> 核心：头文件相互引用，一个 `#include` ，另一个 `class C`
>
> C++ 头文件相互 `#include`时最好是：
>
> - 在 `"CA.h"` 中 ``#include "CB.h"``
>
> - 在 `"CB.h"` 中用类的前向声明: `class CA;`
>
> - 最好加上头文件卫士( ``#ifndef ***   #define ***  #endif`) 

<details><summary>示例代码：</summary>

```c++
/* CA.h */
#ifndef HEADER_CA
#define HEADER_CA 
#include "CB.h" 
class CA
{
    CB* pB;
    CB  b; //正确,因为此处已经知道 CB 类的大小,且定义了 CB,可以为 b 分配空间
}; 
#endif
```

```c++
/* CB.h */
#ifndef HEADER_CB
#define HEADER_CB 
//这个必须要用, 不能只用 #include "CA.h", 如果只是 #include "CA.h" 而没有 class CA; 则会报错.
class CA;
class CB
{
    CA* pA;
    //CA a; //错误,因为此时还不知道 CA 的大小,无法分配空间
}; 
#endif 
```

```c++
/* CA.cpp */
#include "CA.h"
// 但是, *.cpp文件只能 #include “CA.h”. 如果 #icnlude “CB.h” 则错误(展开后 CA 不识别 CB)。
```

</details>

## C++ 类中的 static 成员的初始化和特点

> [C/C++ 中的static关键字](https://zhuanlan.zhihu.com/p/37439983)

<details><summary>示例代码：</summary>

```c++
#include <iostream>

using namespace std;

class Test
{
public:
    Test() : y(1), r(y), d(3){} //对于常量型成员变量和引用型成员变量，必须通过参数化列表的方式进行初始化。
    ~Test(){}

    int y;      	//普通变量成员
    int &r;     	//引用成员变量
    const int d;    //常量成员变量
    static int c;   //静态成员变量
    static const int x = 2.1;     //静态常量整型成员变量
    static const int xx;          //静态常量整型成员变量声明
    static const double z;  	  //静态常量非整型成员变量声明
    static const float zz = 6.6;  //静态常量非整型成员变量
};

const int Test::xx = 4; //静态常量整型成员变量定义
const double Test::z = 5.1; //静态常量非整型成员变量定义
int Test::c = 2;

int main(void)
{
    cout << Test::x << endl;    
    return 0;
}
```

</details>

这些特殊类型的成员变量主要有：

- 引用 
- 常量 
- 静态变量 
- 静态整型常量 
- 静态非整型常量

**结论：**

**静态成员属于类作用域，但不属于类对象，它的生命周期和普通的静态变量一样，程序运行时进行分配内存和初始化，程序结束时则被释放。所以不能在类的构造函数中进行初始化。**

**static 成员的优点**

> - static 成员的名字是在类的作用域中，因此可以避免与其它类成员或全局对象名字冲突。
> - 可以实施封装，static 成员可以是私有的，而全局对象不可以。
> - 阅读程序容易看出 static 成员与某个类相关联，这种可见性可以清晰地反映程序员的意图。

**static 成员函数特点**

> - 因为 static 成员函数没有 this 指针，所以静态成员函数不可以访问非静态成员。
> - 非静态成员函数可以访问静态成员。
> - 静态数据成员与类的大小无关，因为静态成员只是作用在类的范围而已。

<details><summary>示例代码：</summary>

```c++
#include <iostream>

using namespace std;

class test2
{
public:
    test2(int num) : y(num){}
    ~test2(){}

    static void testStaticFun()
    {
      cout << "y = " << y << endl; //Error:静态成员函数不能访问非静态成员
    }

    void testFun()
    {
        cout << "x = " << x << endl; 
    }
private:
    static int x; //静态成员变量的引用性说明
    int y;
};

int test2::x = 10; //静态成员变量的定义性说明

int main(void)
{
    test2 t(100);
    t.testFun();

    return 0;
}
```

</details>

**static 用法总结**

- **c 语言中：**
    - 用于函数内部修饰变量，即函数内的静态变量。这种变量的生存期长于该函数，使得函数具有一定的“状态”。使用静态变量的函数一般是不可重入的，也不是线程安全的，比如 strtok(3)。
    - 用在文件级别（函数体之外），修饰变量或函数，表示该变量或函数只在本文件可见，其他文件看不到也访问不到该变量或函数。专业的说法叫 “具有internal linkage”（简言之：不暴露给别的 translation unit）。

- **c++ 语言中（由于 C++ 引入了类，在保持与 C 语言兼容的同时，static 关键字又有了两种新用法）：**
    - 用于修饰类的数据成员，即所谓 “静态成员”。这种数据成员的生存期大于 class 的对象（实例/instance）。静态数据成员是每个 class 有一份，普通数据成员是每个 instance 有一份。
    - 用于修饰 class 的成员函数，即所谓 “静态成员函数”。这种成员函数只能访问静态成员和其他静态程员函数，不能访问非静态成员和非静态成员函数。

## C++ 四种类型转换运算符：static_cast、dynamic_cast、const_cast 和 reinterpret_cast

> [C++四种类型转换运算符：static_cast、dynamic_cast、const_cast和reinterpret_cast](http://c.biancheng.net/cpp/biancheng/view/3297.html)
>
> [C++ 中static_cast、dynamic_cast、const_cast和reinterpret_cast总结](https://www.jianshu.com/p/5163a2678171)

C++ 对类型转换进行了分类，并新增了四个关键字来予以支持，它们分别是：

| 关键字             | 说明                                                         |
| ------------------ | ------------------------------------------------------------ |
| `static_cast`      | 用于良性转换，一般不会导致意外发生，风险很低。               |
| `const_cast`       | 用于 const 与非 const、volatile 与非 volatile 之间的转换。   |
| `reinterpret_cast` | 高度危险的转换，这种转换仅仅是对二进制位的重新解释，不会借助已有的转换规则对数据进行调整，但是可以实现最灵活的 C++ 类型转换。 |
| `dynamic_cast`     | 借助 RTTI，用于类型安全的向下转型（Downcasting）。这四个关键字的语法格式都是一样的，具体为： |

这四个关键字的语法格式都是一样的，具体为：

`xxx_cast<newType>(data)`

newType 是要转换成的新类型，data 是被转换的数据。例如，老式的 C 风格的 double 转 int 的写法为：

```c
double scores = 95.5;int n = (int)scores;
```

C++ 新风格的写法为：

```c++
double scores = 95.5;int n = static_cast<int>(scores);
```

**static_cast 关键字**

static_cast 只能用于良性转换，这样的转换风险较低，一般不会发生什么意外，例如：

- 原有的自动类型转换，例如 short 转 int、int 转 double、const 转非 const、向上转型等；
- void 指针和具体类型指针之间的转换，例如`void *`转`int *`、`char *`转`void *`等；
- 有转换构造函数或者类型转换函数的类与其它类型之间的转换，例如 double 转 Complex（调用转换构造函数）、Complex 转 double（调用类型转换函数）。

需要注意的是，static_cast 不能用于无关类型之间的转换，因为这些转换都是有风险的，例如：

- 两个具体类型指针之间的转换，例如`int *`转`double *`、`Student *`转`int *`等。不同类型的数据存储格式不一样，长度也不一样，用 A 类型的指针指向 B 类型的数据后，会按照 A 类型的方式来处理数据：如果是读取操作，可能会得到一堆没有意义的值；如果是写入操作，可能会使 B 类型的数据遭到破坏，当再次以 B 类型的方式读取数据时会得到一堆没有意义的值。
- int 和指针之间的转换。将一个具体的地址赋值给指针变量是非常危险的，因为该地址上的内存可能没有分配，也可能没有读写权限，恰好是可用内存反而是小概率事件。

static_cast 也不能用来去掉表达式的 const 修饰和 volatile 修饰。换句话说，不能将 const/volatile 类型转换为非 const/volatile 类型。

static_cast 是“静态转换”的意思，也就是在编译期间转换，转换失败的话会抛出一个编译错误。

<details><summary>下面的代码演示了 static_cast 的正确用法和错误用法：</summary>

```c++
#include <iostream>
#include <cstdlib>

using namespace std;

class Complex{
public:    
    Complex(double real = 0.0, double imag = 0.0): m_real(real), m_imag(imag){ }
public:    
    operator double() const { return m_real; }  //类型转换函数
private:    
    double m_real;    
    double m_imag;
};

int main(){    
    //下面是正确的用法    
    int m = 100;    
    Complex c(12.5, 23.8);    
    long n = static_cast<long>(m);  //宽转换，没有信息丢失    
    char ch = static_cast<char>(m);  //窄转换，可能会丢失信息    
    int *p1 = static_cast<int*>( malloc(10 * sizeof(int)) );  //将void指针转换为具体类型指针    
    void *p2 = static_cast<void*>(p1);  //将具体类型指针，转换为void指针    
    double real= static_cast<double>(c);  //调用类型转换函数       
    
    //下面的用法是错误的    
    float *p3 = static_cast<float*>(p1);  //不能在两个具体类型的指针之间进行转换    
    p3 = static_cast<float*>(0X2DF9);  //不能将整数转换为指针类型    
    return 0;
}
```

</details>

## C++ using 用法

<details><summary>示例代码：</summary>

```c++
#include <iostream>

using namespace std;

class ClassOne 
{
public:
	int w;
protected:
	int a;
};

class ClassTwo
{
public:
	using ModuleType = ClassOne;
};

template <typename ClassType>class ClassThree : private ClassType
{
public:
	using typename ClassType::ModuleType;
	ModuleType m;
	ClassThree() = default;
	virtual ~ClassThree() = default;
};

void main()
{
	ClassThree<ClassTwo>::ModuleType a;
}
```

</details>

在上面代码中，一共有三处使用了`using`，分别是第 3, 16, 22 行，它们的作用为：

- 引入命名空间
- 指定别名
- 在子类中引用基类的成员

**引入命名空间**

指定命名空间是 `C++ using namespace` 中最常被用到的地方，在第 3 行中的：

```c++
using namespace std;
```

**指定别名**

using 的另一个作用是指定别名，一般都是 `using a = b;` 这样的形式出现，比如在 13 行中：

```c++
using ModuleType = ClassOne;
```

ModuleType 是 ClassOne 的一个别名。

using 这个作用也比较常见，比如在 `vector.h` 中就有：

```c++
template<class _Ty,class _Alloc = allocator<_Ty>>class vector: public _Vector_alloc<_Vec_base_types<_Ty, _Alloc>>
{
public:
	using value_type = _Ty;
	using allocator_type = _Alloc;
}
```

即 `value_type` 是 `_Ty` 的一个别名，``value_type a;`` 和 `_Ty a;` 是同样的效果。

**在子类中引用基类的成员**

using 的第三个作用是子类中引用基类的成员，一般都是 `using CBase::a;` 这样的形式出现，比如在 22 行中：

```c++
using typename ClassType::ModuleType;
```

它和一般形式有些区别，就是它加了个 `typename` 修饰，这是因为类 `ClassThree` 本身是个模板类，它的基类`ClassType` 是个模板，这个 `typename` 和 `using` 其实没有什么关系。如果 `ClassType` 不是模板的话，这行代码就可以写成：

```c++
using ClassType::ModuleType;
```

剩下的就是 `using` 的作用，它引用了基类中的成员 `ModuleType`， `ModuleType` 又是类 `ClassOne` 的别名，所以后面 `ModuleType m;` 相当于定义对象 `m`，对于子类成员 `m` 来说，这样的效果和下面是相同的：

```c++
typename ClassType::ModuleType m;
```

不同之处在于 `using` 还修改了基类成员的访问权限，子类 `ClassThree` 私有继承 `ClassType`，所以 `ClassType` 中共有成员 `ModuleType` 在子类 `ClassThree` 是私有的，它不能被外部访问。但是使用 `using`后，在 `main()` 函数中可以使用。

## 智能指针 shared_ptr 的用法

> [智能指针shared_ptr的用法](https://www.cnblogs.com/jiayayao/p/6128877.html)
>
> [C++中的智能指针（1）：shared_ptr 的介绍和用法](https://blog.csdn.net/thinkerleo1997/article/details/78754919)

> 为了解决 C++ 内存泄漏的问题，C++11引入了智能指针（Smart Pointer）。
>
> ​		智能指针的原理是，接受一个申请好的内存地址，构造一个保存在栈上的智能指针对象，当程序退出栈的作用域范围后，由于栈上的变量自动被销毁，智能指针内部保存的内存也就被释放掉了（除非将智能指针保存起来）。

C++11 提供了三种智能指针：`std::shared_ptr`, `std::unique_ptr`, `std::weak_ptr`，使用时需添加头文件 `<memory>`。

`shared_ptr` 使用引用计数，每一个 `shared_ptr` 的拷贝都指向相同的内存。每使用他一次，内部的引用计数加 1，每析构一次，内部的引用计数减 1，减为 0 时，删除所指向的堆内存。``shared_ptr` 内部的引用计数是安全的，但是对象的读取需要加锁。

**shared_ptr 的基本用法**

- 初始化

    可以通过构造函数、`std::make_shared<T>` 辅助函数和 `reset` 方法来初始化 `shared_ptr`：

<details><summary>示例代码：</summary>

```c++
#include "stdafx.h"
#include <iostream>
#include <future>
#include <thread>

using namespace std;
class Person
{
public:
    Person(int v) {
        value = v;
        std::cout << "Cons" <<value<< std::endl;
    }
    ~Person() {
        std::cout << "Des" <<value<< std::endl;
    }

    int value;
};

int main()
{
    std::shared_ptr<Person> p1(new Person(1));// Person(1)的引用计数为1

    std::shared_ptr<Person> p2 = std::make_shared<Person>(2);

    p1.reset(new Person(3));// 首先生成新对象，然后引用计数减1，引用计数为0，故析构Person(1)
                            // 最后将新对象的指针交给智能指针

    std::shared_ptr<Person> p3 = p1;//现在p1和p3同时指向Person(3)，Person(3)的引用计数为2

    p1.reset();//Person(3)的引用计数为1
    p3.reset();//Person(3)的引用计数为0，析构Person(3)
    return 0;
}
```

</details>

注意，不能将一个原始指针直接赋值给一个智能指针，如下所示，原因是一个是类，一个是指针。

```c++
std::shared_ptr<int> p4 = new int(1);// error
```

**reset()包含两个操作。当智能指针中有值的时候，调用reset()会使引用计数减1.当调用reset（new xxx())重新赋值时，智能指针首先是生成新对象，然后将就对象的引用计数减1（当然，如果发现引用计数为0时，则析构旧对象），然后将新对象的指针交给智能指针保管。**

- 获取原始指针　　

```c++
std::shared_ptr<int> p4(new int(5));
int *pInt = p4.get();
```

- 指定删除器

    智能指针可以指定删除器，当智能指针的引用计数为0时，自动调用指定的删除器来释放内存。`std::shared_ptr` 可以指定删除器的一个原因是其默认删除器不支持数组对象，这一点需要注意。

**使用shared_ptr需要注意的问题**

但凡一些高级的用法，使用时都有不少陷阱。

- 不要用一个原始指针初始化多个 `shared_ptr`，原因在于，会造成二次销毁，如下所示：

    ```c++
    int *p5 = new int;
    std::shared_ptr<int> p6(p5);
    std::shared_ptr<int> p7(p5);// logic error
    ```

- 不要在函数实参中创建 `shared_ptr`。因为 C++ 的函数参数的计算顺序在不同的编译器下是不同的。正确的做法是先创建好，然后再传入。

```c++
function(shared_ptr<int>(new int), g());
```

- 禁止通过 `shared_from_this()` 返回 `this` 指针，这样做可能也会造成二次析构。
- 避免循环引用。智能指针最大的一个陷阱是循环引用，循环引用会导致内存泄漏。解决方法是 `AStruct` 或 `BStruct` 改为 `weak_ptr`。

<details><summary>示例代码：</summary>

```c++
struct AStruct;
struct BStruct;

struct AStruct {
    std::shared_ptr<BStruct> bPtr;
    ~AStruct() { cout << "AStruct is deleted!"<<endl; }
};

struct BStruct {
    std::shared_ptr<AStruct> APtr;
    ~BStruct() { cout << "BStruct is deleted!" << endl; }
};

void TestLoopReference()
{
    std::shared_ptr<AStruct> ap(new AStruct);
    std::shared_ptr<BStruct> bp(new BStruct);
    ap->bPtr = bp;
    bp->APtr = ap;
}
```

</details>

## shared_ptr 与 make_shared 的用法

程序使用动态内存出于以下三种原因之一

1. 程序不知道自己需要多少对象；
2. 程序不知道所需对象的准确类型；
3. 程序需要在多个对象间共享数据

**shared_ptr 的用法**

可以指向特定类型的对象，用于自动释放所指的对象

```c++
// 指向类型为CDlgPointCloud的对象PointCloudDlgPointer;
shared_ptr<CDlgPointCloud> PointCloudDlgPointer;
```


当然还有一个最安全的分配和使用动态内存的方法就是调用一个名为 `make_shared` 的标准库函数；

**make_shared 的用法**

`make_shared` 在动态内存中分配一个对象并初始化它，返回指向此对象的 `shared_ptr`，与智能指针一样，`make_shared` 也定义在头文件 `memory` 中；

当要用 `make_shared` 时，必须指定想要创建的对象类型，定义方式与模板类相同，在函数名之后跟一个尖括号，在其中给出类型；

```c++
make_shared<int> p3 = make_shared<int>(42)；
```

一般采用 `auto` 定义一个对象来保存 `make_shared` 的结果，如：

```c++
 auto p1 =make_shared<int>(42);
```

## C++11中 std::move 的使用

> [C++11中std::move的使用](https://blog.csdn.net/fengbingchun/article/details/52558914)

在C++11中，标准库在 `<utility>` 中提供了一个有用的函数 `std::move`，`std::move` 并不能移动任何东西，它唯一的功能是将一个左值强制转化为右值引用，继而可以通过右值引用使用该值，以用于移动语义。从实现上讲，`std::move` 基本等同于一个类型转换：`static_cast<T&&>(lvalue);`

`std::move` 函数可以以非常简单的方式将左值引用转换为右值引用。

通过 `std::move`，可以避免不必要的拷贝操作。

`std::move` 是为性能而生。

`std::move` 是将对象的状态或者所有权从一个对象转移到另一个对象，只是转移，没有内存的搬迁或者内存拷贝。

## C++ std::function 的用法

> [C++ std::function 的用法](https://blog.csdn.net/hzy925/article/details/79676085)

类模版 `std::function` 是一种通用、多态的函数封装。`std::function` 的实例可以对任何可以调用的目标实体进行存储、复制、和调用操作，这些目标实体包括普通函数、Lambda 表达式、函数指针、以及其它函数对象等。`std::function `对象是对 C++ 中现有的可调用实体的一种类型安全的包裹（我们知道像函数指针这类可调用实体，是类型不安全的）。

通常 `std::function` 是一个函数对象类，它包装其它任意的函数对象，被包装的函数对象具有类型为 `T1, …,TN` 的 N 个参数，并且返回一个可转换到R类型的值。`std::function` 使用 模板转换构造函数接收被包装的函数对象；特别是，闭包类型可以隐式地转换为 `std::function`。

## C++ std::enable_shared_from_this 的使用及实现原理

> [enable_shared_from_this 的使用及实现原理](http://blog.guorongfei.com/2017/01/25/enbale-shared-from-this-implementaion/)
>
> [关于std::enable_shared_from_this 的继承和 shared_from_this调用崩溃的解析](https://blog.csdn.net/fm_VAE/article/details/79660768)
>
> [C++ 智能指针的正确使用方式](https://www.cyhone.com/articles/right-way-to-use-cpp-smart-pointer/)



## C++ auto 类型用法总结

> [c++ auto类型用法总结](https://blog.csdn.net/xiaoquantouer/article/details/51647865)

**用途**

auto 是 c++ 程序设计语言的关键字。用于两种情况

1. 声明变量时根据初始化表达式自动推断该变量的类型
2. 声明函数时函数返回值的占位符

**简要理解**

auto 可以在声明变量时根据变量初始值的类型自动为此变量选择匹配的类型。

举例：`对于值x=1；` 既可以声明： `int x=1` 或  `long x=1`，也可以直接声明 `auto x=1`

**用法**

根据初始化表达式自动推断被声明的变量的类型，如：

```c++
auto f = 3.14;  //double
auto s("hello");  //const char*
auto z = new auto(9);  //int *
auto x1 = 5, x2 = 5.0, x3 = 'r';   //错误，必须是初始化为同一类型
```

但是，这么简单的变量声明类型，不建议用 auto 关键字，而是应更清晰地直接写出其类型。

auto 关键字更适用于类型冗长复杂、变量使用范围专一时，使程序更清晰易读。如：

```c++
std::vector<int> vect; 
for(auto it = vect.begin(); it != vect.end(); ++it)
{  //it的类型是std::vector<int>::iterator
    std::cin >> *it;
}
```

或者保存lambda表达式类型的变量声明：

```c++
//类型为 std::function<double(double)> 函数对象
auto ptr = [](double x){return x*x;};
```

**优势**

1. 拥有初始化表达式的复杂类型变量声明时简化代码。

比如：

```c++
#include <string>  
#include <vector>  
void loopover(std::vector<std::string>&vs)  
{  
    std::vector<std::string>::iterator i=vs.begin();  
    for(;i<vs.end();i++)  
    {       
    }
}
```

变为：

```c++
#include <string>  
#include <vector>  
void loopover(std::vector<std::string>&vs)  
{  
    for(  auto i=vs.begin();;i<vs.end();i++)  
    {  
    }
}
```

使用 `std::vector<std::string>::iterator` 来定义 `i` 是 C++ 常用的良好的习惯，但是这样长的声明带来了代码可读性的困难，因此引入 `auto`，使代码可读性增加。并且使用 STL 将会变得更加容易

2. 可以避免类型声明时的麻烦而且避免类型声明时的错误。

但是 auto 不能解决所有的精度问题。比如：

```c++
#include <iostream>  
using namespace std;  
int main()  
{  
   unsigned int a=4294967295;//最大的unsigned int值  
   unsigned int b=1；  
   auto c=a+b;  
   cout<<"a="<<a<<endl;  
   cout<<"b="<<b<<endl;  
   cout<<"c="<<c<<endl;  
}  
```

上面代码中，程序员希望通过声明变量 `c` 为 `auto` 就能解决 `a + b` 溢出的问题。而实际上由于 `a + b`返回的依然是 `unsigned int` 的值，姑且 `c` 的类型依然被推导为 `unsigned int`，`auto` 并不能帮上忙。这个跟动态类型语言中数据hi自动进行拓展的特性还是不一样的。

**注意的地方**

1. 可以用 `valatile`，`pointer（*）`，`reference（&）`，`rvalue reference（&&）` 来修饰 `auto`

```c++
auto k = 5;  
auto* pK = new auto(k);  
auto** ppK = new auto(&k);  
const auto n = 6;  
```

2. 用 auto 声明的变量必须初始化

3. auto 不能与其他类型组合连用

4. 函数和模板参数不能被声明为 auto

5. 定义在堆上的变量，使用了 auto 的表达式必须被初始化

```c++
int* p = new auto(0); //fine  
int* pp = new auto(); // should be initialized  
auto x = new auto(); // Hmmm ... no intializer  
auto* y = new auto(9); // Fine. Here y is a int*  
auto z = new auto(9); //Fine. Here z is a int* (It is not just an int)  
```

6. 以为 auto 是一个占位符，并不是一个他自己的类型，因此不能用于类型转换或其他一些操作，如 sizeof 和 typeid

7. 定义在一个 auto 序列的变量必须始终推导成同一类型

```c++
auto x1 = 5, x2 = 5.0, x3='r';  //错误，必须是初始化为同一类型
```

8. auto 不能自动推导成 CV-qualifiers (constant & volatile qualifiers)

9. auto 会退化成指向数组的指针，除非被声明为引用

## C++11 weak_ptr 使用介绍

> [c++11之weak_ptr 使用介绍](https://blog.csdn.net/c_base_jin/article/details/79440999)
>
> [C++11中weak_ptr的使用](https://blog.csdn.net/fengbingchun/article/details/52203825)
>
> [std::shared_ptr 和 std::weak_ptr的用法以及引用计数的循环引用问题](https://www.cnblogs.com/diegodu/p/6370736.html)

weak_ptr 是弱智能指针对象，它不控制所指向对象生存期的智能指针，它指向由一个 shared_ptr 管理的智能指针。将一个 weak_ptr 绑定到一个 shared_ptr 对象，不会改变 shared_ptr 的引用计数。一旦最后一个所指向对象的 shared_ptr 被销毁，所指向的对象就会被释放，即使此时有 weak_ptr 指向该对象，所指向的对象依然被释放。


## C++ std::bind 用法

> [C/C++ C++ 11 std::function和std::bind用法](https://www.cnblogs.com/YZFHKMS-X/p/12067252.html)

`std::bind` 主要用于绑定生成目标函数，一般用于生成的回调函数，`cocos` 的回退函数都是通过 `std::bind` 和 `std::function` 实现的。两个点要明白：

1. 绑定全局或者静态函数比绑定成员函数少了个成员变量，且不需要引用如下

```c++
//绑定全局函数
auto pfunc = std::bind(func1, 3);
//绑定静态函数
pfunc = std::bind(Func::func6, 3);
//绑定类成员函数，函数前需要添加引用，多一个变量参数func
Func func;
pfunc =std::bind(&Func::func2, func, 3);
```

2. 占位符 `std::placeholders::_1` 代表此处是要输入参数，且这个参数排在第一位，代码分析更明白

<details><summary>示例代码：</summary>

```c++
#include <iostream>
#include <functional>
using namespace std;
class Func
{
    public:
    static void func6(int numa, int numb, int numc)
    {
        std::cout << numa << " " << numb << " " << numc << endl;
    }
    void func2(int numa, int numb, int numc,std::string name)
    {
        std::cout << numa << " " << numb << " " << numc<<" " << name<< endl ;
    }
}
void callFunc(std::function<void(int a,int b)> call)
{
    call(1,2);
}
void func1(int numa, int numb, int numc)
{
    std::cout << numa << " " << numb << " " << numc << endl;
}
int main()
{
    callFunc(std::bind(func1, std::placeholders::_1, std::placeholders::_2, 3));
    callFunc(std::bind(func1, std::placeholders::_2, std::placeholders::_1, 3));
    callFunc(std::bind(func1, std::placeholders::_2, 3, std::placeholders::_1));
    callFunc(std::bind(Func::func6, std::placeholders::_1, std::placeholders::_2, 3));
    callFunc(std::bind(&Func::func2, func, std::placeholders::_1, std::placeholders::_2, 3, "name"));
}

// 运行结果如下 
1 2 3
2 1 3
2 3 1
1 2 3
1 2 3 name
```

</details>

## C++ 11 Lambda 表达式

> [C++ 11 Lambda表达式](https://www.cnblogs.com/DswCnblog/p/5629165.html)

C++11的一大亮点就是引入了Lambda表达式。利用Lambda表达式，可以方便的定义和创建匿名函数。对于C++这门语言来说来说，“Lambda表达式”或“匿名函数”这些概念听起来好像很深奥，但很多高级语言在很早以前就已经提供了Lambda表达式的功能，如C#，[Python](http://lib.csdn.net/base/11)等。今天，我们就来简单介绍一下C++中Lambda表达式的简单使用。

## **声明Lambda表达式**

Lambda 表达式完整的声明格式如下：

```c++
[capture list] (params list) mutable exception-> return type { function body }
```

各项具体含义如下

1. `capture list`：捕获外部变量列表
2. `params list`：形参列表
3. `mutable` 指示符：用来说用是否可以修改捕获的变量
4. `exception` ：异常设定
5. `return type`：返回类型
6. `function body`：函数体

此外，我们还可以省略其中的某些成分来声明“不完整”的Lambda表达式，常见的有以下几种：

| 序号 | 格式                                                        |
| ---- | ----------------------------------------------------------- |
| 1    | [capture list] (params list) -> return type {function body} |
| 2    | [capture list] (params list) {function body}                |
| 3    | [capture list] {function body}                              |

其中：

- 格式 1 声明了 const 类型的表达式，这种类型的表达式不能修改捕获列表中的值。

- 格式 2 省略了返回值类型，但编译器可以根据以下规则推断出 Lambda 表达式的返回类型： 

    - （1）：如果 function body 中存在 return 语句，则该 Lambda 表达式的返回类型由 return 语句的返回类型确定； 
    - （2）：如果 function body 中没有 return 语句，则返回值为 void 类型。

- 格式 3 中省略了参数列表，类似普通函数中的无参函数。

    讲了这么多，我们还没有看到 Lambda 表达式的庐山真面目，下面我们就举一个实例。

<details><summary>示例代码：</summary>

```c++
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

bool cmp(int a, int b)
{
    return  a < b;
}

int main()
{
    vector<int> myvec{ 3, 2, 5, 7, 3, 2 };
    vector<int> lbvec(myvec);

    sort(myvec.begin(), myvec.end(), cmp); // 旧式做法
    cout << "predicate function:" << endl;
    for (int it : myvec)
        cout << it << ' ';
    cout << endl;

    sort(lbvec.begin(), lbvec.end(), [](int a, int b) -> bool { return a < b; });   // Lambda表达式
    cout << "lambda expression:" << endl;
    for (int it : lbvec)
        cout << it << ' ';
}
```

</details>

在 C++11 之前，我们使用 STL 的 sort 函数，需要提供一个谓词函数。如果使用 C++11 的 Lambda 表达式，我们只需要传入一个匿名函数即可，方便简洁，而且代码的可读性也比旧式的做法好多了。











