#include "Core.h"

namespace Vivium {
	Flag::flag_t Flag::m_Flags = 0;

	void Flag::Set(flag_t flag, bool val)
	{
		if (val) {
			m_Flags |= flag;
		}
		else {
			m_Flags &= ~flag;
		}
	}

	void Flag::Toggle(flag_t flag)
	{
		m_Flags ^= flag;
	}

	bool Flag::Test(flag_t flag)
	{
		return m_Flags & flag;
	}
}
