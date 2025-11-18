/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/

#include <cmath>
#include <algorithm>

#include "OvRendering/Data/Frustum.h"

// We create an enum of the sides so we don't have to call each side 0 or 1.
// This way it makes it more understandable and readable when dealing with frustum sides.
enum FrustumSide
{
	RIGHT = 0,		// The RIGHT side of the frustum
	LEFT = 1,		// The LEFT	 side of the frustum
	BOTTOM = 2,		// The BOTTOM side of the frustum
	TOP = 3,		// The TOP side of the frustum
	BACK = 4,		// The BACK	side of the frustum
	FRONT = 5		// The FRONT side of the frustum
};

// Like above, instead of saying a number for the ABC and D of the plane, we
// want to be more descriptive.
enum PlaneData
{
	A = 0,				// The X value of the plane's normal
	B = 1,				// The Y value of the plane's normal
	C = 2,				// The Z value of the plane's normal
	D = 3				// The distance the plane is from the origin
};

///////////////////////////////// NORMALIZE PLANE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This normalizes a plane (A side) from a given frustum.
/////
///////////////////////////////// NORMALIZE PLANE \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void NormalizePlane(float frustum[6][4], int side)
{
	// Here we calculate the magnitude of the normal to the plane (point A B C)
	// Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
	// To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
	float magnitude = (float)sqrt(frustum[side][A] * frustum[side][A] +
		frustum[side][B] * frustum[side][B] +
		frustum[side][C] * frustum[side][C]);

	// Then we divide the plane's values by it's magnitude.
	// This makes it easier to work with.
	frustum[side][A] /= magnitude;
	frustum[side][B] /= magnitude;
	frustum[side][C] /= magnitude;
	frustum[side][D] /= magnitude;
}


///////////////////////////////// CALCULATE FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This extracts our frustum from the projection and modelview matrix.
/////
///////////////////////////////// CALCULATE FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

void OvRendering::Data::Frustum::CalculateFrustum(const OvMaths::FMatrix4& p_viewProjection)
{
	auto columnMajorViewProjection = OvMaths::FMatrix4::Transpose(p_viewProjection);
	float const* clip = columnMajorViewProjection.data;

	// Now we actually want to get the sides of the frustum.  To do this we take
	// the clipping planes we received above and extract the sides from them.

	// This will extract the RIGHT side of the frustum
	m_frustum[RIGHT][A] = clip[3] - clip[0];
	m_frustum[RIGHT][B] = clip[7] - clip[4];
	m_frustum[RIGHT][C] = clip[11] - clip[8];
	m_frustum[RIGHT][D] = clip[15] - clip[12];

	// Now that we have a normal (A,B,C) and a distance (D) to the plane,
	// we want to normalize that normal and distance.

	// Normalize the RIGHT side
	NormalizePlane(m_frustum, RIGHT);

	// This will extract the LEFT side of the frustum
	m_frustum[LEFT][A] = clip[3] + clip[0];
	m_frustum[LEFT][B] = clip[7] + clip[4];
	m_frustum[LEFT][C] = clip[11] + clip[8];
	m_frustum[LEFT][D] = clip[15] + clip[12];

	// Normalize the LEFT side
	NormalizePlane(m_frustum, LEFT);

	// This will extract the BOTTOM side of the frustum
	m_frustum[BOTTOM][A] = clip[3] + clip[1];
	m_frustum[BOTTOM][B] = clip[7] + clip[5];
	m_frustum[BOTTOM][C] = clip[11] + clip[9];
	m_frustum[BOTTOM][D] = clip[15] + clip[13];

	// Normalize the BOTTOM side
	NormalizePlane(m_frustum, BOTTOM);

	// This will extract the TOP side of the frustum
	m_frustum[TOP][A] = clip[3] - clip[1];
	m_frustum[TOP][B] = clip[7] - clip[5];
	m_frustum[TOP][C] = clip[11] - clip[9];
	m_frustum[TOP][D] = clip[15] - clip[13];

	// Normalize the TOP side
	NormalizePlane(m_frustum, TOP);

	// This will extract the BACK side of the frustum
	m_frustum[BACK][A] = clip[3] - clip[2];
	m_frustum[BACK][B] = clip[7] - clip[6];
	m_frustum[BACK][C] = clip[11] - clip[10];
	m_frustum[BACK][D] = clip[15] - clip[14];

	// Normalize the BACK side
	NormalizePlane(m_frustum, BACK);

	// This will extract the FRONT side of the frustum
	m_frustum[FRONT][A] = clip[3] + clip[2];
	m_frustum[FRONT][B] = clip[7] + clip[6];
	m_frustum[FRONT][C] = clip[11] + clip[10];
	m_frustum[FRONT][D] = clip[15] + clip[14];

	// Normalize the FRONT side
	NormalizePlane(m_frustum, FRONT);
}

