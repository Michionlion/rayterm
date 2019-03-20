#ifndef _RAYTERM_GEOMETRY_H_
#define _RAYTERM_GEOMETRY_H_

#include <optix.h>
#include <optix_world.h>
#include "tiny_obj_loader.h"

class ObjFile {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    ObjFile(tinyobj::attrib_t attrib, std::vector<tinyobj::shape_t> shapes,
        std::vector<tinyobj::material_t> materials) {
        this->attrib    = std::move(attrib);
        this->shapes    = std::move(shapes);
        this->materials = std::move(materials);
    };

   public:
    optix::Geometry getOptixGeometry();
};

ObjFile* loadObj(const char* objfile);

#endif
