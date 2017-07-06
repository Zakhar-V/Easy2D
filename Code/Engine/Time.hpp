#pragma once

#include "System.hpp"

namespace Easy2D
{
	//----------------------------------------------------------------------------//
	// Time
	//----------------------------------------------------------------------------//

#define gTime Time::Instance

	class Time : public Module<Time>
	{
	public:
		//!
		bool OnEvent(int _type, void* _arg) override;

		//!	\return current time in seconds
		double Current(void);
		//! \return scaled frame time in seconds
		float Delta(void) { return m_deltaTime; }
		//!	\return unscaled frame time in seconds
		float UnscaledDelta(void) { return m_unscaledDeltaTime; }
		//!
		void Scale(float _scale);
		//!
		float Scale(void) { return m_timeScale; }

	protected:
		double m_prevTime = 0;
		float m_unscaledDeltaTime = 0;
		float m_deltaTime = 0;
		float m_timeScale = 1;
	};

	//----------------------------------------------------------------------------//
	//
	//----------------------------------------------------------------------------//
}
