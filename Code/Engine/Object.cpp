#include "Object.hpp"

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// RefCounted
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	RefCounted::RefCounted(void)
		: m_rc(new Counter)
	{
		m_rc->object = this;
	}
	//----------------------------------------------------------------------------//
	RefCounted::~RefCounted(void)
	{
		ASSERT(m_rc == nullptr);
	}
	//----------------------------------------------------------------------------//
	void RefCounted::AddRef(void)
	{
		m_rc->ref++;
	}
	//----------------------------------------------------------------------------//
	void RefCounted::Release(void)
	{
		if (!--m_rc->ref)
			_DeleteThis();
	}
	//----------------------------------------------------------------------------//
	void RefCounted::_DeleteThis(void)
	{
		m_rc->object = nullptr;
		m_rc->Release();
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	// Object
	//----------------------------------------------------------------------------//

	HashMap<uint, Object::TypeInfo> Object::s_types;

	//----------------------------------------------------------------------------//
	Object::TypeInfo* Object::GetOrCreateTypeInfo(const char* _name)
	{
		uint _type = StringUtils::Hash(_name);
		auto _iter = s_types.find(_type);
		if (_iter != s_types.end())
			return &_iter->second;

		LOG("Register %s(0x%04x) typeinfo", _name, _type);

		auto& _typeInfo = s_types[_type];
		_typeInfo.type = _type;
		_typeInfo.name = _name;

		return &_typeInfo;
	}
	//----------------------------------------------------------------------------//
	Object::TypeInfo* Object::GetTypeInfo(uint _type)
	{
		auto _iter = s_types.find(_type);
		if (_iter != s_types.end())
			return &_iter->second;
		return nullptr;
	}
	//----------------------------------------------------------------------------//
	ObjectPtr Object::Create(const char* _name)
	{
		TypeInfo* _typeinfo = GetTypeInfo(_name);
		if (_typeinfo && _typeinfo->Factory)
			return _typeinfo->Factory();

		LOG("Error: Factory for %s not found", _name);
		return nullptr;
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//
}
