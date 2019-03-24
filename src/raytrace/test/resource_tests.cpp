#include <gtest/gtest.h>
#include <optix.h>
#include <optix_world.h>
#include <cstdio>
#include "renderer.h"
#include "resources.h"

TEST(GeometryTest, ObjLoad) {
    auto renderer = new Renderer(80, 52, 8);
    Resources resources(renderer->getContext(), renderer->programs);

    int id = resources.loadObjFile("test/monkey.obj");

    Mesh* mesh = resources.getMesh(id);

    int num_verts = mesh->attrib.vertices.size() / 3;
    EXPECT_EQ(num_verts, 7958) << "Loaded incorrect number of vertices";

    // assumes one shape
    int num_tris = mesh->shapes[0].mesh.num_face_vertices.size();
    EXPECT_EQ(num_tris, 15744) << "Loaded incorrect number of faces";
    delete renderer;
}

TEST(GeometryTest, MatLoad) {
    auto renderer = new Renderer(80, 52, 8);
    Resources resources(renderer->getContext(), renderer->programs);

    int mat_id = resources.loadMatFile("test/diffuse_clay.mat");

    optix::Material mat = resources.getMaterial(mat_id);

    int num_vars = mat->getVariableCount();
    EXPECT_EQ(num_vars, 1) << "Loaded incorrect number of variables";
    delete renderer;
}

TEST(GeometryTest, GeometryInstance) {
    auto renderer = new Renderer(80, 52, 8);
    Resources resources(renderer->getContext(), renderer->programs);

    int mesh_id = resources.loadObjFile("test/monkey.obj");
    int mat_id  = resources.loadMatFile("test/diffuse_clay.mat");

    optix::GeometryInstance instance = resources.createGeometryGroup(mesh_id, mat_id)->getChild(0);

    int num_vars = instance->getVariableCount();
    EXPECT_EQ(num_vars, 4) << "Did not load buffer and material variables";

    int num_mats = instance->getMaterialCount();
    EXPECT_EQ(num_mats, 1) << "Did not load correct number of materials";

    optix::Material mat = instance->getMaterial(0u);
    EXPECT_EQ(mat, resources.getMaterial(mat_id)) << "Loaded Material not equal to resources's";

    optix::GeometryTriangles tris = instance->getGeometryTriangles();
    Mesh* mesh                    = resources.getMesh(mesh_id);
    EXPECT_EQ(tris->getPrimitiveCount(), mesh->shapes[0].mesh.num_face_vertices.size())
        << "Mesh and GeometryTriangles triangle count do not match";
    delete renderer;
}
