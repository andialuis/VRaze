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
 
#include "car_physics.h"


namespace {

inline mathfu::Matrix<float, 2> Rotation2D(float angle) {
  float x = cosf(angle);
  float y = sinf(angle);
  return {x, y, -y, x};
};

constexpr float kSteeringSpeedCompensationRatio = 0.12f;

}  // namespace

constexpr float CarPhysics::WEIGHT = 1500.0f;
constexpr float CarPhysics::LENGTH = 2.5f;
constexpr float CarPhysics::MIN_TRACTION = 1000.0f;
constexpr float CarPhysics::MAX_TRACTION = 25000.0f;
constexpr float CarPhysics::TRACTION_INCREASE = 2500.0f;
constexpr float CarPhysics::DRAG_RATIO = 8.0f;
constexpr float CarPhysics::ROAD_FRICTION = 300.0f;
constexpr float CarPhysics::GROUND_FRICTION = 2000.0f;
constexpr float CarPhysics::BRAKING = 2000.0f;
constexpr float CarPhysics::STEERING_RATIO = 0.15f;


CarPhysics::CarPhysics(const mathfu::vec2 position, RoadDescriptor* road_descriptor)
    : position_(position), traction_(MIN_TRACTION), road_descriptor_(road_descriptor) {
}


void CarPhysics::Move(float delta_time, bool accelerating, bool braking, float steering_wheel_angle) {
  position_ += direction_ * speed_ * delta_time;

  UpdateDirection(delta_time, steering_wheel_angle);
  UpdateSpeed(delta_time, accelerating, braking);
}


void CarPhysics::UpdateDirection(float delta_time, float steering_wheel_angle) {
  float steering_angle = steering_wheel_angle * STEERING_RATIO;
  if (steering_angle == 0.0f)
    return;
  float turn_radius = LENGTH / sinf(steering_angle);
  float w = speed_ / turn_radius;
  w /= 1.0f + (fabsf(speed_) * kSteeringSpeedCompensationRatio);
  direction_ = (Rotation2D(w * delta_time) * direction_).Normalized();
}


void CarPhysics::UpdateSpeed(float delta_time, bool accelerating, bool braking) {
  float force = -DRAG_RATIO * speed_ * speed_ - GetFriction() * speed_;
  if (accelerating) {
    traction_ += delta_time * TRACTION_INCREASE;
    traction_ = std::min(traction_, MAX_TRACTION);
    force += traction_;
  } else {
    traction_ -= delta_time * TRACTION_INCREASE;
    traction_ = std::max(traction_, MIN_TRACTION);
  }
  if (braking) {
    force -= BRAKING;
  }
  speed_ += force / WEIGHT * delta_time;
}


float CarPhysics::GetFriction() const {
  return road_descriptor_->IsInside(position_) ? ROAD_FRICTION : GROUND_FRICTION;
}
