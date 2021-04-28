#pragma once
#include <vector>
#include <string>

class Entity;
class Light;
class Camera;
class Scene
{
public:
   Scene() = default;
   virtual ~Scene();

   virtual void Init() = 0;

   Entity& CreateEntity(const std::string_view name);
   Light& CreateLight();
   Camera& CreateCamera();

   [[nodiscard]]
   const std::vector<Entity*>& GetEntities() const { return m_entities; }
   [[nodiscard]]
   const std::vector<Light*>& GetLights() const { return m_lights; }
   [[nodiscard]]
   const std::vector<Camera*>& GetCameras() const { return m_cameras; }

private:
   std::vector<Entity*> m_entities;
   std::vector<Light*> m_lights;
   std::vector<Camera*> m_cameras;
};