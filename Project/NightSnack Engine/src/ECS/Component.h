#ifndef COMPONENT_HPP
#define COMPONENT_HPP

// ================================================================================================
// COMPONENTS
// ================================================================================================

struct Position {
  double x, y;
};

struct Velocity {
  double dx, dy;
};

struct Sprite {
  std::string TextureName;
};

struct Color {
	float r, g, b, a;
};

// Add more Components here if necessary...

#endif // COMPONENT_HPP
