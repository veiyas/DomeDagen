#ifndef GAMEOBJECT_H_
#define GAMEOBJECT_H_

#include "renderable.hpp"
#include "sgct/shadermanager.h"
#include "sgct/shaderprogram.h"
#include <vector>
#include <string>

class GameObject : public Renderable {
public:
    GameObject() = default;
    virtual ~GameObject() { };  // hur är det med virtuella destructors nu igen
  
    void render() const override;

protected:
  //nån slags position
  //const sgct::ShaderProgram prg_;
};

#endif