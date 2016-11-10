#pragma once

#include <cstdint>
#include <ostream>
#include "global.h"

namespace shmoptix {

class Vector3;

class Color {
public:
	Color() : value{0.f, 0.f, 0.f} {}
	Color(float v) : value{v, v, v} {}
	Color(float r, float g, float b) : value{r, g, b} {}
	Color(Vector3 vector);
	~Color() {}
public:
	Color operator*(float f) {
		return Color{ value[0] * f, value[1] * f, value[2] * f };
	}

	Color& operator+=(Color c) {
		value[0] += c.value[0];
		value[1] += c.value[1];
		value[2] += c.value[2];
		return *this;
	}
public:
	uint64_t get() { return (uint64_t)value; }
public:
	friend llvm::raw_ostream& operator<<(llvm::raw_ostream&, const Color&);
private:
	float value[3];
};

class Vector3 {
public:
	Vector3() : value{0.f, 0.f, 1.f} {}
	Vector3(float v) : value{v, v, v} {}
	Vector3(float x, float y, float z) : value{x, y, z} {}
	~Vector3() {}
public:
	Vector3 operator/(float v) {
		return Vector3{ value[0] / v, value[1] / v, value[2] / v };
	}
public:
	uint64_t get() { return (uint64_t)value; }
public:
	float value[3];
};

Color::Color(Vector3 vector) {
	value[0] = vector.value[0];
	value[1] = vector.value[1];
	value[2] = vector.value[2];
	llvm::outs() << "Constructed Color with " << *this << newline;
}

llvm::raw_ostream& operator<<(llvm::raw_ostream& out, const Color& color) {
	return out << color.value[0] << space << color.value[1] << space << color.value[2];
}

llvm::raw_ostream& operator<<(llvm::raw_ostream& out, const Vector3& vector) {
	return out << vector.value[0] << space << vector.value[1] << space << vector.value[2];
}

}
