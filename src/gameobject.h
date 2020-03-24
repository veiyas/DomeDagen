#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include "renderable.h"
#include "sgct/shadermanager.h"
#include "sgct/shaderprogram.h"
#include <vector>
#include <string>

class GameObject : public Renderable {
public:
    GameObject() = default;
  virtual ~GameObject() { };  // hur är det med virtuella destructors nu igen
  
  void Render() const override;

 protected:
  //nån slags position
  //const sgct::ShaderProgram prg_;
};

#endif