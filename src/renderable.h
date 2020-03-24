#ifndef RENDERABLE_H_
#define RENDERABLE_H_

class Renderable {
 public:
  virtual ~Renderable() { };
  virtual void Render() const = 0;
};

#endif