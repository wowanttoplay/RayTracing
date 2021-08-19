/*
 * @Author: your name
 * @Date: 2021-08-19 12:21:28
 * @LastEditTime: 2021-08-19 13:17:22
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /RayTracing/Render.h
 */
#pragma once
#include"Scene.h"

struct hit_payload
{
    float tNear;
    uint32_t index;
    Vector2f uv;
    Object* hit_obj;
};

class Renderer
{
public:
    void Render(const Scene& scene);

private:
};