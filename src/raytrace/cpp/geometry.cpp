#define TINYOBJLOADER_IMPLEMENTATION

#include "geometry.h"
#include <optix.h>
#include <optix_world.h>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "build_variables.h"
#include "programs.h"

// string split algorithm based on
// https://stackoverflow.com/a/46943631/3997791
std::vector<std::string> split(std::string str, const char* token) {
    std::vector<std::string> result;
    while (str.size() != 0) {
        int index = str.find(token);
        if (index != std::string::npos) {
            result.push_back(str.substr(0, index));
            str = str.substr(index + strlen(token));
        } else {
            result.push_back(str);
            str = "";
        }
    }
    return result;
}

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

void throwMatError(
    const std::string& desc, const std::string& matfile, int linenum, const std::string& line) {
    std::ostringstream error;
    error << desc << " in " << matfile << ":" << linenum << "\n  " << line;
    throw std::runtime_error(error.str());
}

int Resources::loadMatFile(std::string matfile) {
    matfile = std::string(ASSET_FOLDER) + std::string("/") + matfile;

    optix::Material material = context->createMaterial();

    std::ifstream matstream(matfile.c_str());
    std::string line;
    int linenum = 0;
    while (std::getline(matstream, line)) {
        linenum++;
        if (line.find("closest:") == 0) {
            // closestHit program line
            // format: "closest:filename,methodname,raytype"
            line                          = line.substr(8);
            std::vector<std::string> args = split(line, ",");
            if (args.size() != 3) {
                throwMatError("Malformed closest line", matfile, linenum, line);
            }
            material->setClosestHitProgram(static_cast<unsigned int>(std::stoul(args[2])),
                programs->get(args[0].c_str(), args[1].c_str()));
        } else if (line.find("any:") == 0) {
            // anyHit program line
            // format: "any:filename,methodname,raytype"
            line                          = line.substr(4);
            std::vector<std::string> args = split(line, ",");
            if (args.size() != 3) {
                throwMatError("Malformed any line", matfile, linenum, line);
            }
            material->setAnyHitProgram(static_cast<unsigned int>(std::stoul(args[2])),
                programs->get(args[0].c_str(), args[1].c_str()));
        } else if (line.find("var:") == 0) {
            // variable def line
            // format: "var:vartype,varname,intvalue"
            line                          = line.substr(4);
            std::vector<std::string> args = split(line, ",");
            if (args.size() < 3) {
                throwMatError("Malformed var line", matfile, linenum, line);
            }
            // FIXME: no error checking if type is specified and not enough values given
            if (args[0].compare("int")) {
                material[args[1]]->setInt(std::stoi(args[2]));  // NOLINT
            } else if (args[0].compare("int2")) {
                material[args[1]]->setInt(std::stoi(args[2]), std::stoi(args[3]));  // NOLINT
            } else if (args[0].compare("int3")) {
                material[args[1]]->setInt(
                    std::stoi(args[2]), std::stoi(args[3]), std::stoi(args[4]));  // NOLINT
            } else if (args[0].compare("int4")) {
                material[args[1]]->setInt(std::stoi(args[2]), std::stoi(args[3]),
                    std::stoi(args[4]), std::stoi(args[5]));  // NOLINT
            } else if (args[0].compare("float")) {
                material[args[1]]->setFloat(std::stof(args[2]));  // NOLINT
            } else if (args[0].compare("float2")) {
                material[args[1]]->setFloat(std::stof(args[2]), std::stof(args[3]));  // NOLINT
            } else if (args[0].compare("float3")) {
                material[args[1]]->setFloat(
                    std::stof(args[2]), std::stof(args[3]), std::stof(args[4]));  // NOLINT
            } else if (args[0].compare("float4")) {
                material[args[1]]->setFloat(std::stof(args[2]), std::stof(args[3]),
                    std::stof(args[4]), std::stof(args[5]));  // NOLINT
            }
        }
    }

    return addMaterial(material);
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
