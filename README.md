# RayTracer

code for [RayTracingTheNextWeek](https://github.com/RayTracing/raytracing.github.io/)

[TOC]

#### 主要模块

| name           |                             | members                                     |
| -------------- | --------------------------- | ------------------------------------------- |
| Vec3           | 三维向量                    | (x,y,z)                                     |
| Ray            | 直线                        | $\mathbf{P}(t) = \mathbf{A} + t \mathbf{b}$ |
| Hittable       | 可碰撞抽象基类              | hit_record/hit/bounding_box/Instances       |
| FlipFace       | flip normal                 |                                             |
| Sphere         | 球                          | c&r,hit(Ray,self)                           |
| MovingSphere   |                             |                                             |
| HittableList   |                             |                                             |
| Camera         |                             | orig/LDcorner/vertical/horiontal...         |
| Material       | 材质抽象基类                | produce scattered ray/emit                  |
| Lambertian     | diffuse                     |                                             |
| Metal          | mirrored reflect            |                                             |
| Dielectric     | refract                     |                                             |
| AABB           | Axis-Aligned Bounding Boxes |                                             |
| BVH            |                             |                                             |
| AARect         | Axis-Aligned rect           |                                             |
| Box            |                             |                                             |
| Texture        | 纹理抽象基类                |                                             |
| SolidColor     |                             |                                             |
| CheckerTexture |                             |                                             |
| NoiseTexture   |                             |                                             |
| IMGTexture     |                             |                                             |
| Perlin         |                             |                                             |



#### 1.Motion Blur

> In a real camera, the shutter opens and stays open for a time interval, and the camera and objects may move during that time. Its really an average of what the camera sees over that interval that we want.

#### 2.Bounding Volume Hierarchies

> - idea 
>
> A key thing is we are dividing objects into subsets. We are not dividing the screen or the volume. Any object is in just one bounding volume, but bounding volumes can overlap.

![](img/p1.png)

> - Axis-Aligned Bounding Boxes
>
> $$
> t_x\cap t_y \cap t_z \neq \phi
> $$
>
> NaN should be take care of.

![](img/p2.png)

![](img/p3.png)



> - BVH Node
>
> Note that the children pointers are to generic hittables. They can be other BVHnodes, or spheres, or any other hittable. A brilliant trick.
>
> - Splitting BVH Volumes
>
> 1. randomly choose an axis
> 2. sort the primitives (`using std::sort`)
> 3. put half in each subtree
>
> When the list coming in is two elements, I put one in each subtree and end the recursion. The traversal algorithm should be smooth and not have to check for null pointers, so if I just have one element I duplicate it in each subtree.
>
> The check for whether there is a bounding box at all is in case you sent in something like an infinite plane that doesn’t have a bounding box.

#### 3.Solid Textures

> - spherical coordinates
>
>   `another version`
>
> ![](img/p4.png)
> $$
> u = \frac{\theta+\pi}{2\pi}
> $$
>
> $$
> v = \frac{\varphi}{\pi}
> $$
>
> - Checker Texture
>
> We can create a checker texture by noting that the sign of sine and cosine just alternates in a regular way, and if we multiply trig functions in all three dimensions, the sign of that product forms a 3D checker pattern.

#### 4.Perlin Noise

- [ ] why does it work?

> - Smoothing out the Result
>
>   linearly interpolate
>
> - Hermitian Smoothing
>
>   use a Hermite cubic to round off the interpolation
>
> - Tweaking The Frequency
>
> - Using Random Vectors on the Lattice Points
>
>   use a dot product to move the min and max off the lattice
>
> - Turbulence
>
>   a composite noise that has multiple summed frequencies
>
>   The basic idea is to make color proportional to something like a sine function, and use turbulence to adjust the phase (so it shifts x in sin(x)) which makes the stripes undulate.

#### 5.Image Texture Mapping

> $$
> u = \frac{i}{N_x-1}
> $$
>
> $$
> v = \frac{j}{N_y-1}
> $$
>
> - notice
>
>   image_data is left upper corner.
>
>   $\theta=-\pi /2 \rightarrow u=0 $

#### 6.Rectangles and Lights

#### 7.Instances

>  instead we move the rays in the opposite direction

![](img/p5.png)

> Unlike the situation with translations, the surface normal vector also changes, so we need to transform directions too if we get a hit.

![](img/p6.png)