/*
 * @Author: your name
 * @Date: 2021-08-13 11:39:39
 * @LastEditTime: 2021-08-19 13:44:24
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /RayTracing/main.cpp
 */
#include <iostream>
#include <memory>
#include "Light.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Vector.h"
#include"Scene.h"
#include"Render.h"
#include<vector>
using namespace std;

int main(int, char**) {Scene scene(1280, 960);

  auto sph1 = std::make_unique<Sphere>(Vector3f(2, 0, -12), 1);
  sph1->energy_attenuation = 0.9;
  sph1->diffuse_color = Vector3f(0.0, 0.0, 1.0);

  auto sph2 = std::make_unique<Sphere>(Vector3f(0.5, -0.5, -8), 1.5);
  sph2->ior = 1.5;
  sph2->energy_attenuation = 0.1;
  sph2->diffuse_color = Vector3f(0.0, 1.0, 0.0);

  auto sph3 = std::make_unique<Sphere>(Vector3f(-1, 0, -16), 2);
  sph3->energy_attenuation = 0.9;
  sph3->diffuse_color = Vector3f(1.0, 0.0, 0.0);

  scene.Add(std::move(sph1));
  scene.Add(std::move(sph2));
  scene.Add(std::move(sph3));

    vector<Vector3f> verts{{-5, -3, -6}, {5, -3, -6}, {5, -3, -16}, {-5, -3, -16}};
    vector<uint32_t> vertIndex{0, 1, 3, 1, 2, 3};
    vector<Vector2f> st{{0, 0}, {1, 0}, {1, 1}, {0, 1}};
  auto mesh = std::make_unique<Triangle>(verts, vertIndex, st);
  mesh->energy_attenuation = 0.8;

  scene.Add(std::move(mesh));
  scene.Add(std::make_unique<Light>(Vector3f(-20, 70, 20), 0.5));
  scene.Add(std::make_unique<Light>(Vector3f(30, 50, -12), 0.5));

  Renderer r;
  r.Render(scene);
  // r.Render(scene);

  return 0;}
