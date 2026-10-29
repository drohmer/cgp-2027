#include "test_geometric_transformation.hpp"

#include "cgp/01_base/base.hpp"
#include "cgp/09_geometric_transformation/geometric_transformation.hpp"

#include <cmath>

#if defined(__linux__) || defined(__EMSCRIPTEN__)
#pragma GCC diagnostic ignored "-Wunused-variable"
#endif

using namespace cgp;

namespace cgp_test
{
	void test_geometric_transformation()
	{
		// frame::ux/uy/uz must return the COLUMNS of the orientation matrix
		//  (the local axes expressed in world coordinates = R * world_basis), not the rows.
		{
			rotation_transform const R = rotation_transform::from_axis_angle(vec3{0,0,1}, Pi/2.0f);
			frame const f(R, vec3{0,0,0});

			// R is a +90 deg rotation around z: x->y, y->-x, z->z
			assert_cgp_no_msg(is_equal(f.ux(), vec3{0,1,0}));
			assert_cgp_no_msg(is_equal(f.uy(), vec3{-1,0,0}));
			assert_cgp_no_msg(is_equal(f.uz(), vec3{0,0,1}));

			// must match R applied to the world basis vectors
			assert_cgp_no_msg(is_equal(f.ux(), R*vec3{1,0,0}));
			assert_cgp_no_msg(is_equal(f.uy(), R*vec3{0,1,0}));
			assert_cgp_no_msg(is_equal(f.uz(), R*vec3{0,0,1}));
		}

		// interpolation_bilinear: a function linear in x and y is interpolated exactly,
		//  including up to the last valid coordinate (dimension-2).
		{
			grid_2D<float> g;
			g.resize(3, 3);
			for (int kx = 0; kx < 3; ++kx)
				for (int ky = 0; ky < 3; ++ky)
					g(kx, ky) = float(kx) + 2.0f*float(ky); // f(x,y) = x + 2y

			assert_cgp_no_msg(is_equal(interpolation_bilinear(g, 1.5f, 0.5f), 1.5f + 2.0f*0.5f));
			assert_cgp_no_msg(is_equal(interpolation_bilinear(g, 0.0f, 0.0f), 0.0f));
			// upper bound: x in [0, dim.x-2] = [0,1], here x close to 1 with x1 = 2 still valid
			assert_cgp_no_msg(is_equal(interpolation_bilinear(g, 1.9f, 1.9f), 1.9f + 2.0f*1.9f));
		}

		// projection matrices: forward * inverse == identity
		{
			mat4 const I = mat4::build_identity();
			{
				mat4 const P    = projection_perspective(1.0f, 1.5f, 1.0f, 10.0f);
				mat4 const Pinv = projection_perspective_inverse(1.0f, 1.5f, 1.0f, 10.0f);
				assert_cgp_no_msg(is_equal(P*Pinv, I));
				assert_cgp_no_msg(is_equal(Pinv*P, I));
			}
			{
				mat4 const P    = projection_orthographic(-2.0f, 3.0f, -1.0f, 4.0f, 1.0f, 10.0f);
				mat4 const Pinv = projection_orthographic_inverse(-2.0f, 3.0f, -1.0f, 4.0f, 1.0f, 10.0f);
				assert_cgp_no_msg(is_equal(P*Pinv, I));
				assert_cgp_no_msg(is_equal(Pinv*P, I));
			}
		}

		// affine_rts inverse: inverse(T) must undo T (translation divided by scaling).
		{
			affine_rts const T(rotation_transform::from_axis_angle(vec3{0,0,1}, 0.7f), vec3{1,2,3}, 2.0f);
			affine_rts const Ti = inverse(T);

			vec3 const p{4,-5,6};
			assert_cgp_no_msg(is_equal(Ti*(T*p), p));
			assert_cgp_no_msg(is_equal(T*(Ti*p), p));
			assert_cgp_no_msg(type_str(Ti) == "affine_rts");
		}

		// affine * vec4: the translation is weighted by w (a direction, w=0, is not translated),
		//  as for affine_rt and affine_rts.
		{
			affine T;
			T.translation = vec3{1,2,3};
			assert_cgp_no_msg(is_equal(T*vec4{1,0,0,0}, vec4{1,0,0,0}));
			assert_cgp_no_msg(is_equal(T*vec4{1,0,0,1}, vec4{2,2,3,1}));
		}

		// affine: scaling_xyz stretches the local axes of the shape, before the rotation
		//  (p -> R * (scaling * scaling_xyz * p) + t), as T*R*S in most 3D tools.
		{
			auto const close = [](vec3 const& a, vec3 const& b) { return norm(a-b) < 1e-3f; };
			affine const T(rotation_axis_angle({0,0,1}, Pi/2), vec3{1,0,0}, 1.0f, vec3{2,1,1});
			// the local x axis is stretched by 2, then turned to y
			assert_cgp_no_msg(close(T*vec3{1,0,0}, vec3{1,2,0}));
			assert_cgp_no_msg(close(T*vec3{0,1,0}, vec3{0,0,0}));
			// matrix() and operator* agree
			vec3 const p{0.3f,-1.2f,2.0f};
			vec4 const q = T.matrix()*vec4(p,1.0f);
			assert_cgp_no_msg(close(q.xyz(), T*p));
			assert_cgp_no_msg(close((T*vec4(p,1.0f)).xyz(), T*p));
		}

		// affine::from_matrix recovers a non-uniform scaling, and keeps scaling_xyz={1,1,1}
		//  for a rigid or uniformly scaled matrix
		{
			auto const close = [](vec3 const& a, vec3 const& b) { return norm(a-b) < 1e-3f; };
			affine const T(rotation_axis_angle(normalize(vec3{1,2,3}), 0.7f), vec3{1,2,3}, 1.5f, vec3{2,0.5f,1});
			affine const A = affine::from_matrix(T.matrix());
			vec3 const p{0.3f,-1.2f,2.0f};
			assert_cgp_no_msg(close(A*p, T*p));

			affine_rts const U(rotation_axis_angle({0,1,0}, 0.4f), vec3{0,1,0}, 2.0f);
			affine const B = affine::from_matrix(U.matrix());
			assert_cgp_no_msg(std::abs(B.scaling-2.0f) < 1e-4f);
			assert_cgp_no_msg(close(B.scaling_xyz, vec3{1,1,1}));
			assert_cgp_no_msg(close(B*p, U*p));
		}

		// a rigid or similarity transform composed with an affine (parent * child) is exact
		{
			auto const close = [](vec3 const& a, vec3 const& b) { return norm(a-b) < 1e-3f; };
			affine const child(rotation_axis_angle({1,0,0}, 0.5f), vec3{0,1,0}, 1.0f, vec3{3,1,0.5f});
			affine_rts const parent(rotation_axis_angle({0,0,1}, 1.1f), vec3{2,0,0}, 0.5f);
			affine_rt const rigid(rotation_axis_angle({0,1,0}, -0.3f), vec3{0,0,1});
			vec3 const p{0.3f,-1.2f,2.0f};
			assert_cgp_no_msg(close((parent*child)*p, parent*(child*p)));
			assert_cgp_no_msg(close((rigid*child)*p, rigid*(child*p)));
		}

		// rotation_transform normalizes a quaternion close to unit norm
		{
			rotation_transform const R(quaternion{ 0,0,0,1.02f });
			assert_cgp_no_msg(std::abs(norm(R.data) - 1.0f) < 1e-6f);
		}
	}
}
