#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include <glm/vec2.hpp>

#include "renderable.h"

class GameObject : public Renderable {
 public:
  virtual ~GameObject(){};  // hur är det med virtuella destructors nu igen

  void Render() const override;
  virtual void Update(float delta_time);

  // accessors
  const glm::vec2& get_position() const { return position_; };
  const glm::vec2& get_velocity() const { return velocity_; };
  float get_orientation() const { return orientation_; };
  float get_scale() const { return scale_; };

  // mutators
  void set_position(const glm::vec2& position) { position_ = position; };
  void set_velocity(const glm::vec2& velocity) { velocity_ = velocity; };
  void set_orientation(float orientation) { orientation_ = orientation; };
  void set_scale(float scale) { scale_ = scale; };

 protected:
  glm::vec2 position_;  // oklart om detta är rätt typ av position eg
  glm::vec2 velocity_;  // eg borde ju speed räcka i kombination med orientation
  float orientation_;   // in radians
  float scale_;         // uniform in all directions
};

#endif