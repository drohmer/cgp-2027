#pragma once

#include "cgp/09_geometric_transformation/rotation_transform/rotation_transform.hpp"

namespace cgp
{
	struct affine_rts;
	struct affine_rt;

	// ( rotation * scaling * diag(scaling_xyz) | translation )
	// (                 0                      |      1      )
	//  T * p = rotation * (scaling * scaling_xyz * p) + translation
	//  scaling_xyz stretches the local axes of the shape, then the rotation is applied.
	struct affine
	{
		rotation_transform rotation;
		vec3 translation;
		float scaling;
		vec3 scaling_xyz;

		affine();
		affine(affine_rt const& T);
		affine(affine_rts const& T);
		explicit affine(rotation_transform const& rotation, vec3 const& translation=vec3{0,0,0}, float scaling = 1.0f, vec3 const& scaling_xyz = vec3{1,1,1});

		mat4 matrix() const;

		static affine from_matrix(mat4 const& M);

		affine& set_scaling(float value);
		affine& set_scaling_xyz(vec3 const& xyz);
		affine& set_translation(vec3 const& xyz);
		affine& set_rotation(rotation_transform const& r);
	};

	vec3 operator*(affine const& T, vec3 const& p);
	vec4 operator*(affine const& T, vec4 const& p);

	// Exact composition: a rigid or similarity transform (parent) applied after an affine (child).
	affine operator*(affine_rts const& T1, affine const& T2);
	affine operator*(affine_rt const& T1, affine const& T2);
	// An affine applied after a rigid or similarity transform is representable only if
	//  T1.scaling_xyz is uniform (asserted). Use T1.matrix()*T2.matrix() otherwise.
	affine operator*(affine const& T1, affine_rts const& T2);
	affine operator*(affine const& T1, affine_rt const& T2);
	
	mat4 operator*(affine const& T1, mat4 const& T2);
	mat4 operator*(mat4 const& T1, affine const& T2);
	mat4 operator*(affine const& T1, mat3 const& T2);
	mat4 operator*(mat3 const& T1, affine const& T2);

	// Note: no inverse(affine) is provided on purpose.
	//  With a non-uniform scaling_xyz, the affine struct (which always applies the scaling
	//  before the rotation, i.e. R*diag(s_xyz)) cannot exactly represent its own inverse
	//  diag(1/s_xyz)*R^T. Rather than returning a silently wrong result, the inverse is not
	//  offered: use affine_rt / affine_rts (whose inverse is always exact) when an inverse is needed.

	std::string type_str(affine const& );
	std::string str(affine const& T);
	std::ostream& operator<<(std::ostream& s, affine const& T);

}