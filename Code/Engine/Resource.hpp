#pragma once

#include "Object.hpp"
#include "System.hpp"
#include "File.hpp"

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// Resource
	//----------------------------------------------------------------------------//
	
	typedef SharedPtr<class Resource> ResourcePtr;

	//!
	class Resource abstract : public Object
	{
	public:
		RTTI("Resource");

		//!
		virtual bool Load(Stream* _src);
		//!
		virtual bool Save(Stream* _dst);

		//!
		void SetName(const String& _name) { m_name = _name; }
		//!
		const String& GetName(void) { return m_name; }

	protected:
		String m_name;
	};

	//----------------------------------------------------------------------------//
	// ResourceCache
	//----------------------------------------------------------------------------//

#define gResources ResourceCache::Instance

	//!
	class ResourceCache : public Module<ResourceCache>
	{
	public:
		//!
		ResourceCache(void);
		//!
		~ResourceCache(void);

		//!
		bool OnEvent(int _type, void* _arg) override;

		//!
		Resource* GetResource(const char* _type, const String& _name, uint _typeid = 0, bool _tmp = false);
		//!
		template <class T> T* GetResource(const String& _name, bool _tmp = false)
		{
			return static_cast<T*>(GetResource(T::TypeName, _name, T::TypeID, _tmp));
		}

	protected:

		HashMap<uint, HashMap<uint, ResourcePtr>> m_resources;
	};

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//

}
