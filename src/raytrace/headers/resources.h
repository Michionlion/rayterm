#ifndef _RAYTERM_RESOURCES_
#define _RAYTERM_RESOURCES_

#include <optix.h>
#include <optix_world.h>
#include <cstdlib>
#include <vector>
#include "programs.h"
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
    Programs* programs;

    Mesh** meshes   = nullptr;
    int meshes_size = 0;

    std::vector<optix::Material> materials;

   public:
    explicit Resources(const optix::Context& context, Programs* programs)
        : context(context), programs(programs){};
    ~Resources() {
        try {
            for (int i = 0; i < meshes_size; i++) {
                delete meshes[i];
            }

            free(meshes);

            for (auto material : materials) {
                material->removeReference();
            }
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

    optix::Material getMaterial(int id) { return materials[id]; }
    int addMaterial(const optix::Material& mat) {
        materials.push_back(mat);
        return materials.size() - 1;
    }

    // load obj file and return mesh id
    // this method allocates a new Mesh
    // and adds it to the meshes array
    int loadObjFile(std::string objfile);
    int loadObjFile(const char* objfile) { return loadObjFile(std::string(objfile)); }

    // load mat file and return mat id
    // this method creates a material
    // and adds it to the materials vector
    int loadMatFile(std::string matfile);
    int loadMatFile(const char* matfile) { return loadMatFile(std::string(matfile)); }

    optix::GeometryGroup createGeometryGroup(
        int mesh_id, int mat_id, const optix::Matrix4x4& transform = optix::Matrix4x4::identity());
};
#endif
