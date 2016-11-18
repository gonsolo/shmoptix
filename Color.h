#pragma once

#include <cstdint>
#include <ostream>
#include "global.h"

namespace shmoptix {

class Vector4;

class Color {
public:
	Color() : value{0.f, 0.f, 0.f, 0.f} {}
	Color(float v) : value{v, v, v, v} {}
	Color(float r, float g, float b, float a = 1.f) : value{r, g, b, a} {}
	Color(Vector4 vector);
	~Color() {}
public:
	Color operator*(float f) {
		return Color{ value[0] * f, value[1] * f, value[2] * f };
	}

	Color& operator+=(Color c) {

		value[0] += c.value[0];
		value[1] += c.value[1];
		value[2] += c.value[2];
		value[3] += c.value[3];

		return *this;
	}

	float& operator[](int i) {
		return value[i];
	}
public:
	uint64_t get() { return (uint64_t)&value; }
public:
	friend llvm::raw_ostream& operator<<(llvm::raw_ostream&, const Color&);
private:
	alignas(16) float value[4];
};

class Vector4 {
public:
	Vector4() : value{0.f, 0.f, 0.f, 0.f} {}
	Vector4(float v) : value{v, v, v, v} {}
	Vector4(float x, float y, float z, float w = 0.f) : value{x, y, z, w} {}
	~Vector4() {}
public:
	Vector4 operator/(float v) {
		return Vector4{ value[0] / v, value[1] / v, value[2] / v, value[3] / v };
	}
public:
	uint64_t get() { return (uint64_t)&value; }
public:
	alignas(16) float value[4];
};

Color::Color(Vector4 vector) {
	value[0] = vector.value[0];
	value[1] = vector.value[1];
	value[2] = vector.value[2];
	llvm::outs() << "Constructed Color with " << *this << newline;
}

llvm::raw_ostream& operator<<(llvm::raw_ostream& out, const Color& color) {
	return out << color.value[0] << space << color.value[1] << space << color.value[2];
}

llvm::raw_ostream& operator<<(llvm::raw_ostream& out, const Vector4& vector) {
	return out << vector.value[0] << space << vector.value[1] << space << vector.value[2];
}

}
