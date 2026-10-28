#include "test_path.hpp"

#include "cgp/01_base/base.hpp"
#include "cgp/21_scene_project_helper/path/path.hpp"

#include <filesystem>
#include <string>

namespace cgp
{
	// split_executable_path has external linkage but is not declared in path.hpp.
	//  Forward-declare it here so the test can exercise the extraction logic directly.
	void split_executable_path(std::string const& executable_path, std::string& executable_dir_out, std::string& executable_name_out);
}

using namespace cgp;

namespace cgp_test
{
	void test_path()
	{
		// ---------- extension stripping at the FIRST dot, no separator ----------
		// These inputs contain neither '/' nor '\\', so the directory split is a no-op
		//  on every platform and only the extension-stripping branch is exercised.
		{
			std::string dir, name;

			// No extension, no separator: name kept verbatim, empty directory.
			split_executable_path("project", dir, name);
			assert_cgp_no_msg(dir == "");
			assert_cgp_no_msg(name == "project");

			// ".exe" stripped (name length > 4): cut at the first '.'.
			split_executable_path("renderer.exe", dir, name);
			assert_cgp_no_msg(dir == "");
			assert_cgp_no_msg(name == "renderer");

			// Several dots: must cut at the FIRST one (find_first_of), not the last.
			split_executable_path("my.app.exe", dir, name);
			assert_cgp_no_msg(dir == "");
			assert_cgp_no_msg(name == "my");

			// Short name (length <= 4): the dot is NOT stripped.
			split_executable_path("x.y", dir, name);
			assert_cgp_no_msg(dir == "");
			assert_cgp_no_msg(name == "x.y");
		}

#if !defined(_WIN32)
		// ---------- directory split with '/' separator (POSIX) ----------
		// Pins the executable-name substring length: it must be exactly the part after
		//  the last '/', not over- or under-counted.
		{
			std::string dir, name;

			split_executable_path("a/bc", dir, name);
			assert_cgp_no_msg(dir == "a/");
			assert_cgp_no_msg(name == "bc");

			split_executable_path("build/bin/project", dir, name);
			assert_cgp_no_msg(dir == "build/bin/");
			assert_cgp_no_msg(name == "project");

			// Directory split AND extension stripping together.
			split_executable_path("build/renderer.exe", dir, name);
			assert_cgp_no_msg(dir == "build/");
			assert_cgp_no_msg(name == "renderer");
		}

		// ---------- project_path_find: <exe_dir>/../<exe_name>/shaders/ ----------
		// Executable in <root>/bin/, resources in <root>/project/shaders/, current directory
		//  5 levels below <root>/w (out of reach of the cwd-relative candidates "../" x4):
		//  only the candidate executable_dir + "../" + executable_name + "/" matches.
		{
			namespace fs = std::filesystem;
			fs::path const root = fs::temp_directory_path() / "cgp_test_project_path_find";
			fs::remove_all(root);
			fs::create_directories(root / "bin");
			fs::create_directories(root / "project" / "shaders");
			fs::create_directories(root / "w" / "a" / "b" / "c" / "d");
			fs::path const cwd = fs::current_path();
			fs::current_path(root / "w" / "a" / "b" / "c" / "d");

			std::string const found = project_path_find((root / "bin" / "project").string(), "shaders/");
			assert_cgp_no_msg(fs::equivalent(found, root / "project"));

			fs::current_path(cwd);
			fs::remove_all(root);
		}
#endif
	}
}
