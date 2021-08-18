/*
 * @Author: your name
 * @Date: 2021-08-19 02:14:41
 * @LastEditTime: 2021-08-19 02:23:08
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /RayTracing/Scene.h
 */

#pragma once
#include <memory>
#include <vector>
#include "Light.h"
#include "Object.h"
#include "Vector.h"
using namespace std;
class Scene {
 private:
  /* data */
  vector<unique_ptr<Object>> objects_;
  vector<unique_ptr<Light>> lights_;

 public:
  Scene(uint32_t w, uint32_t h) : width(w), height(h) {}

  ~Scene() = default;

  void Add(unique_ptr<Object> object) { objects_.push_back(move(object)); }

  void Add(unique_ptr<Light> light) { lights_.push_back(move(light)); }

  [[nodiscard]] const vector<unique_ptr<Object>>& GetObjects() const { return objects_; }

  [[nodiscard]] const vector<unique_ptr<Light>>& GetLights() const { return lights_; }

 public:
  uint32_t width = 1960, height = 1080;
  double fov = 90;
  Vector3f background _color = Vector3f(0.2f);
  uint32_t max_depth = 5;
  float epsilon = 0.00001f;
};
