# RayTracer

code for [RayTracingInOneWeekend](https://github.com/RayTracing/raytracing.github.io/)

~~第一次用vscode格式化C++代码，真香~~

#### 顺带学习一下C11/14/17的一些feature

`override`：标识 for virtual function

`Initiallizer list`

```c++
std::vector<int> v{1, 2, 3};
```

`nullptr`

`auto/for`

```c++
for (auto i : vec)	//read only
for (const auto& i : vec)	//read only
for (auto& i : vec)	//changeable
```

`delete`

```c++
fun1(int a)
fun1(double) = delete;	//not allowed
```

`smart pointer`

1. `shared_ptr`

> Every time you assign its value to another shared pointer (usually with a simple assignment), the reference count is incremented. As shared pointers go out of scope (like at the end of a block or function), the reference count is decremented. Once the count goes to zero, the object is deleted.
>
> It makes memory management automatic.
>
> `std::shared_ptr` is included with the ` <memory>` header.

```c++
auto double_ptr = make_shared<double>(0.37);
auto vec3_ptr   = make_shared<vec3>(1.414214, 2.718281, 1.618034);
auto sphere_ptr = make_shared<sphere>(point3(0,0,0), 1.0);
```

```
use_count 返回引用计数的个数
unique 返回是否是独占所有权( use_count 为 1)
swap 交换两个 shared_ptr 对象(即交换所拥有的对象)
reset 放弃内部对象的所有权或拥有对象的变更, 会引起原有对象的引用计数的减少
get 返回内部对象(指针), 由于已经重载了()方法, 因此和直接使用对象是一样的
```

2. `weak_ptr`

> weak_ptr是用来解决shared_ptr相互引用时的死锁问题，如果说两个shared_ptr相互引用，那么这两个指针的引用计数永远不可能下降为0，资源永远不会释放。
>
> 它是对对象的一种弱引用，不会增加对象的引用计数，和shared_ptr之间可以相互转化，shared_ptr可以直接赋值给它，它可以通过调用lock函数来获得shared_ptr。
>
> ```c++
> void fun()
> {
> 	shared_ptr<B> pb(new B());
> 	shared_ptr<A> pa(new A());
> 	cout << pb.use_count() << endl;	//1
> 	cout << pa.use_count() << endl;	//1
> 	pb->pa_ = pa;
> 	pa->pb_ = pb;
> 	cout << pb.use_count() << endl;	//2
> 	cout << pa.use_count() << endl;	//2
> }
> ```
>
> 可以看到fun函数中pa ，pb之间互相引用，两个资源的引用计数为2，当要跳出函数时，智能指针pa，pb析构时两个资源引用计数会减1，但是两者引用计数还是为1。
>
> 如果把其中一个改为weak_ptr就可以了，我们把类A里面的shared\_ptr pb\_，改为weak_ptr pb\_ 
>
> ```c++
> 1
> 1
> 1
> 2
> B delete
> A delete
> ```
>
> 注意：我们不能通过weak_ptr直接访问对象的方法

```
weak_ptr 没有重载*和->但可以使用 lock 获得一个可用的 shared_ptr 对象. 注意, weak_ptr 在使用前需要检查合法性.
expired 用于检测所管理的对象是否已经释放, 如果已经释放, 返回 true; 否则返回 false.
lock 用于获取所管理的对象的强引用(shared_ptr). 如果 expired 为 true, 返回一个空的 shared_ptr; 否则返回一个 shared_ptr, 其内部对象指向与 weak_ptr 相同.
use_count 返回与 shared_ptr 共享的对象的引用计数.
reset 将 weak_ptr 置空.
weak_ptr 支持拷贝或赋值, 但不会影响对应的 shared_ptr 内部对象的计数.
```

3. `unique_ptr`





#### 主要模块

|          |          |                                             |
| -------- | -------- | ------------------------------------------- |
| Vec3     | 三维向量 | (x,y,z)                                     |
| Ray      | 直线     | $\mathbf{P}(t) = \mathbf{A} + t \mathbf{b}$ |
| Hittable | 抽象基类 |                                             |
| Sphere   | 球       | c&r,hit(Ray,self)                           |
|          |          |                                             |
|          |          |                                             |
|          |          |                                             |
|          |          |                                             |
|          |          |                                             |



#### 1.First PPM image

#### 2.A blue-to-white gradient depending on ray Y coordinate

![](img/p1.png)

​	Also, Rendering a red sphere

#### 3.Rendering surface normals on a sphere

​	when a ray hits a sphere,
$$
(\mathbf{P} - \mathbf{C}) \cdot (\mathbf{P} - \mathbf{C}) = r^2
$$

$$
t^2 \mathbf{b} \cdot \mathbf{b}
     + 2t \mathbf{b} \cdot (\mathbf{A}-\mathbf{C})
     + (\mathbf{A}-\mathbf{C}) \cdot (\mathbf{A}-\mathbf{C}) - r^2 = 0
$$



> A common trick used for visualizing normals (because it’s easy and somewhat intuitive to assume $\vec{n}$ is a unit length vector — so each component is between −1 and 1) is to map each component to the interval from 0 to 1, and then map x/y/z to r/g/b.

#### 4.An Abstraction for Hittable Objects

>Most ray tracers have found it convenient to add a valid interval for hits $t_{min}$ to $t_{max}$, so the hit only “counts” if $t_{min}<t<t_{max}$.

#### 5.Front Faces Versus Back Faces&A List of Hittable Objects

![](img/p2.png)

> If the ray and the normal face in the same direction, the ray is inside the object. 
>
> This can be determined by taking the dot product of the two vectors, where if their dot is positive, the ray is inside the sphere.





