#include "Resource.hpp"

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// Resource
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	bool Resource::Load(Stream* _src)
	{
		LOG("Error: Load not supported for %s", TypeName);
		return false;
	}
	//----------------------------------------------------------------------------//
	bool Resource::Save(Stream* _dst)
	{
		LOG("Error: Save not supported for %s", TypeName);
		return false;
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	// ResourceCache
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	ResourceCache::ResourceCache(void)
	{
	}
	//----------------------------------------------------------------------------//
	ResourceCache::~ResourceCache(void)
	{
	}
	//----------------------------------------------------------------------------//
	bool ResourceCache::OnEvent(int _type, void* _arg)
	{
		switch (_type)
		{
		case SystemEvent::Startup:
		{
			//TODO:

		} break;
		case SystemEvent::Shutdown:
		{
			m_resources.clear();

		} break;
		}

		return false;
	}
	//----------------------------------------------------------------------------//
	Resource* ResourceCache::GetResource(const char* _type, const String& _name, uint _typeid, bool _tmp)
	{
		if (!_typeid)
			_typeid = StringUtils::Hash(_type);
		uint _id = StringUtils::Hash(_name.c_str());
		auto& _cache = m_resources[_typeid];

		auto& _exists = _cache.find(_id);
		if (_exists != _cache.end())
			return _exists->second;

		{
			// TODO: find in temporary resources
		}

		Object::TypeInfo* _typeinfo = Object::GetOrCreateTypeInfo(_type);
		if (!_typeinfo->Factory)
		{
			LOG("Error: Unable to create %s \"%s\"", _type, _name.c_str());
			return nullptr;
		}

		ResourcePtr _res = _typeinfo->Factory().Cast<Resource>();
		ASSERT(_res != nullptr);

		if (_tmp)
		{
			// TODO
		}
		else
		{
			_cache[_id] = _res;
		}

		_res->SetName(_name);
		_res->Load(gFileSystem->OpenFile(_name));

		return _res;
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//
}