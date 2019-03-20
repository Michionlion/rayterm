#ifndef _RAYTERM_GEOMETRY_H_
#define _RAYTERM_GEOMETRY_H_

#include <optix.h>
#include <optix_world.h>
#include <cstdlib>
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

ObjFile* loadObj(const char* objfile);

#endif
