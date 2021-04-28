#include "Scene.h"
#include "Entity.h"
#include "Light.h"
#include "Camera.h"

Scene::~Scene()
{
   for (auto& entity : m_entities)
   {
      if (entity != nullptr)
      {
         delete entity;
         entity = nullptr;
      }
   }

   for (auto& light : m_lights)
   {
      if (light != nullptr)
      {
         delete light;
         light = nullptr;
      }
   }

   for (auto& camera : m_cameras)
   {
       if (camera != nullptr)
       {
           delete camera;
           camera = nullptr;
       }
   }
}

Entity& Scene::CreateEntity(const std::string_view name)
{
   const auto temp = new Entity(name);
   m_entities.push_back(temp);
   return (*temp);
}

Light& Scene::CreateLight()
{
   const auto temp = new Light();
   m_lights.push_back(temp);
   return (*temp);
}

Camera& Scene::CreateCamera()
{
    const auto temp = new Camera(glm::vec3(0.0f, 0.0f, 0.0f));
    m_cameras.push_back(temp);
    return (*temp);
}