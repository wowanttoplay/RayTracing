/*
 * @Author: your name
 * @Date: 2021-08-19 12:21:45
 * @LastEditTime: 2021-08-19 14:19:09
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /RayTracing/Render.cc
 */
#include "Render.h"
#include <fstream>
#include <optional>
#include "Scene.h"
#include "Vector.h"
using namespace std;

inline float deg2rad(const float &deg) { return deg * M_PI / 180.0; }

inline float clamp(const float &lo, const float &hi, const float &v) {
  return std::max(lo, std::min(hi, v));
}
// Compute reflection direction
Vector3f reflect(const Vector3f &I, const Vector3f &N) { return I - 2 * dotProduct(I, N) * N; }

Vector3f refract(const Vector3f &I, const Vector3f &N, const float &ior) {
  float cosi = clamp(-1, 1, dotProduct(I, N));
  float etai = 1, etat = ior;
  Vector3f n = N;
  if (cosi < 0) {
    cosi = -cosi;
  } else {
    std::swap(etai, etat);
    n = -N;
  }
  float eta = etai / etat;
  float k = 1 - eta * eta * (1 - cosi * cosi);
  return k < 0 ? 0 : eta * I + (eta * cosi - sqrtf(k)) * n;
}

// 计算反射比率
float fresnel(const Vector3f &I, const Vector3f &N, const float &ior) {
  float cosi = clamp(-1, 1, dotProduct(I, N));
  float etai = 1, etat = ior;
  if (cosi > 0) {
    std::swap(etai, etat);
  }
  // Compute sini using Snell's law
  float sint = etai / etat * sqrtf(std::max(0.f, 1 - cosi * cosi));
  // Total internal reflection
  if (sint >= 1) {
    return 1;
  } else {
    float cost = sqrtf(std::max(0.f, 1 - sint * sint));
    cosi = fabsf(cosi);
    float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
    float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
    return (Rs * Rs + Rp * Rp) / 2;
  }
  // As a consequence of the conservation of energy, transmittance is given by:
  // kt = 1 - kr;
}

std::optional<hit_payload> trace(const Vector3f &orig, const Vector3f &dir,
                                 const std::vector<std::unique_ptr<Object> > &objects) {
  float tNear = numeric_limits<float>::max();
  std::optional<hit_payload> payload;
  for (const auto &object : objects) {
    float tNearK = numeric_limits<float>::max();
    uint32_t indexK;
    Vector2f uvK;
    if (object->Intersect(orig, dir, tNearK, indexK, uvK) && tNearK < tNear) {
      payload.emplace();
      payload->hit_obj = object.get();
      payload->tNear = tNearK;
      payload->index = indexK;
      payload->uv = uvK;
      tNear = tNearK;
    }
  }

  return payload;
}

Vector3f calculateBlinnPhongColor(const Vector3f &orig, const Vector3f &dir, const Scene &scene,
                                  int depth, const std::optional<hit_payload> &payload) {
  Vector3f hitColor = scene.background_color;
  Vector3f hitPoint = orig + dir * payload->tNear;
  Vector3f N;   // normal
  Vector2f st;  // st coordinates
  payload->hit_obj->GetSurfaceProperties(hitPoint, dir, payload->index, payload->uv, N, st);
  Vector3f lightAmt = 0, specularColor = 0;
  Vector3f shadowPointOrig =
      (dotProduct(dir, N) < 0) ? hitPoint + N * scene.epsilon : hitPoint - N * scene.epsilon;
 
  for (auto &light : scene.GetLights()) {
    Vector3f lightDir = light->position_ - hitPoint;
    // square of the distance between hitPoint and the light
    float lightDistance2 = dotProduct(lightDir, lightDir);
    lightDir = normalize(lightDir);
    float LdotN = std::max(0.f, dotProduct(lightDir, N));
    // is the point in shadow, and is the nearest occluding object closer to the object than
    // the light itself?
    auto shadow_res = trace(shadowPointOrig, lightDir, scene.GetObjects());
    bool inShadow = shadow_res && (shadow_res->tNear * shadow_res->tNear < lightDistance2);

    lightAmt += inShadow ? 0 : light->intensity_ * LdotN;
    Vector3f reflectionDirection = reflect(-lightDir, N);

    specularColor += powf(std::max(0.f, -dotProduct(reflectionDirection, dir)),
                          payload->hit_obj->specular_exponent) *
                     light->intensity_;
  }

  hitColor = lightAmt * payload->hit_obj->GetDiffuseColor(st) * payload->hit_obj->kd +
             specularColor * payload->hit_obj->ks;
  return hitColor;
}

