#include "Time.hpp"
#include <chrono>

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// Time
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	bool Time::OnEvent(int _type, void* _arg)
	{
		switch (_type)
		{
		case SystemEvent::BeginFrame:
		{
			double _ct = Current();
			if (m_prevTime == 0)
				m_prevTime = _ct;
			m_unscaledDeltaTime = (float)(_ct - m_prevTime);
			m_deltaTime = m_timeScale * m_unscaledDeltaTime;
			m_prevTime = _ct;
		} break;
		}

		return false;
	}
	//----------------------------------------------------------------------------//
	double Time::Current(void)
	{
		std::chrono::duration<double> _time;
		_time = std::chrono::duration_cast<decltype(_time)>(std::chrono::high_resolution_clock::now().time_since_epoch());
		return _time.count();
	}
	//----------------------------------------------------------------------------//
	void Time::Scale(float _scale)
	{
		if (_scale < 0)
			_scale = 0;
		m_timeScale = _scale;
	}
	//----------------------------------------------------------------------------//

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//
}

