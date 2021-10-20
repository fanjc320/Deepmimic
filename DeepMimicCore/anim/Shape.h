#pragma once

#include <string>

class cShape
{
public:
	enum eShape
	{
		eShapeNull,//0
		eShapeBox,//1
		eShapeCapsule,//2
		eShapeSphere,//3
		eShapeCylinder,
		eShapePlane,
		eShapeMax,
	};

	static bool ParseShape(const std::string& str, cShape::eShape& out_shape);
};