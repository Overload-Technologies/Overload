/**
* @project: Overload
* @author: Overload Tech.
* @licence: MIT
*/


#include <OvMaths/FVector2.h>
#include <OvMaths/FVector3.h>
#include <OvMaths/FVector4.h>
#include <OvMaths/FMatrix3.h>
#include <OvMaths/FMatrix4.h>
#include <OvMaths/FQuaternion.h>

#include <sol/sol.hpp>

void BindLuaMath(sol::state& p_luaState)
{
	using namespace OvMaths;

	p_luaState.new_usertype<FVector2>("Vector2",
		/* Constructors */
		sol::constructors<
			FVector2(),
			FVector2(float, float)
		>(),

		/* Operators */
		sol::meta_function::addition, &FVector2::operator+,
		sol::meta_function::subtraction, sol::resolve<FVector2(const FVector2&) const>(&FVector2::operator-),
		sol::meta_function::unary_minus, sol::resolve<FVector2() const>(&FVector2::operator-),
		sol::meta_function::multiplication, &FVector2::operator*,
		sol::meta_function::division, &FVector2::operator/,
		sol::meta_function::to_string, [](const FVector2& target) { return "(" + std::to_string(target.x) + "," + std::to_string(target.y) + ")"; },

		/* Variables */
		"x", &FVector2::x,
		"y", &FVector2::y,

		/* Static Consts */
		"One", []() { return FVector2::One; },
		"Zero", []() { return FVector2::Zero; },

		/* Methods */
		"Length", &FVector2::Length,
		"Dot", &FVector2::Dot,
		"Normalize", &FVector2::Normalize,
		"Lerp", &FVector2::Lerp,
		"AngleBetween", &FVector2::AngleBetween
	);

	p_luaState.new_usertype<FVector3>("Vector3",
		/* Constructors */
		sol::constructors<
			FVector3(),
			FVector3(float, float, float)
		>(),

		/* Operators */
		sol::meta_function::addition, &FVector3::operator+,
		sol::meta_function::subtraction, sol::resolve<FVector3(const FVector3&) const>(&FVector3::operator-),
		sol::meta_function::unary_minus, sol::resolve<FVector3() const>(&FVector3::operator-),
		sol::meta_function::multiplication, sol::overload(
			sol::resolve<FVector3(float) const>(&FVector3::operator*),
			sol::resolve<FVector3(const FVector3&) const>(&FVector3::operator*)
		),
		sol::meta_function::division, &FVector3::operator/,
		sol::meta_function::to_string, [](const FVector3& target) { return "(" + std::to_string(target.x) + "," + std::to_string(target.y) + "," + std::to_string(target.z) + ")"; },

		/* Variables */
		"x", &FVector3::x,
		"y", &FVector3::y,
		"z", &FVector3::z,

		/* Static Consts */
		"One", []() { return FVector3::One; },
		"Zero", []() { return FVector3::Zero; },
		"Forward", []() { return FVector3::Forward; },
		"Up", []() { return FVector3::Up; },
		"Right", []() { return FVector3::Right; },
		"Backward", []() { return -FVector3::Forward; },
		"Down", []() { return -FVector3::Up; },
		"Left", []() { return -FVector3::Right; },

		/* Methods */
		"Length", &FVector3::Length,
		"Dot", &FVector3::Dot,
		"Cross", &FVector3::Cross,
		"Normalize", &FVector3::Normalize,
		"Lerp", &FVector3::Lerp,
		"AngleBetween", &FVector3::AngleBetween,
		"Distance", &FVector3::Distance
	);

	p_luaState.new_usertype<FVector4>("Vector4",
		/* Constructors */
		sol::constructors<
			FVector4(),
			FVector4(float, float, float, float)
		>(),

		/* Operators */
		sol::meta_function::addition, &FVector4::operator+,
		sol::meta_function::subtraction, sol::resolve<FVector4(const FVector4&) const>(&FVector4::operator-),
		sol::meta_function::unary_minus, sol::resolve<FVector4() const>(&FVector4::operator-),
		sol::meta_function::multiplication, &FVector4::operator*,
		sol::meta_function::division, &FVector4::operator/,
		sol::meta_function::to_string, [](const FVector4& target) { return "(" + std::to_string(target.x) + "," + std::to_string(target.y) + "," + std::to_string(target.z) + "," + std::to_string(target.w) + ")"; },

		/* Variables */
		"x", &FVector4::x,
		"y", &FVector4::y,
		"z", &FVector4::z,
		"w", &FVector4::w,

		/* Static Consts */
		"One", []() { return FVector4::One; },
		"Zero", []() { return FVector4::Zero; },

		/* Methods */
		"Length", &FVector4::Length,
		"Dot", &FVector4::Dot,
		"Normalize", &FVector4::Normalize,
		"Lerp", &FVector4::Lerp
		);

	p_luaState.new_usertype<FMatrix3>("Matrix3",
		/* Constructors */
		sol::constructors<
			FMatrix3(),
			FMatrix3(float),
			FMatrix3(float, float, float, float, float, float, float, float, float)
		>(),

		/* Operators */
		sol::meta_function::addition, &FMatrix3::operator+,
		sol::meta_function::subtraction, &FMatrix3::operator-,
		sol::meta_function::multiplication, sol::overload(
			sol::resolve<FMatrix3(float) const>(&FMatrix3::operator*),
			sol::resolve<FVector3(const FVector3&) const>(&FMatrix3::operator*),
			sol::resolve<FMatrix3(const FMatrix3&) const>(&FMatrix3::operator*)
		),
		sol::meta_function::division, sol::overload(
			sol::resolve<FMatrix3(float) const>(&FMatrix3::operator/),
			sol::resolve<FMatrix3(const FMatrix3&) const>(&FMatrix3::operator/)
		),
		sol::meta_function::to_string, [](const FMatrix3& target) { return "Can't show matrix as string for now"; },

		/* Static Consts */
		"Identity", []() { return FMatrix3::Identity; },

		/* Methods */
		"IsIdentity", &FMatrix3::IsIdentity,
		"Determinant", &FMatrix3::Determinant,
		"Transpose", &FMatrix3::Transpose,
		"Cofactor", &FMatrix3::Cofactor,
		"Minor", &FMatrix3::Minor,
		"Adjoint", &FMatrix3::Adjoint,
		"Inverse", &FMatrix3::Inverse,
		"Translation", &FMatrix3::Translation,
		"Translate", &FMatrix3::Translate,
		"Rotation", &FMatrix3::Rotation,
		"Rotate", &FMatrix3::Rotate,
		"Scaling", &FMatrix3::Scaling,
		"Scale", &FMatrix3::Scale,
		"GetRow", &FMatrix3::GetRow,
		"GetColumn", &FMatrix3::GetColumn,
		"Get", [](FMatrix3& target, int row, int col) { return target(row, col); },
		"Set", [](FMatrix3& target, int row, int col, float value) { target(row, col) = value; }
	);

	p_luaState.new_usertype<FMatrix4>("Matrix4",
		/* Constructors */
		sol::constructors<
			FMatrix4(),
			FMatrix4(float, float, float, float, float, float, float, float, float, float, float, float, float, float, float, float)
		>(),

		/* Operators */
		sol::meta_function::addition, &FMatrix4::operator+,
		sol::meta_function::subtraction, sol::overload(
			sol::resolve<FMatrix4(float) const>(&FMatrix4::operator-),
			sol::resolve<FMatrix4(const FMatrix4&) const>(&FMatrix4::operator-)
		),
		sol::meta_function::multiplication, sol::overload(
			sol::resolve<FMatrix4(float) const>(&FMatrix4::operator*),
			sol::resolve<FVector4(const FVector4&) const>(&FMatrix4::operator*),
			sol::resolve<FMatrix4(const FMatrix4&) const>(&FMatrix4::operator*)
		),
		sol::meta_function::division, sol::overload(
			sol::resolve<FMatrix4(float) const>(&FMatrix4::operator/),
			sol::resolve<FMatrix4(const FMatrix4&) const>(&FMatrix4::operator/)
		),
		sol::meta_function::to_string, [](const FMatrix4& target) { return "Can't show matrix as string for now"; },

		/* Static Consts */
		"Identity", []() { return FMatrix4::Identity; },

		/* Methods */
		"IsIdentity", &FMatrix4::IsIdentity,
		"Determinant", &FMatrix4::Determinant,
		"Transpose", &FMatrix4::Transpose,
		"Minor", &FMatrix4::GetMinor,
		"Inverse", &FMatrix4::Inverse,
		"Translation", &FMatrix4::Translation,
		"Translate", &FMatrix4::Translate,
		"RotationOnAxisX", &FMatrix4::RotationOnAxisX,
		"RotateOnAxisX", &FMatrix4::RotateOnAxisX,
		"RotationOnAxisY", &FMatrix4::RotationOnAxisY,
		"RotateOnAxisY", &FMatrix4::RotateOnAxisY,
		"RotationOnAxisZ", &FMatrix4::RotationOnAxisZ,
		"RotateOnAxisZ", &FMatrix4::RotateOnAxisZ,
		"RotationYXZ", &FMatrix4::RotationYXZ,
		"RotateYXZ", &FMatrix4::RotateYXZ,
		"Rotation", &FMatrix4::Rotation,
		"Rotate", &FMatrix4::Rotate,
		"Scaling", &FMatrix4::Scaling,
		"Scale", &FMatrix4::Scale,
		"GetRow", &FMatrix4::GetRow,
		"GetColumn", &FMatrix4::GetColumn,
		"CreatePerspective", &FMatrix4::CreatePerspective,
		"CreateView", &FMatrix4::CreateView,
		"CreateFrustum", &FMatrix4::CreateFrustum,
		"Get", [](FMatrix4& target, int row, int col) { return target(row, col); },
		"Set", [](FMatrix4& target, int row, int col, float value) { target(row, col) = value; }
	);

	

	p_luaState.new_usertype<FQuaternion>("Quaternion",
		/* Constructors */
		sol::constructors
		<
		FQuaternion(),
		FQuaternion(float),
		FQuaternion(float, float, float, float),
		FQuaternion(const FVector3&)
		>(),

		/* Operators */
		sol::meta_function::addition, &FQuaternion::operator+,
		sol::meta_function::subtraction, &FQuaternion::operator-,
		sol::meta_function::division, &FQuaternion::operator/,
		sol::meta_function::multiplication, sol::overload
		(
			sol::resolve<FQuaternion(float) const>(&FQuaternion::operator*),
			sol::resolve<FQuaternion(const FQuaternion&) const>(&FQuaternion::operator*),
			sol::resolve<FMatrix3(const FMatrix3&) const>(&FQuaternion::operator*),
			sol::resolve<FVector3(const FVector3&) const>(&FQuaternion::operator*)
		),
		sol::meta_function::to_string, [](const FQuaternion& target) { return "(" + std::to_string(target.x) + "," + std::to_string(target.y) + "," + std::to_string(target.z) + "," + std::to_string(target.w) + ")"; },

		/* Methods */
		"IsIdentity", &FQuaternion::IsIdentity,
		"IsPure", &FQuaternion::IsPure,
		"IsNormalized", &FQuaternion::IsNormalized,
		"Dot", &FQuaternion::DotProduct,
		"Normalize", &FQuaternion::Normalize,
		"Length", &FQuaternion::Length,
		"LengthSquare", &FQuaternion::LengthSquare,
		"GetAngle", &FQuaternion::GetAngle,
		"GetRotationAxis", &FQuaternion::GetRotationAxis,
		"Inverse", &FQuaternion::Inverse,
		"Conjugate", &FQuaternion::Conjugate,
		"Square", &FQuaternion::Square,
		"GetAxisAndAngle", &FQuaternion::GetAxisAndAngle,
		"AngularDistance", &FQuaternion::AngularDistance,
		"Lerp", &FQuaternion::Lerp,
		"Slerp", &FQuaternion::Slerp,
		"Nlerp", &FQuaternion::Nlerp,
		"RotatePoint", sol::overload(
			sol::resolve<FVector3(const FVector3&, const FQuaternion&)>(&FQuaternion::RotatePoint), // Rotate without pivot
			sol::resolve<FVector3(const FVector3&, const FQuaternion&, const FVector3&)>(&FQuaternion::RotatePoint) // Rotate with pivot
		),
		"EulerAngles", &FQuaternion::EulerAngles,
		"ToMatrix3", &FQuaternion::ToMatrix3,
		"ToMatrix4", &FQuaternion::ToMatrix4,

		/* Variables */
		"x", &FQuaternion::x,
		"y", &FQuaternion::y,
		"z", &FQuaternion::z,
		"w", &FQuaternion::w
	);
}
