#include <gtest/gtest.h>
#include <optix.h>
#include <optix_world.h>
#include <cstdio>
#include "geometry.h"
#include "renderer.h"

TEST(GeometryTest, ObjLoad) {
    auto renderer = new Renderer(80, 52);
    Resources resources(renderer->getContext(), renderer->programs);

    int id = resources.loadObjFile("monkey.obj");

    Mesh* mesh = resources.getMesh(id);

    int num_verts = mesh->attrib.vertices.size() / 3;
    EXPECT_EQ(num_verts, 7958) << "Loaded incorrect number of vertices";
}

TEST(GeometryTest, MatLoad) {
    auto renderer = new Renderer(80, 52);
    Resources resources(renderer->getContext(), renderer->programs);

    int mat_id = resources.loadMatFile("diffuse.mat");

    optix::Material mat = resources.getMaterial(mat_id);

    int num_vars = mat->getVariableCount();
    EXPECT_EQ(num_vars, 2) << "Loaded incorrect number of variables";
}

TEST(GeometryTest, GeometryInstance) {
    auto renderer = new Renderer(80, 52);
    Resources resources(renderer->getContext(), renderer->programs);

    int mesh_id = resources.loadObjFile("monkey.obj");
    int mat_id  = resources.loadMatFile("diffuse.mat");

    optix::GeometryInstance instance = resources.createGeometryInstance(mesh_id, mat_id);

    // EXPECT_EQ()
}
