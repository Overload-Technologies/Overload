/**
* @project: baregl
* @author: Adrien Givry
* @licence: MIT
*/

#include <baregl/detail/NativeObject.h>

#include <baregl/debug/Assert.h>
#include <baregl/detail/glad/glad.h>
#include <baregl/detail/Types.h>
#include <baregl/Buffer.h>

namespace baregl::detail
{
	NativeObject::NativeObject(NativeID p_id) :
		m_id{p_id}
	{
	}

	NativeID NativeObject::GetID() const
	{
		return m_id;
	}
}
