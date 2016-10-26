#pragma once

#include <ostream>

enum Type {
	Float = 0,
	Color = 1,
	Error = -1
};

std::ostream& operator<<(std::ostream& out, Type type) {
	switch (type) {
	case Type::Float: out << "Float"; break;
	case Type::Color: out << "Color"; break;
	case Type::Error: out << "Error"; break;
	default: out << "Unknown Type"; break;
	}
	return out;
}