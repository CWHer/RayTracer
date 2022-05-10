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

#### 1.A Simple Monte Carlo Program

#### 2.One Dimensional MC Integration

#### 3.MC Integration on the Sphere of Directions

蒙特卡洛积分 - chopper的文章 - 知乎 https://zhuanlan.zhihu.com/p/146144853

#### 4.MC Integration on the Sphere of Directions

#### 5.Light Scattering

#### 6.Importance Sampling Materials

#### 7.Generating Random Directions

#### 8.Orthonormal Bases

#### 9.Sampling Lights Directly

#### 10.Mixture Densities

#### 11.Some Architectural Decisions

#### 12.Cleaning Up PDF Management