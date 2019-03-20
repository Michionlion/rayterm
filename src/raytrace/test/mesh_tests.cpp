#include <gtest/gtest.h>
#include <cstdio>
#include "geometry.h"

TEST(GeometryTest, Load) {
    MeshResources resources;

    int id = resources.loadObjFile("assets/monkey.obj");

    Mesh* mesh = resources.getMesh(id);

    int num_verts = mesh->attrib.vertices.size() / 3;
    EXPECT_EQ(num_verts, 7958) << "Loaded incorrect number of vertices";
}
