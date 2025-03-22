#include "Point.h"

std::ostream& operator << (std::ostream& stream, const Point& point) {
  return stream << '(' << point.lat << ", " << point.lng << ')';
}

std::istream& operator >> (std::istream& stream, Point& point) {
  return stream >> point.lat >> point.lng;
}

bool operator==(const Point& other, const Point& current){
	return current.lat == other.lat && current.lng == other.lng; 
}
