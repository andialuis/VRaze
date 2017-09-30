/*
 * Copyright 2017 Aldo Culquicondor
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "scene.h"


namespace {

const float kGroundDepth = -1.0f;

const mathfu::mat4 kSteeringModelMatrix =
    mathfu::mat4::FromTranslationVector({0.0f, -0.3f, -0.4f}) *
        mathfu::mat4::FromRotationMatrix(mathfu::mat4::RotationX(-0.3f));

}  // namespace


Scene::Scene(fplbase::AssetManager* asset_manager)
    : ground_(asset_manager), steering_(asset_manager) {
  asset_manager->StartLoadingTextures();
}


void Scene::Render(fplbase::Renderer* renderer,
                   const mathfu::mat4& view_projection_matrix,
                   const Car& car,
                   float steering_rotation) {
  ground_.Render(renderer,view_projection_matrix *
      mathfu::mat4::FromRotationMatrix(mathfu::mat4::RotationY({car.GetDirection().x, -car.GetDirection().y})) *
      mathfu::mat4::FromTranslationVector(
          {car.GetPosition().y, kGroundDepth, car.GetPosition().x}));
  steering_.Render(renderer,
                   view_projection_matrix * kSteeringModelMatrix *
                       mathfu::mat4::FromRotationMatrix(
                           mathfu::mat4::RotationZ(steering_rotation)));
}
