
void Assignment15::createBoxMesh(std::vector<Vertex> &vDef, std::vector<uint32_t> &vIdx) {
	// The primitive built here is a box centered in the origin, with proportions respecting the texture.

	// Fill array vPos with the positions of the vertices of the mesh
	vDef.push_back({ {-1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} });// vertex 0 - Position and Normal
	vDef.push_back({ {1.0f, -1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} });// vertex 1 - Position and Normal
	vDef.push_back({ {-1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} });// vertex 2 - Position and Normal
	vDef.push_back({ {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f, 1.0f} });// vertex 3 - Position and Normal

	vDef.push_back({ {1.0f, -1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} });// vertex 1/4 - Position and Normal
	vDef.push_back({ {1.0f, -1.0f, -1.0f}, {1.0f, 0.0f, 0.0f} });// vertex 5/5 - Position and Normal
	vDef.push_back({ {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f, 0.0f} });// vertex 3/6 - Position and Normal
	vDef.push_back({ {1.0f, 1.0f, -1.0f}, {1.0f, 0.0f, 0.0f} });// vertex 7/7 - Position and Normal

	vDef.push_back({ {1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f} });// vertex 5/8 - Position and Normal
	vDef.push_back({ {-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, -1.0f} });// vertex 4/9 - Position and Normal
	vDef.push_back({ {1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f} });// vertex 7/10 - Position and Normal
	vDef.push_back({ {-1.0f, 1.0f, -1.0f}, {0.0f, 0.0f, -1.0f} });// vertex 6/11 - Position and Normal

	vDef.push_back({ {-1.0f, -1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f} });// vertex 4/12 - Position and Normal
	vDef.push_back({ {-1.0f, -1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} });// vertex 0/13 - Position and Normal
	vDef.push_back({ {-1.0f, 1.0f, -1.0f}, {-1.0f, 0.0f, 0.0f} });// vertex 6/14 - Position and Normal
	vDef.push_back({ {-1.0f, 1.0f, 1.0f}, {-1.0f, 0.0f, 0.0f} });// vertex 2/15 - Position and Normal

	vDef.push_back({ {-1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} });// vertex 0/16 - Position and Normal
	vDef.push_back({ {1.0f, -1.0f, 1.0f}, {0.0f, -1.0f, 0.0f} });// vertex 1/17 - Position and Normal
	vDef.push_back({ {-1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f} });// vertex 4/18 - Position and Normal
	vDef.push_back({ {1.0f, -1.0f, -1.0f}, {0.0f, -1.0f, 0.0f} });// vertex 5/19 - Position and Normal

	vDef.push_back({ {-1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} });// vertex 2/20 - Position and Normal
	vDef.push_back({ {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 0.0f} });// vertex 3/21 - Position and Normal
	vDef.push_back({ {-1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f} });// vertex 6/22 - Position and Normal
	vDef.push_back({ {1.0f, 1.0f, -1.0f}, {0.0f, 1.0f, 0.0f} });// vertex 7/23 - Position and Normal

	// Fill the array vIdx with the indices of the vertices of the triangles
	vIdx.push_back(0); vIdx.push_back(1); vIdx.push_back(2);
	vIdx.push_back(1); vIdx.push_back(2); vIdx.push_back(3);
	vIdx.push_back(4); vIdx.push_back(5); vIdx.push_back(6);
	vIdx.push_back(5); vIdx.push_back(6); vIdx.push_back(7);
	vIdx.push_back(8); vIdx.push_back(9); vIdx.push_back(10);
	vIdx.push_back(9); vIdx.push_back(10); vIdx.push_back(11);
	vIdx.push_back(12); vIdx.push_back(13); vIdx.push_back(14);
	vIdx.push_back(13); vIdx.push_back(14); vIdx.push_back(15);
	vIdx.push_back(16); vIdx.push_back(17); vIdx.push_back(18);
	vIdx.push_back(17); vIdx.push_back(18); vIdx.push_back(19);
	vIdx.push_back(20); vIdx.push_back(21); vIdx.push_back(22);
	vIdx.push_back(21); vIdx.push_back(22); vIdx.push_back(23);
}

#define M_PI 3.141595f
void Assignment15::createSphereMesh(std::vector<Vertex> &vDef, std::vector<uint32_t> &vIdx) {
	// The primitive built here is a sphere of radius 1, centered in the origin, on which the Mars texture is applied seamless.
	
	// The procedure fills array vPos with the positions of the vertices and of the normal vectors of the mesh
	vDef.push_back({{0,0,0}, {0,0,1}, {0,0}});	// vertex 0 - Position, Normal and uv
	vDef.push_back({{1,0,0}, {0,0,1}, {1,0}});	// vertex 1 - Position and Normal
	vDef.push_back({{0,1,0}, {0,0,1}, {0,1}});	// vertex 2 - Position and Normal
	vDef.push_back({{1,1,0}, {0,0,1}, {1,1}});// vertex 3 - Position and Normal

	// The procedures also fill the array vIdx with the indices of the vertices of the triangles
	vIdx.push_back(0); vIdx.push_back(1); vIdx.push_back(2);	// First triangle
	vIdx.push_back(1); vIdx.push_back(2); vIdx.push_back(3);	// Second triangle
}

