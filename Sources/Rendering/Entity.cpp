﻿#include "Entity.h"
#include "Model.h"

void Entity::Render(Shader& shader) const
{
   if (m_model != nullptr)
   {
      shader.SetMat4("modelMatrix", this->m_transform);
      m_model->Draw(shader);
   }
}