// The code below will allow us to make checks within the frustum.  For example,
// if we want to see if a point, a sphere, or a cube lies inside of the frustum.
// Because all of our planes point INWARDS (The normals are all pointing inside the frustum)
// we then can assume that if a point is in FRONT of all of the planes, it's inside.

///////////////////////////////// POINT IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This determines if a point is inside of the frustum
/////
///////////////////////////////// POINT IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool OvRendering::Data::Frustum::PointInFrustum(float x, float y, float z) const
{
	// If you remember the plane equation (A*x + B*y + C*z + D = 0), then the rest
	// of this code should be quite obvious and easy to figure out yourself.
	// In case don't know the plane equation, it might be a good idea to look
	// at our Plane Collision tutorial at www.GameTutorials.com in OpenGL Tutorials.
	// I will briefly go over it here.  (A,B,C) is the (X,Y,Z) of the normal to the plane.
	// They are the same thing... but just called ABC because you don't want to say:
	// (x*x + y*y + z*z + d = 0).  That would be wrong, so they substitute them.
	// the (x, y, z) in the equation is the point that you are testing.  The D is
	// The distance the plane is from the origin.  The equation ends with "= 0" because
	// that is true when the point (x, y, z) is ON the plane.  When the point is NOT on
	// the plane, it is either a negative number (the point is behind the plane) or a
	// positive number (the point is in front of the plane).  We want to check if the point
	// is in front of the plane, so all we have to do is go through each point and make
	// sure the plane equation goes out to a positive number on each side of the frustum.
	// The result (be it positive or negative) is the distance the point is front the plane.

	// Go through all the sides of the frustum
	for (int i = 0; i < 6; i++)
	{
		// Calculate the plane equation and check if the point is behind a side of the frustum
		if (m_frustum[i][A] * x + m_frustum[i][B] * y + m_frustum[i][C] * z + m_frustum[i][D] <= 0)
		{
			// The point was behind a side, so it ISN'T in the frustum
			return false;
		}
	}

	// The point was inside of the frustum (In front of ALL the sides of the frustum)
	return true;
}


///////////////////////////////// SPHERE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This determines if a sphere is inside of our frustum by it's center and radius.
/////
///////////////////////////////// SPHERE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool OvRendering::Data::Frustum::SphereInFrustum(float x, float y, float z, float radius) const
{
	// Now this function is almost identical to the PointInFrustum(), except we
	// now have to deal with a radius around the point.  The point is the center of
	// the radius.  So, the point might be outside of the frustum, but it doesn't
	// mean that the rest of the sphere is.  It could be half and half.  So instead of
	// checking if it's less than 0, we need to add on the radius to that.  Say the
	// equation produced -2, which means the center of the sphere is the distance of
	// 2 behind the plane.  Well, what if the radius was 5?  The sphere is still inside,
	// so we would say, if(-2 < -5) then we are outside.  In that case it's false,
	// so we are inside of the frustum, but a distance of 3.  This is reflected below.

	// Go through all the sides of the frustum
	for (int i = 0; i < 6; i++)
	{
		// If the center of the sphere is farther away from the plane than the radius
		if (m_frustum[i][A] * x + m_frustum[i][B] * y + m_frustum[i][C] * z + m_frustum[i][D] <= -radius)
		{
			// The distance was greater than the radius so the sphere is outside of the frustum
			return false;
		}
	}

	// The sphere was inside of the frustum!
	return true;
}


///////////////////////////////// CUBE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*
/////
/////	This determines if a cube is in or around our frustum by it's center and 1/2 it's length
/////
///////////////////////////////// CUBE IN FRUSTUM \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\*

