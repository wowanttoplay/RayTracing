/*
 * @Author: your name
 * @Date: 2021-08-19 00:13:57
 * @LastEditTime: 2021-08-19 01:41:19
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /RayTracing/Light.h
 */
#pragma once

#include "Vector.h"

class Light {
 public:
  Light(const Vector3f& position, const float& intensity)
      : position_(position), intensity_(intensity) {}

 private:
  Vector3f position_;
  Vector3f intensity_;
};