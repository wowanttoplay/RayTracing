/*
 * @Author: your name
 * @Date: 2021-08-19 00:08:04
 * @LastEditTime: 2021-08-19 00:50:17
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /RayTracing/object.h
 */
#pragma once
#include "vector.h"

class Object {
 public:
  Object() = default;

  virtual ~Object() = default;

  /**
   * @description: 判断光线与物体的碰撞，返回相关值
   * @param {const} Vector3f
   * @param {const} Vector3f
   * @return {*}
   */
  virtual bool Intersect(const Vector3f&, const Vector3f&, float&, uint32_t&, Vector2f&) const = 0;

  /**
   * @description: 返回物体对应位置的法向量
   * @param {const} Vector3f
   * @return {*}
   */
  virtual Vector3f GetNormal(const Vector3f&) = 0;

  /**
   * @description: 返回对应纹理坐标的漫反射颜色
   * @param {const} Vector2f 纹理坐标
   * @return {*}
   */
  virtual Vector3f GetDiffuseColor(const Vector2f&) { return diffuse_color; }

 public:
  float ior = 1.2;                         // 折射率
  float kd = 0.8;                          // 漫反射系数
  float ks = 0.2;                          // 镜反射系数
  Vector3f diffuse_color = Vector3f(0.2);  // 反射颜色
  float specular_exponent = 64.0f;         // 镜反射幂次
  float energy_attenuation = 0.3f;         //能量衰减系数
};