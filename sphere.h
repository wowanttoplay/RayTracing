/*
 * @Author: your name
 * @Date: 2021-08-19 00:16:34
 * @LastEditTime: 2021-08-19 01:32:25
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Editude
 * @FilePath: /RayTracing/sphere.h
 */
#pragma once
#include "Object.h"
#include "Vector.h"

class Sphere : public Object {
 private:
  /* data */
  Vector3f center_;
  float radius_;
  float radius2_;  // 半径平方，避免重复计算

 public:
  Sphere(const Vector3f& center, float radius)
      : center_(center), radius_(radius), radius2_(radius * radius) {}
  ~Sphere() = default;

  /**
   * @description:
   * @param {origin, dir, t_near} : 射线起始点， 射线方向，最近的碰撞点
   * @return {BOOL} 是否碰撞
   */
  virtual bool Intersect(const Vector3f& origin, const Vector3f& dir, float& t_near, uint32_t&,
                         Vector2f&) const override {
    //射线与球的第一个交点，参考https://zhuanlan.zhihu.com/p/136763389
    Vector3f l = center_ - origin;
    float s = dotProduct(l, dir);
    float l2 = dotProduct(l, l);
    // 如果球心光起始点向量与射线方向相反，且射线起始点在球外，则两者不可能相交
    if (s < 0 && l2 > radius2_) {
      return false;
    }
    floatm m2 = l2 - s * s;  // 球心与射线的垂直距离
    if (m2 > radius2_) {
      return false;
    }

    float q = std::sqrt(radius2_ - m2);
    if (l2 > radius2_) {
      t_near = s - q;
    } else {
      t_near = s + q;
    }
    return true;
  }
  /**
   * @description:
   * @param {const} Vector3f 球面某点
   * @return {*} 返回球面法向量
   */
  virtual Vector3f GetNormal(const Vector3f& p) { return normalize(p - center_); }
};
