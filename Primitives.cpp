#include "Primitives.h"
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
		auto u = y*std::cos(i*2*pi/N)*radius + z*std::sin(i*2*pi/N)*radius;
		points.push_back(u);
	}

	for(int i = 0; i < N; i++)
	{
		auto v = y*std::cos(i*2*pi/N)*radius + z*std::sin(i*2*pi/N)*radius + x;
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
		indices.push_back(i);
		indices.push_back((i+1)%N);
		indices.push_back(N+i);

		indices.push_back(N+i);
		indices.push_back((i+1)%N);
		indices.push_back(N+(i+1)%N);
	}

	for(int i = 0; i < N; i++)
	{
		indices.push_back(2*N);
		indices.push_back(2*N+i);
		indices.push_back(2*N+(i+1)%N);

		indices.push_back(3*N);
		indices.push_back(3*N+(i+1)%N);
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

std::pair<std::vector<Vertex3>, std::vector<int>> buildCone(real length, real radius, int N)
{
	Vector3 x = { 1, 0, 0 }, y = { 0, 1, 0 }, z = { 0, 0, 1 };

	std::vector<Vector3> points;
	std::vector<Vertex3> vertices;
	std::vector<int> indices;

	for(int i = 0; i < N; i++)
	{
		auto u = y*std::cos(i*2*pi/N)*radius + z*std::sin(i*2*pi/N)*radius;
		points.push_back(u);
	}
	points.push_back( { length, 0, 0 } );

	for(int i = 0; i < N; i++)
	{
		auto u = points[i];

		Vector3 a = points[i] - points[N];
		Vector3 v = points[i];
		auto n_u = (a%v)%a;

		Vertex3 U(u, n_u.normalized(), { 0, 0 });
		vertices.push_back(U);
	}

	for(int i = 0; i < N; i++)
	{
		Vector3 a = points[i] - points[N];
		Vector3 v = points[i];
		auto n_u = (a%v)%a;

		Vertex3 U( { length, 0, 0 }, n_u.normalized(), { 0, 0 });
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
		indices.push_back(i);
		indices.push_back((i+1)%N);
		indices.push_back(N+i);
	}

	for(int i = 0; i < N; i++)
	{
		indices.push_back(2*N);
		indices.push_back(2*N+i);
		indices.push_back(2*N+(i+1)%N);
	}

	return { vertices, indices };
}

Model3d* createConeModel(real length, real radius, int N)
{
    auto material = new LambertianMaterial({ 0, 0.8, 0.1 });

	auto [vertices, triangles] = buildCone(length, radius, N);

	auto model = new Model3d();
    Mesh3d* mesh = new Mesh3d(vertices, triangles, material);
    model->addMesh(*mesh);

	return model;
}

void addSphereVertices(int a, int b, int c, real radius, std::vector<Vector3>& points, std::vector<int>& indices, int N)
{
	if(--N < 1)
	{
		indices.push_back(a);
		indices.push_back(b);
		indices.push_back(c);
		return;
	}

	int d = points.size(), e = d+1, f = e+1;
	points.push_back(((points[a]+points[b])/2).normalized()*radius);
	points.push_back(((points[b]+points[c])/2).normalized()*radius);
	points.push_back(((points[c]+points[a])/2).normalized()*radius);
	addSphereVertices(e, b, d, radius, points, indices, N);
	addSphereVertices(f, d, a, radius, points, indices, N);
	addSphereVertices(e, f, c, radius, points, indices, N);
	addSphereVertices(e, d, f, radius, points, indices, N);
}

std::pair<std::vector<Vertex3>, std::vector<int>> buildSphere(real radius, int N)
{
	auto u = 2*sqrt(2)/3, v = sqrt(2)/sqrt(3), w = sqrt(2)/3, t = 1/3.0;
	auto a = Vector3(u, 0, t);
	auto b = Vector3(-w, v, t);
	auto c = Vector3(-w, -v, t);
	auto d = Vector3(0, 0, -1);

	std::vector<Vector3> points = { a*radius, b*radius, c*radius, d*radius };
	std::vector<int> indices;

	addSphereVertices(0, 1, 2, radius, points, indices, N);
	addSphereVertices(0, 2, 3, radius, points, indices, N);
	addSphereVertices(3, 2, 1, radius, points, indices, N);
	addSphereVertices(0, 3, 1, radius, points, indices, N);

	std::vector<Vertex3> vertices;

	for(auto& p : points)
		vertices.push_back( { p, p.normalized(), { 0, 0 } });

	return { vertices, indices };
}

Model3d* createSphereModel(real radius, int N)
{
    auto material = new LambertianMaterial({ 0, 0.8, 0.1 });

	auto [vertices, triangles] = buildSphere(radius, N);

	auto model = new Model3d();
    Mesh3d* mesh = new Mesh3d(vertices, triangles, material);
    model->addMesh(*mesh);

	return model;
}

std::pair<std::vector<Vertex3>, std::vector<int>> buildBox(real length, real width, real height)
{
	real w = length, d = width, h = height;
    std::vector<Vector3> c = {
        { -w/2, -d/2, -h/2 }, // bottom-front-left
        {  w/2, -d/2, -h/2 }, // bottom-front-right
        {  w/2, -d/2,  h/2 }, // top-front-right
        { -w/2, -d/2,  h/2 }, // top-front-left
        { -w/2,  d/2, -h/2 }, // bottom-back-left
        {  w/2,  d/2, -h/2 }, // bottom-back-right
        {  w/2,  d/2,  h/2 }, // top-back-right
        { -w/2,  d/2,  h/2 }  // top-back-left
    };

    std::vector<std::vector<int>> cornerIndices = {
        { 0, 1, 2, 3 },
        { 1, 5, 6, 2 },
        { 4, 7, 6, 5 },
        { 0, 3, 7, 4 },
        { 2, 6, 7, 3 },
        { 0, 4, 5, 1 }
    };

    std::vector<int> triangles;
    std::vector<Vertex3> vertices;

    for(auto ci : cornerIndices)
    {
        int t1[3] = { 0, 1, 2 }, t2[3] = { 0, 2, 3 };

        int j = vertices.size();
        for(int i = 0; i < 4; i++)
            vertices.push_back({ c[ci[i]], calcNormal(c[ci[0]], c[ci[1]], c[ci[2]]), { 0, 0 } });
        triangles.insert(triangles.end(), { j, j+1, j+2 });
        triangles.insert(triangles.end(), { j, j+2, j+3 });
    }

	return { vertices, triangles };
}

Model3d* createBoxModel(real length, real width, real height)
{
    auto material = new LambertianMaterial({ 0, 0.8, 0.1 });

	auto [vertices, triangles] = buildBox(length, width, height);

	auto model = new Model3d();
    Mesh3d* mesh = new Mesh3d(vertices, triangles, material);
    model->addMesh(*mesh);

	return model;
}