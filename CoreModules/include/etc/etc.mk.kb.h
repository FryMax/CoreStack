#pragma once
#include "../shared_in.h"

LIB_NAMESPACE_START

namespace
{
	struct SpecKeys
	{
		bool KeyAlt;
		bool KeyCtrl;
		bool KeyShift;
		bool KeySyper; // todo mb change to "KeyPlatform"
	};

	using uk32 = int32_t; //char32_t;

	struct KeyboardEvent
	{
		KKey key_vk; //key
		uk32 key_wk; //unicode
		bool is_press;
		bool is_stick;
	};
}

struct KeyboardState
{
public:
	SpecKeys Switchers;

	//---------// SET

	inline void InputPress(const os::KeyEvent& arg)
	{
		mHasEvent    = true;
		mHasPress   |=  arg.is_press ? true : mHasPress;
		mHasRelease |= !arg.is_press ? true : mHasRelease;

		KeyboardEvent new_event;

		new_event.key_vk   = arg.kk_key;
		new_event.key_wk   = arg.wc_key;
		new_event.is_press = arg.is_press;
		new_event.is_stick = false;

		if (arg.is_press)
		{
			if (!mKeysActive[(int)arg.kk_key])
				mKeysPressed[(int)arg.kk_key] = true;
			else
				new_event.is_stick = true;
		}
		else
		{
			mKeysPressed [(int)arg.kk_key] = false;
			mKeysReleased[(int)arg.kk_key] = true;
		}

		mKeysActive[(int)arg.kk_key] = arg.is_press;

		if (!new_event.is_stick || (_stick_key_event_is_press_event && new_event.is_stick)) //todo check
		{
			mKeyEvents.push_back(new_event);
			mLastEvent = new_event;
		}
	}

	inline void ClearEvents()
	{
 		if (mHasEvent)
		{
			mKeyEvents.clear();

			std::fill(mKeysPressed.begin(),  mKeysPressed.end(),  false);
			std::fill(mKeysReleased.begin(), mKeysReleased.end(), false);

			mHasEvent  = false;
			mHasPress  = false;
			mHasRelease = false;
		}
	}

	//---------// GET

	inline bool IsPressed	(KKey _key) const { return mKeysActive  [int(_key)]; }
	inline bool WasPressed	(KKey _key) const { return mKeysPressed [int(_key)]; }
	inline bool WasReleased	(KKey _key) const { return mKeysReleased[int(_key)]; }

	inline bool HasEvent	() const { return mHasEvent;   }
	inline bool HasPress	() const { return mHasPress;   }
	inline bool HasRelease	() const { return mHasRelease; }

	inline const std::vector<KeyboardEvent>& GetEvents()
	{
 		return mKeyEvents;
 	}

	inline KeyboardEvent GetLastEvent()
	{
		if (mHasEvent)
			return mLastEvent;
		else
			return { KKey(0), 0, false, false };
	}

protected:
	const bool _stick_key_event_is_press_event = true;

	//----------------------------------------------------------------

	std::vector<KeyboardEvent> mKeyEvents;
	KeyboardEvent mLastEvent;

	//----------------------------------------------------------------

	std::array<bool, (size_t)KKey::_enum_size> mKeysActive;   //
	std::array<bool, (size_t)KKey::_enum_size> mKeysPressed;  //
	std::array<bool, (size_t)KKey::_enum_size> mKeysReleased; //

	//----------------------------------------------------------------

	bool mHasEvent   = false;  // any event
	bool mHasPress   = false;  // press
	bool mHasRelease = false;  // release
};

struct MouseState
{
public:
	MouseState()
	{
		ClearState();
	}

	//---------// SET

	inline void InputMove  (const os::MouseEvent &arg)
	{
		mHasEvent = true;
		mHasMove  = true;

		const INT2 new_pos = INT2(arg.x, arg.y);

		frameOffset = localPos - new_pos;
		localPos = new_pos;
	}

	inline void InputPress (const os::MouseClickEvent &arg)
	{
		mHasEvent = true;
		mHasClick = true;

		mHasPress   |= arg.is_click;
		mHasRelease |= !arg.is_click;

 		if (arg.is_click)
		{
			if (!mKeysActive[(size_t)arg.mk_key])
				mKeysPressed[(size_t)arg.mk_key] = true;

			mKeysActive  [(size_t)arg.mk_key] = true;
			mKeysReleased[(size_t)arg.mk_key] = false;
		}
		else
		{
			mKeysActive  [(size_t)arg.mk_key] = false;
 			mKeysPressed [(size_t)arg.mk_key] = false;
			mKeysReleased[(size_t)arg.mk_key] = true;
 		}

		if (_any_event_spawn_move_event)
			InputMove(arg);
	}

	inline void InputWeel  (const os::MouseWheelEvent &arg)
	{
		mHasEvent = true;
		mHasWeel  = true;

		frameWell.x = arg.wheel_x;
		frameWell.y = arg.wheel_x;

		if (_any_event_spawn_move_event)
			InputMove(arg);
	}

	inline void ClearState (void)
	{
		if (mHasEvent)
		{
 			frameOffset = INT2(0, 0);
			frameWell   = INT2(0, 0);

			mHasEvent	= false;
			mHasMove		= false;
			mHasClick	= false;
			mHasPress	= false;
			mHasRelease	= false;
			mHasWeel		= false;

			std::fill(mKeysPressed.begin(), mKeysPressed.end(), false);
			std::fill(mKeysReleased.begin(), mKeysReleased.end(), false);
		}
	}

	//---------// GET

	inline bool IsPressed   (MKey key) const { return mKeysActive  [int(key)]; }
	inline bool WasPressed  (MKey key) const { return mKeysPressed [int(key)]; }
	inline bool WasReleased (MKey key) const { return mKeysReleased[int(key)]; }
	inline bool WasWheel    ()         const { return mHasWeel;                }

	inline INT2 GetLocalPos () const { return localPos;    }
	inline INT2 GetOffset   () const { return frameOffset; }
	inline INT2 GetWeel     () const { return frameWell;   }

	//---------// mb dell

	inline void OSSetPos(int x, int y)
	{
		LOG_WARN_NO_IMPL;
	}
	inline void OSSetVisibility(bool state)
	{
		LOG_WARN_NO_IMPL;
	}

protected:
	std::array<bool, (size_t)MKey::_enum_size> mKeysActive;   //
	std::array<bool, (size_t)MKey::_enum_size> mKeysPressed;  //
	std::array<bool, (size_t)MKey::_enum_size> mKeysReleased; //

	//----------------------------------------------------------------

	const bool _any_event_spawn_move_event = false;

	//----------------------------------------------------------------

	INT2 localPos    = {0, 0};  // window workspace mouse pos
	INT2 frameOffset = {0, 0};  // last frame offset
	INT2 frameWell   = {0, 0};  // last frame weel

	//----------------------------------------------------------------

	bool mHasEvent   = false;  // any event
	bool mHasMove    = false;  // move
	bool mHasClick   = false;  // press/release
	bool mHasPress   = false;  // press
	bool mHasRelease = false;  // release
	bool mHasWeel    = false;  // weel
};

LIB_NAMESPACE_END