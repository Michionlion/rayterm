#ifndef _RAYTERM_GEOMETRY_H_
#define _RAYTERM_GEOMETRY_H_

#include <optix.h>
#include <optix_world.h>
#include <cstdlib>
#include <vector>
#include "tiny_obj_loader.h"

// Code here is roughly inspired from OptiXMesh in the OptiX samples
// https://github.com/nvpro-samples/optix_advanced_samples

class Mesh {
   public:
    Mesh(tinyobj::attrib_t attrib, std::vector<tinyobj::shape_t> shapes,
        std::vector<tinyobj::material_t> materials) {
        this->attrib    = std::move(attrib);
        this->shapes    = std::move(shapes);
        this->materials = std::move(materials);
    };

    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
};

class Resources {
    optix::Context context;

    Mesh** meshes   = nullptr;
    int meshes_size = 0;

   public:
    explicit Resources(const optix::Context& context) : context(context){};
    ~Resources() {
        try {
            for (int i = 0; i < meshes_size; i++) {
                delete meshes[i];
            }

            free(meshes);
        } catch (const optix::Exception& ex) {
            printf("~Resources Error: %d (%s)\n", ex.getErrorCode(), ex.getErrorString().c_str());
        }
    };

    Mesh* getMesh(int id) { return meshes[id]; }

    // pass in a mesh and MeshResources will handle the rest
    // returns the mesh id
    int addMesh(Mesh* mesh) {
        meshes = static_cast<Mesh**>(realloc(meshes, (meshes_size + 1) * sizeof(Mesh*)));
        meshes[meshes_size] = mesh;
        return meshes_size++;
    }

    // load obj file and return mesh id
    // this method allocates a new Mesh
    // and adds it to the meshes array
    int loadObjFile(const char* objfile);

    optix::GeometryInstance createGeometryInstance(const int mesh_id);
};
#endif