Vector3f castRay(const Vector3f &orig, const Vector3f &dir, const Scene &scene, int depth) {
  if (depth > scene.max_depth) {
    // return Vector3f(0.0, 0.0, 0.0);
    return scene.background_color;
  }

  Vector3f hitColor = scene.background_color;
  if (auto payload = trace(orig, dir, scene.GetObjects()); payload) {
    Vector3f hitPoint = orig + dir * payload->tNear;
    Vector3f N;   // normal
    Vector2f st;  // st coordinates
    payload->hit_obj->GetSurfaceProperties(hitPoint, dir, payload->index, payload->uv, N, st);

    Vector3f reflectionDirection = normalize(reflect(dir, N));
    Vector3f refractionDirection = normalize(refract(dir, N, payload->hit_obj->ior));
    Vector3f reflectionRayOrig = (dotProduct(reflectionDirection, N) < 0)
                                     ? hitPoint - N * scene.epsilon
                                     : hitPoint + N * scene.epsilon;
    Vector3f refractionRayOrig = (dotProduct(refractionDirection, N) < 0)
                                     ? hitPoint - N * scene.epsilon
                                     : hitPoint + N * scene.epsilon;
    Vector3f reflectionColor = castRay(reflectionRayOrig, reflectionDirection, scene, depth + 1);
    Vector3f refractionColor = castRay(refractionRayOrig, refractionDirection, scene, depth + 1);
    float kr = fresnel(dir, N, payload->hit_obj->ior);
    hitColor = reflectionColor * kr + refractionColor * (1 - kr);
    hitColor = calculateBlinnPhongColor(orig, dir, scene, depth, payload) +
               payload->hit_obj->energy_attenuation * hitColor;
    // hitColor +=
    //     (payload->hit_obj->energy_attenuation) * calculateBlinnPhongColor(orig, dir, scene,
    //     depth, payload) + (1 - payload->hit_obj->energy_attenuation) * hitColor;
  }

  return hitColor;
}

void Renderer::Render(const Scene &scene) {
  std::vector<Vector3f> framebuffer(scene.width * scene.height);

  float scale = std::tan(deg2rad(scene.fov * 0.5f));
  float imageAspectRatio = scene.width / (float)scene.height;

  // Use this variable as the eye position to start your rays.
  Vector3f eye_pos(0);
  int m = 0;
  for (int j = 0; j < scene.height; ++j) {
    for (int i = 0; i < scene.width; ++i) {
      // generate primary ray direction
      // float x;
      // float y;
      // y = (1.0f - (2.0f * (float(j) + 0.5f) / scene.height)) * scale;
      // x = (2 * (float(i) + 0.5f) / scene.width - 1.0f) * scale * imageAspectRatio;

      // Vector3f dir = Vector3f(x, y, -1);  // Don't forget to normalize this direction!
      // dir = normalize(dir);
      // framebuffer[m++] = castRay(eye_pos, dir, scene, 0);
      Vector3f color;
      vector<float> t{0.25, 0.75};
      // vector<float> t{0.5};
      for (int k = 0; k < t.size(); ++k) {
        for (int l = 0; l < t.size(); ++l) {
          float x;
          float y;
          y = (1.0f - (2.0f * (float(j) + t[k]) / scene.height)) * scale;
          x = (2 * (float(i) + t[l]) / scene.width - 1.0f) * scale * imageAspectRatio;

          Vector3f dir = Vector3f(x, y, -1);  // Don't forget to normalize this direction!
          dir = normalize(dir);
          color += castRay(eye_pos, dir, scene, 0);
        }
      }
      framebuffer[m++] = color / t.size();
    }
  }

  // save framebuffer to file
  FILE *fp = fopen("result.ppm", "wb");
  (void)fprintf(fp, "P6\n%d %d\n255\n", scene.width, scene.height);
  for (auto i = 0; i < scene.height * scene.width; ++i) {
    static unsigned char color[3];
    /* code */
    // float exposure = 1.0f;
    // framebuffer[i].x = 1.0f - std::exp(-framebuffer[i].x * exposure);
    // framebuffer[i].y = 1.0f - std::exp(-framebuffer[i].y * exposure);
    // framebuffer[i].z = 1.0f - std::exp(-framebuffer[i].z * exposure);

    color[0] = (unsigned char)(255 * clamp(0, 1, framebuffer[i].x));
    color[1] = (unsigned char)(255 * clamp(0, 1, framebuffer[i].y));
    color[2] = (unsigned char)(255 * clamp(0, 1, framebuffer[i].z));
    fwrite(color, 1, 3, fp);
  }
  fclose(fp);
}
