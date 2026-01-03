#ifndef JBL_GENERATE_LOD_H
#define JBL_GENERATE_LOD_H


#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/array_mesh.hpp>

using namespace godot;

class JBL_GENERATE_LOD : public Control {
    GDCLASS(JBL_GENERATE_LOD, Control)

private:


protected:
    static void _bind_methods();


public:
    JBL_GENERATE_LOD();
    ~JBL_GENERATE_LOD();
    void a_test_fun();
	static bool jbl_generate_lod(
		const Ref<ArrayMesh> &mesh,
        const String &save_dir,
        int max_lods = 8,
        float reduction_ratio = 0.5f);

};

#endif