/*
 * @Author: your name
 * @Date: 2021-08-19 01:46:18
 * @LastEditTime: 2021-08-19 02:12:57
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /RayTracing/Triangle.h
 */

#pragma once
#include <vector>
#include "Object.h"
#include "Vector.h"

using namespace std;

bool rayTriangleIntersect(const Vector3f& v0, const Vector3f& v1, const Vector3f& v2,
                          const Vector3f& orig, const Vector3f& dir, float& tnear, float& u,
                          float& v) {
  // 射线与三角形求交， https://zhuanlan.zhihu.com/p/73686686
  Vector3f E1 = v1 - v0;
  Vector3f E2 = v2 - v0;
  Vector3f S = orig - v0;
  Vector3f S1 = crossProduct(dir, E2);
  Vector3f S2 = crossProduct(S, E1);

  float norm = dotProduct(S1, E1);
  if (norm == 0) return false;

  tnear = dotProduct(S2, E2) / norm;
  u = dotProduct(S1, S) / norm;
  v = dotProduct(S2, dir) / norm;
  if (tnear > 0 && u > 0 && v > 0 && 1 - u - v > 0) {
    return true;
  }

  return false;
}

class Triangle : public Object {
 public:
  Triangle(const vector<Vector3f>& vertexs, const vector<uint32_t> indexs,
           const vector<Vector2f>& st)
      : vertices_(vertexs), vertex_index_(indexs), st_coordinates_(st) {
    num_triangles_ = vertex_index_.size() / 3;
  }
  virtual ~Triangle() = default;

  virtual bool Intersect(const Vector3f& orig, const Vector3f& dir, float& tnear, uint32_t& index,
                         Vector2f& uv) const override {
    bool intersect = false;
    for (uint32_t k = 0; k < num_triangles_; ++k) {
      const Vector3f& v0 = vertices_[vertex_index_[k * 3]];
      const Vector3f& v1 = vertices_[vertex_index_[k * 3 + 1]];
      const Vector3f& v2 = vertices_[vertex_index_[k * 3 + 2]];
      float t, u, v;
      if (rayTriangleIntersect(v0, v1, v2, orig, dir, t, u, v) && t < tnear) {
        tnear = t;
        uv.x = u;
        uv.y = v;
        index = k;
        intersect |= true;
      }
    }

    return intersect;
  }

  virtual Vector3f GetDiffuseColor(const Vector2f& st) const override {
    float scale = 5;
    float pattern = (fmodf(st.x * scale, 1) > 0.5) ^ (fmodf(st.y * scale, 1) > 0.5);
    return lerp(Vector3f(0.815, 0.235, 0.031), Vector3f(0.937, 0.937, 0.231), pattern);
  }

  /**
   * @description: 返回三角面片中指定三角形的指定坐标的法线与纹理坐标
   * @param {*}
   * @return {*}
   */
  virtual void GetSurfaceProperties(const Vector3f&, const Vector3f&, const uint32_t& index,
                                    const Vector2f& uv, Vector3f& N, Vector2f& st) const override {
    const Vector3f& v0 = vertices_[vertex_index_[index * 3]];
    const Vector3f& v1 = vertices_[vertex_index_[index * 3 + 1]];
    const Vector3f& v2 = vertices_[vertex_index_[index * 3 + 2]];
    Vector3f e0 = normalize(v1 - v0);
    Vector3f e1 = normalize(v2 - v1);
    N = normalize(crossProduct(e0, e1));
    const Vector2f& st0 = st_coordinates_[vertex_index_[index * 3]];
    const Vector2f& st1 = st_coordinates_[vertex_index_[index * 3 + 1]];
    const Vector2f& st2 = st_coordinates_[vertex_index_[index * 3 + 2]];
    st = st0 * (1 - uv.x - uv.y) + st1 * uv.x + st2 * uv.y;
  }

 private:
  vector<Vector3f> vertices_;        // 顶点属性
  uint32_t num_triangles_;           // 三角形数量
  vector<uint32_t> vertex_index_;    // 索引属性
  vector<Vector2f> st_coordinates_;  // 纹理坐标属性
};
