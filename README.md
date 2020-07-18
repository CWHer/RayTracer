# RayTracer

code for [RayTracingTheNextWeek](https://github.com/RayTracing/raytracing.github.io/)

[TOC]

#### 主要模块

| name         |                  | members                                     |
| ------------ | ---------------- | ------------------------------------------- |
| Vec3         | 三维向量         | (x,y,z)                                     |
| Ray          | 直线             | $\mathbf{P}(t) = \mathbf{A} + t \mathbf{b}$ |
| Hittable     | 可碰撞抽象基类   | hit                                         |
| Sphere       | 球               | c&r,hit(Ray,self)                           |
| MovingSphere |                  |                                             |
| HittableList |                  |                                             |
| Camera       |                  | orig/LDcorner/vertical/horiontal...         |
| Material     | 材质抽象基类     | produce scattered ray                       |
| Lambertian   | diffuse          |                                             |
| Metal        | mirrored reflect |                                             |
| Dielectric   | refract          |                                             |




#### 1.Motion Blur

> In a real camera, the shutter opens and stays open for a time interval, and the camera and objects may move during that time. Its really an average of what the camera sees over that interval that we want.


