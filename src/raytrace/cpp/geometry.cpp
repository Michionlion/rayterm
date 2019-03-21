#define TINYOBJLOADER_IMPLEMENTATION

#include "geometry.h"
#include <optix.h>
#include <optix_world.h>
#include "build_variables.h"

int Resources::loadObjFile(std::string objfile) {
    objfile = std::string(ASSET_FOLDER) + std::string("/") + objfile;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;

    std::string errString;
    // triangulate model while loading -- all loaded models will consist of only triangles
    if (!tinyobj::LoadObj(
            &attrib, &shapes, &materials, &errString, objfile.c_str(), nullptr, true)) {
        throw std::runtime_error(errString);
    }

    Mesh* mesh = new Mesh(attrib, shapes, materials);

    return addMesh(mesh);
}

// // Loop over shapes
// for (auto shape : shapes) {
//     // Loop over faces(polygon)
//     size_t index_offset = 0;
//     for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++) {
//         int fv = shape.mesh.num_face_vertices[f];
//
//         // Loop over vertices in the face.
//         for (int v = 0; v < fv; v++) {
//             // access to vertex
//             // tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
//             // tinyobj::real_t vx   = attrib.vertices[3 * idx.vertex_index + 0];
//             // tinyobj::real_t vy   = attrib.vertices[3 * idx.vertex_index + 1];
//             // tinyobj::real_t vz   = attrib.vertices[3 * idx.vertex_index + 2];
//             // tinyobj::real_t nx   = attrib.normals[3 * idx.normal_index + 0];
//             // tinyobj::real_t ny   = attrib.normals[3 * idx.normal_index + 1];
//             // tinyobj::real_t nz   = attrib.normals[3 * idx.normal_index + 2];
//             // tinyobj::real_t tx   = attrib.texcoords[2 * idx.texcoord_index + 0];
//             // tinyobj::real_t ty   = attrib.texcoords[2 * idx.texcoord_index + 1];
//             // Optional: vertex colors
//             // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
//             // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
//             // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
//         }
//         index_offset += fv;
//
//         // per-face material
//         shape.mesh.material_ids[f];
//     }
// }