bool OvRendering::Data::Frustum::CubeInFrustum(float x, float y, float z, float size) const
{
	// This test is a bit more work, but not too much more complicated.
	// Basically, what is going on is, that we are given the center of the cube,
	// and half the length.  Think of it like a radius.  Then we checking each point
	// in the cube and seeing if it is inside the frustum.  If a point is found in front
	// of a side, then we skip to the next side.  If we get to a plane that does NOT have
	// a point in front of it, then it will return false.

	// *Note* - This will sometimes say that a cube is inside the frustum when it isn't.
	// This happens when all the corners of the bounding box are not behind any one plane.
	// This is rare and shouldn't effect the overall rendering speed.

	for (int i = 0; i < 6; i++)
	{
		if (m_frustum[i][A] * (x - size) + m_frustum[i][B] * (y - size) + m_frustum[i][C] * (z - size) + m_frustum[i][D] > 0)
			continue;
		if (m_frustum[i][A] * (x + size) + m_frustum[i][B] * (y - size) + m_frustum[i][C] * (z - size) + m_frustum[i][D] > 0)
			continue;
		if (m_frustum[i][A] * (x - size) + m_frustum[i][B] * (y + size) + m_frustum[i][C] * (z - size) + m_frustum[i][D] > 0)
			continue;
		if (m_frustum[i][A] * (x + size) + m_frustum[i][B] * (y + size) + m_frustum[i][C] * (z - size) + m_frustum[i][D] > 0)
			continue;
		if (m_frustum[i][A] * (x - size) + m_frustum[i][B] * (y - size) + m_frustum[i][C] * (z + size) + m_frustum[i][D] > 0)
			continue;
		if (m_frustum[i][A] * (x + size) + m_frustum[i][B] * (y - size) + m_frustum[i][C] * (z + size) + m_frustum[i][D] > 0)
			continue;
		if (m_frustum[i][A] * (x - size) + m_frustum[i][B] * (y + size) + m_frustum[i][C] * (z + size) + m_frustum[i][D] > 0)
			continue;
		if (m_frustum[i][A] * (x + size) + m_frustum[i][B] * (y + size) + m_frustum[i][C] * (z + size) + m_frustum[i][D] > 0)
			continue;

		// If we get here, it isn't in the frustum
		return false;
	}

	return true;
}

bool OvRendering::Data::Frustum::BoundingSphereInFrustum(const OvRendering::Geometry::BoundingSphere& p_boundingSphere, const OvMaths::FTransform& p_transform) const
{
	const auto& position = p_transform.GetWorldPosition();
	const auto& rotation = p_transform.GetWorldRotation();
	const auto& scale = p_transform.GetWorldScale();

	float maxScale = std::max(std::max(std::max(scale.x, scale.y), scale.z), 0.0f);
	float scaledRadius = p_boundingSphere.radius * maxScale;
	auto sphereOffset = OvMaths::FQuaternion::RotatePoint(p_boundingSphere.position, rotation) * maxScale;

	OvMaths::FVector3 worldCenter = position + sphereOffset;

	return SphereInFrustum(worldCenter.x, worldCenter.y, worldCenter.z, scaledRadius);
}

bool OvRendering::Data::Frustum::IsMeshInFrustum(const OvRendering::Resources::Mesh& p_mesh, const OvMaths::FTransform& p_transform) const
{
	return BoundingSphereInFrustum(p_mesh.GetBoundingSphere(), p_transform);
}

std::vector<OvRendering::Resources::Mesh*> OvRendering::Data::Frustum::GetMeshesInFrustum(const OvRendering::Resources::Model& p_model, const OvRendering::Geometry::BoundingSphere& p_modelBoundingSphere, const OvMaths::FTransform& p_modelTransform, OvRendering::Settings::ECullingOptions p_cullingOptions) const
{
	const bool frustumPerModel = OvRendering::Settings::IsFlagSet(Settings::ECullingOptions::FRUSTUM_PER_MODEL, p_cullingOptions);

	if (!frustumPerModel || BoundingSphereInFrustum(p_modelBoundingSphere, p_modelTransform))
	{
		std::vector<OvRendering::Resources::Mesh*> result;

		const bool frustumPerMesh = OvRendering::Settings::IsFlagSet(Settings::ECullingOptions::FRUSTUM_PER_MESH, p_cullingOptions);
		const auto& meshes = p_model.GetMeshes();

		for (auto mesh : meshes)
		{
			// Do not check if the mesh is in frustum if the model has only one mesh, because model and mesh bounding sphere are equals
			if (meshes.size() == 1 || !frustumPerMesh || IsMeshInFrustum(*mesh, p_modelTransform))
			{
				result.push_back(mesh);
			}
		}

		return result;
	}

	return {};
}

std::array<float, 4> OvRendering::Data::Frustum::GetNearPlane() const
{
	return { m_frustum[FRONT][0], m_frustum[FRONT][1], m_frustum[FRONT][2], m_frustum[FRONT][3] };
}

std::array<float, 4> OvRendering::Data::Frustum::GetFarPlane() const
{
	return { m_frustum[BACK][0], m_frustum[BACK][1], m_frustum[BACK][2], m_frustum[BACK][3] };
}
