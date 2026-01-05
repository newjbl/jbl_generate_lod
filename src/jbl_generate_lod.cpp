#include "jbl_generate_lod.h"

#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

#include <godot_cpp/classes/resource_saver.hpp>
#include <godot_cpp/classes/dir_access.hpp>

#include "meshoptimizer/meshoptimizer.h" // Godot 同款 API
#include <vector>
#include <cmath>


using namespace godot;


void JBL_GENERATE_LOD::_bind_methods() {
	ClassDB::bind_method(D_METHOD("a_test_fun"), &JBL_GENERATE_LOD::a_test_fun);
	// 绑定LOD生成函数，
	
	ClassDB::bind_static_method(
        "JBL_GENERATE_LOD",
        D_METHOD("jbl_generate_lod", "mesh", "save_dir", "save_format", "max_lods", "reduction_ratio", "error_f"),
        &JBL_GENERATE_LOD::jbl_generate_lod
    );
	
}

JBL_GENERATE_LOD::JBL_GENERATE_LOD() {
	
}

JBL_GENERATE_LOD::~JBL_GENERATE_LOD() {
	// Add your cleanup here.
}

void JBL_GENERATE_LOD::a_test_fun() {
	UtilityFunctions::print("i am a test for jbl!");
}


// =============================================================
// 核心：Godot原生全属性网格简化函数【无阉割完整版】
// 入参：源网格ArrayMesh + 简化比例(0,1]
// 出参：带法线/UV/顶点坐标的简化后网格，无任何属性丢失
// =============================================================

bool JBL_GENERATE_LOD::jbl_generate_lod(
    const Ref<ArrayMesh> &mesh,
    const String &save_dir,
	const String &save_format,
    int max_lods,
    float reduction_ratio,
	float error_f
	
) {
	UtilityFunctions::print("jbl_generate_lod:");
	UtilityFunctions::print(error_f);
	UtilityFunctions::print(save_format);
    ERR_FAIL_COND_V(mesh.is_null(), false);
    ERR_FAIL_COND_V(max_lods <= 0, false);

    if (!DirAccess::dir_exists_absolute(save_dir)) {
        DirAccess::make_dir_recursive_absolute(save_dir);
    }

    const int surface_count = mesh->get_surface_count();
    ERR_FAIL_COND_V(surface_count == 0, false);

    for (int lod = 0; lod < max_lods; lod++) {
        Ref<ArrayMesh> lod_mesh = memnew(ArrayMesh);
        const float ratio = std::pow(reduction_ratio, lod);

        for (int s = 0; s < surface_count; s++) {
            Array arrays = mesh->surface_get_arrays(s);

            PackedVector3Array vertices = arrays[Mesh::ARRAY_VERTEX];
            PackedInt32Array indices  = arrays[Mesh::ARRAY_INDEX];

            if (vertices.is_empty() || indices.is_empty()) {
                continue;
            }

            const size_t vertex_count = (size_t)vertices.size();
            const size_t index_count  = (size_t)indices.size();

            const size_t target_index_count =
                (lod == 0) ? index_count : size_t(index_count * ratio);

            // LOD0：完全复制
            if (lod == 0 || target_index_count >= index_count) {
                lod_mesh->add_surface_from_arrays(
                    mesh->surface_get_primitive_type(s),
                    arrays
                );
                continue;
            }

            // -------- 核心：只用 C API --------
            std::vector<unsigned int> simplified_indices(index_count);

            const unsigned int *src_indices =
                reinterpret_cast<const unsigned int *>(indices.ptr());

            const float *src_positions =
                reinterpret_cast<const float *>(vertices.ptr());

            size_t new_index_count = meshopt_simplify(
                simplified_indices.data(),   // unsigned int*
                src_indices,                 // const unsigned int*
                index_count,
                src_positions,               // const float*
                vertex_count,
                sizeof(Vector3),
                target_index_count,
                error_f                        // error tolerance
            );
            // ----------------------------------

            if (new_index_count < 3) {
                continue;
            }

            PackedInt32Array new_indices;
            new_indices.resize((int)new_index_count);

            for (size_t i = 0; i < new_index_count; i++) {
                new_indices[(int)i] = (int)simplified_indices[i];
            }

            Array new_arrays = arrays.duplicate();
            new_arrays[Mesh::ARRAY_INDEX] = new_indices;

            lod_mesh->add_surface_from_arrays(
                mesh->surface_get_primitive_type(s),
                new_arrays
            );
        }

        if (lod_mesh->get_surface_count() == 0) {
            continue;
        }

        const String path =
            save_dir.path_join("lod_" + itos(lod) + "." + save_format);

        Error err = ResourceSaver::get_singleton()->save(lod_mesh, path);
        if (err != OK) {
            UtilityFunctions::printerr("Failed to save LOD: ", path);
        }
    }
	
    return true;
}
