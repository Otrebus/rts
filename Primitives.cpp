#include "Primitives.h"
#include <cmath>
#include <vector>
#include "Vertex3d.h"
#include "Vector3.h"
#include "Math.h"
#include "Model3d.h"
#include "LambertianMaterial.h"

std::pair<std::vector<Vertex3>, std::vector<int>> buildCylinder(real length, real radius, int N)
{
	Vector3 x = { 1, 0, 0 }, y = { 0, 1, 0 }, z = { 0, 0, 1 };

	std::vector<Vector3> points;
	std::vector<Vertex3> vertices;
	std::vector<int> indices;

	for(int i = 0; i < N; i++)
	{
		auto u = y*std::cos(i*2*pi/N)*radius + z*std::sin(i*2*pi/N)*radius - x*length/2;
		points.push_back(u);
	}

	for(int i = 0; i < N; i++)
	{
		auto v = y*std::cos(i*2*pi/N)*radius + z*std::sin(i*2*pi/N)*radius + y*length/2;
		points.push_back(v);
	}

	for(int i = 0; i < N; i++)
	{
		auto u = points[i];
		Vector3 n_u = { 0, points[i].y, points[i].z };
		Vertex3 U(u, n_u, { 0, 0 });
		vertices.push_back(U);
	}

	for(int i = 0; i < N; i++)
	{
		auto u = points[N+i];
		Vector3 n_u = { 0, points[N+i].y, points[N+i].z };
		Vertex3 U(u, n_u, { 0, 0 });
		vertices.push_back(U);
	}

	for(int i = 0; i < N; i++)
	{
		auto u = points[i];
		Vector3 n_u = { -1, 0, 0 };
		Vertex3 U(u, n_u, { 0, 0 });
		vertices.push_back(U);
	}

	for(int i = 0; i < N; i++)
	{
		auto u = points[N+i];
		Vector3 n_u = { 1, 0, 0 };
		Vertex3 U(u, n_u, { 0, 0 });
		vertices.push_back(U);
	}

	for(int i = 0; i < N; i++)
	{
		indices.push_back(i + 0);
		indices.push_back(N+i + 0);
		indices.push_back(i + 1);

		indices.push_back(N+i + 0);
		indices.push_back(N+i + 1);
		indices.push_back(i + 1);
	}

	for(int i = 0; i < N; i++)
	{
		indices.push_back(2*N);
		indices.push_back(2*N+i);
		indices.push_back(2*N+i+1);

		indices.push_back(3*N);
		indices.push_back(2*N+i+1);
		indices.push_back(3*N+i);
	}

	return { vertices, indices };
}

Model3d* createCylinderModel(real length, real radius, int N)
{
    auto material = new LambertianMaterial({ 0, 0.8, 0.1 });

	auto [vertices, triangles] = buildCylinder(length, radius, N);

	auto model = new Model3d();
    Mesh3d* mesh = new Mesh3d(vertices, triangles, material);
    model->addMesh(*mesh);

	return model;
}
