# RayTracer

code for [RayTracingTheRestofYourLife](https://github.com/RayTracing/raytracing.github.io/)

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
| AABB           | Axis-Aligned Bounding Boxes |                                             |
| BVH            |                             |                                             |
| AARect         | Axis-Aligned rect           |                                             |
| Box            |                             |                                             |
| ConstantMedium |                             |                                             |
| Camera         |                             | orig/LDcorner/vertical/horiontal...         |
| Material       | 材质抽象基类                | produce scattered ray/emit                  |
| Lambertian     | diffuse                     |                                             |
| Metal          | mirrored reflect            |                                             |
| Dielectric     | refract                     |                                             |
| Isotropic      |                             |                                             |
| Texture        | 纹理抽象基类                |                                             |
| SolidColor     |                             |                                             |
| CheckerTexture |                             |                                             |
| NoiseTexture   |                             |                                             |
| IMGTexture     |                             |                                             |
| Perlin         |                             |                                             |


