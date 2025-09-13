#ifndef COMPONENT_HPP
#define COMPONENT_HPP

// ================================================================================================
// COMPONENTS
// ================================================================================================
// Add new Components here if necessary;

struct Position {
  double x, y;
};

struct Velocity {
  double dx, dy;
};

struct Sprite {
  std::string TextureName;
};


#endif // COMPONENT_HPP
