#pragma once

#include <cstdint>
#include <ostream>
#include "global.h"

namespace shmoptix {

class Color {
public:
	Color() : value{0.f, 0.f, 0.f} {}
	Color(float v) : value{v, v, v} {}
	Color(float r, float g, float b) : value{r, g, b} {}
	~Color() {}
public:
	uint64_t get() { return (uint64_t)value; }
public:
	friend std::ostream& operator<<(std::ostream&, const Color&);
private:
	float value[3];
};


std::ostream& operator<<(std::ostream& out, const Color& color) {
	return out << color.value[0] << space << color.value[1] << space << color.value[2];
}

}
