#include "gameobject.h"
#include <sgct/sgct.h>

void GameObject::Render() const {

}

void GameObject::Update(float delta_time) {
  position_ += delta_time * velocity_;
}
