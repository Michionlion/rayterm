#define TINYOBJLOADER_IMPLEMENTATION

#include "resources.h"
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

    bool requirements[2] = {false, false};

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
            requirements[0] = true;
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
            requirements[1] = true;
        } else if (line.find("attrib:") == 0) {
            // attrib program line
            // format: "attrib:filename,methodname"
            line                          = line.substr(7);
            std::vector<std::string> args = split(line, ",");
            if (args.size() != 2) {
                throwMatError("Malformed attrib line", matfile, linenum, line);
            }
            material["attribProgram"]->setProgramId(
                programs->get(args[0].c_str(), args[1].c_str()));
            requirements[2] = true;
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

    // verify requirements
    if (!requirements[0]) {
        throw std::runtime_error("No closest line in " + matfile);
    } else if (!requirements[1]) {
        throw std::runtime_error("No any line in " + matfile);
    }

    return addMaterial(material);
}

optix::GeometryGroup Resources::createGeometryGroup(int mesh_id, int mat_id,
    const optix::Matrix4x4& transform /* = optix::Matrix4x4::identity() */) {
    Mesh* mesh               = getMesh(mesh_id);
    optix::Material material = getMaterial(mat_id);

    int num_verts = mesh->attrib.vertices.size();
    int num_norms = mesh->attrib.normals.size();
    // FIXME: assumes one shape
    int num_tris = mesh->shapes[0].mesh.num_face_vertices.size();

    optix::Buffer vbuf = context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, num_verts);
    optix::Buffer nbuf = context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, num_norms);
    optix::Buffer vidx = context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_INT3, num_tris);
    optix::Buffer nidx = context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_INT3, num_tris);

    // FIXME: use different indexing scheme for nidx, need to actually get correct normal index that
    // varies with vertex, not face normal

    auto vbuf_data = static_cast<optix::float3*>(vbuf->map());
    auto nbuf_data = static_cast<optix::float3*>(nbuf->map());
    auto vidx_data = static_cast<optix::uint3*>(vidx->map());
    auto nidx_data = static_cast<optix::uint3*>(nidx->map());

    // clang-format off
    for (int i = 0; i < num_verts; i++) {
        optix::float4 vert = optix::make_float4(
            mesh->attrib.vertices[3 * i + 0],
            mesh->attrib.vertices[3 * i + 1],
            mesh->attrib.vertices[3 * i + 2],
            1.0f
        );
        vbuf_data[i] = optix::make_float3(transform * vert);
    }

    optix::Matrix4x4 norm_transform = transform.inverse().transpose();

    for (int i = 0; i < num_norms; i++) {
        optix::float4 vert = optix::make_float4(
            mesh->attrib.normals[3 * i + 0],
            mesh->attrib.normals[3 * i + 1],
            mesh->attrib.normals[3 * i + 2],
            0.0f
        );
        nbuf_data[i] = optix::make_float3(norm_transform * vert);
    }

    for (int i = 0; i < num_tris; i++) {
        vidx_data[i] = optix::make_uint3(
            mesh->shapes[0].mesh.indices[3 * i + 0].vertex_index,
            mesh->shapes[0].mesh.indices[3 * i + 1].vertex_index,
            mesh->shapes[0].mesh.indices[3 * i + 2].vertex_index
        );
    }
    for (int i = 0; i < num_tris; i++) {
        nidx_data[i] = optix::make_uint3(
            mesh->shapes[0].mesh.indices[3 * i + 0].normal_index,
            mesh->shapes[0].mesh.indices[3 * i + 1].normal_index,
            mesh->shapes[0].mesh.indices[3 * i + 2].normal_index
        );
    }
    // clang-format on

    vbuf->unmap();
    nbuf->unmap();
    vidx->unmap();
    nidx->unmap();

    // create GeometryTriangles
    optix::GeometryTriangles triangles = context->createGeometryTriangles();

    triangles->setPrimitiveCount(num_tris);
    triangles->setVertices(num_verts, vbuf, RT_FORMAT_FLOAT3);
    triangles->setTriangleIndices(vidx, RT_FORMAT_UNSIGNED_INT3);
    if (material["attribProgram"]->getType() == RT_OBJECTTYPE_PROGRAM) {
        printf("before\n");
        triangles->setAttributeProgram(material["attribProgram"]->getProgram());
        printf("after\n");
    }

    // create GeometryInstance
    optix::GeometryInstance instance = context->createGeometryInstance(triangles, material);
    // instance->setGeometryTriangles(triangles);
    // instance->setMaterialCount(1);
    // instance->setMaterial(0u, material);

    instance["vbuf"]->set(vbuf);
    instance["nbuf"]->set(nbuf);
    instance["vidx"]->set(vidx);
    instance["nidx"]->set(nidx);

    optix::GeometryGroup group = context->createGeometryGroup();
    group->addChild(instance);
    group->setAcceleration(context->createAcceleration("Trbvh"));

    return group;
}
