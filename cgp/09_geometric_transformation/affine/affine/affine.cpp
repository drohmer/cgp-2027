#include "cgp/01_base/base.hpp"
#include "affine.hpp"

#include "../affine_rt/affine_rt.hpp"
#include "../affine_rts/affine_rts.hpp"

namespace cgp
{
	affine::affine()
		:rotation(), translation(), scaling(1.0f), scaling_xyz({ 1.0f, 1.0f, 1.0f })
	{}
	affine::affine(affine_rt const& T)
		:rotation(T.rotation), translation(T.translation), scaling(1.0f), scaling_xyz({ 1.0f, 1.0f, 1.0f })
	{}
	affine::affine(affine_rts const& T)
		: rotation(T.rotation), translation(T.translation), scaling(T.scaling), scaling_xyz({ 1.0f, 1.0f, 1.0f })
	{}
	affine::affine(rotation_transform const& rotation_arg, vec3 const& translation_arg, float scaling_arg, vec3 const& scaling_xyz)
		:rotation(rotation_arg), translation(translation_arg), scaling(scaling_arg), scaling_xyz(scaling_xyz)
	{}

	mat4 affine::matrix() const
	{
		mat3 const& R = rotation.matrix();
		float const sx = scaling_xyz.x * scaling;
		float const sy = scaling_xyz.y * scaling;
		float const sz = scaling_xyz.z * scaling;
		return mat4{ 
			get<0,0>(R) * sx, get<0,1>(R) * sy, get<0,2>(R) * sz, translation.x,
			get<1,0>(R) * sx, get<1,1>(R) * sy, get<1,2>(R) * sz, translation.y,
			get<2,0>(R) * sx, get<2,1>(R) * sy, get<2,2>(R) * sz, translation.z,
			0.0f  ,  0.0f ,  0.0f ,    1.0f     
		};
		
	}

	vec3 operator*(affine const& T, vec3 const& p)
	{
		// scaling along the local axes first, then rotation and translation
		return T.rotation * (T.scaling * T.scaling_xyz * p) + T.translation;
	}
	
	vec4 operator*(affine const& T, vec4 const& p)
	{
		vec3 const q = T.rotation * (T.scaling * T.scaling_xyz * p.xyz()) + T.translation * p.w;
		return vec4{ q.x, q.y, q.z, p.w };
	}


	affine operator*(affine_rts const& T1, affine const& T2)
	{
		return affine(T1.rotation * T2.rotation, T1 * T2.translation, T1.scaling * T2.scaling, T2.scaling_xyz);
	}
	affine operator*(affine_rt const& T1, affine const& T2)
	{
		return affine(T1.rotation * T2.rotation, T1 * T2.translation, T2.scaling, T2.scaling_xyz);
	}

	static bool is_uniform(vec3 const& s)
	{
		float const m = std::max(std::abs(s.x), std::max(std::abs(s.y), std::abs(s.z)));
		return std::abs(s.x - s.y) <= 1e-5f * m && std::abs(s.x - s.z) <= 1e-5f * m;
	}
	affine operator*(affine const& T1, affine_rts const& T2)
	{
		assert_cgp(is_uniform(T1.scaling_xyz), "affine * affine_rts needs a uniform T1.scaling_xyz (the result would not be an affine): use T1.matrix()*T2.matrix()");
		float const s = T1.scaling * T1.scaling_xyz.x;
		return affine(T1.rotation * T2.rotation, T1 * T2.translation, s * T2.scaling, vec3{ 1,1,1 });
	}
	affine operator*(affine const& T1, affine_rt const& T2)
	{
		return T1 * affine_rts(T2);
	}


	std::string type_str(affine const&)
	{
		return "affine";
	}
	std::string str(affine const& T)
	{
		return str(T.rotation)+" "+str(T.translation)+" "+str(T.scaling)+" "+str(T.scaling_xyz);
	}
	std::ostream& operator<<(std::ostream& s, affine const& T)
	{
		s << str(T);
		return s;
	}

	affine affine::from_matrix(mat4 const& M)
	{
		// The linear block is rotation * scaling * diag(scaling_xyz): column j has the norm
		//  scaling * scaling_xyz[j]. A uniform scaling goes to `scaling` (scaling_xyz stays {1,1,1}).
		affine a;
		a.translation = M.get_block_translation();

		mat3 L = M.get_block_linear();
		vec3 s;
		for (int j = 0; j < 3; ++j)
			s[j] = std::sqrt(L(0, j) * L(0, j) + L(1, j) * L(1, j) + L(2, j) * L(2, j));
		if (s.x < 1e-5f || s.y < 1e-5f || s.z < 1e-5f)
			return a; // degenerate: keep the identity rotation
		if (det(L) < 0)
			s.x = -s.x; // reflection: carried by scaling_xyz.x
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				L(i, j) /= s[j];
		a.rotation = rotation_transform::from_matrix(L);

		if (is_uniform(s))
			a.scaling = s.x;
		else
			a.scaling_xyz = s;
		return a;
	}

	mat4 operator*(affine const& T1, mat4 const& T2) {
		return T1.matrix()*T2;
	}
	mat4 operator*(mat4 const& T1, affine const& T2)
	{
		return T1*T2.matrix();
	}
	mat4 operator*(affine const& T1, mat3 const& T2)
	{
		return T1.matrix()*mat4(T2);
	}
	mat4 operator*(mat3 const& T1, affine const& T2)
	{
		return mat4(T1)*T2.matrix();
	}

	affine& affine::set_scaling(float value) {scaling = value; return *this;}
	affine& affine::set_scaling_xyz(vec3 const& xyz) {scaling_xyz = xyz; return *this;}
	affine& affine::set_translation(vec3 const& xyz) {translation = xyz; return *this;}
	affine& affine::set_rotation(rotation_transform const& r) {rotation = r; return *this;}

}