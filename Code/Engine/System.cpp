#include "System.hpp"

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// System
	//----------------------------------------------------------------------------//

	System* System::s_first = nullptr;
	System* System::s_last = nullptr;

	//----------------------------------------------------------------------------//
	System::System(void)
	{
		m_prev = s_last;
		if (m_prev)
			m_prev->m_next = this;
		else
			s_first = this;
		s_last = this;
	}
	//----------------------------------------------------------------------------//
	System::~System(void)
	{
		if (m_prev)
			m_prev->m_next = m_next;
		else
			s_first = m_next;

		if (m_next)
			m_next->m_prev = m_prev;
		else
			s_last = m_prev;
	}
	//----------------------------------------------------------------------------//
	bool System::SendEvent(int _event, void* _arg)
	{
		/*for (System* i = s_first; i; i = i->m_next)
		{
		if (i->OnEvent(_event, _arg))
		return true;
		}*/
		for (System* i = s_last; i; i = i->m_prev)
		{
			if (i->OnEvent(_event, _arg))
				return true;
		}
		return false;
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//

}
