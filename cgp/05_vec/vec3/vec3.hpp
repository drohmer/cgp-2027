#pragma once

#include "cgp/02_numarray/numarray_stack/numarray_stack.hpp"

#include <iostream>


namespace cgp
{

	// vec3 is an alias on buffer_stack<float, 3>
	// Its implementation can be found in file cgp/containers/buffer_stack/buffer_stack3.hpp
	using vec3 = numarray_stack3<float>;

	// A vec3 struct can be used as
	//   struct vec3 { float x, y, z; }
	//   (with additional functions handled as a buffer_stack)

	inline float dot(vec3 const& a, vec3 const& b);
	inline float norm(vec3 const& p);
	inline vec3 cross(vec3 const& a, vec3 const& b);
}

namespace cgp
{

	inline float dot(vec3 const& a, vec3 const& b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	inline float norm(vec3 const& p) {

		return std::sqrt(p.x * p.x + p.y * p.y + p.z * p.z);
	}

	
	inline vec3 cross(vec3 const& a, vec3 const& b)
	{
		return vec3(
			a.y * b.z - a.z * b.y,
			a.z * b.x - a.x * b.z,
			a.x * b.y - a.y * b.x
		);
	}



}