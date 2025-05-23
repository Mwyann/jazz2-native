#pragma once

#if defined(WITH_SDL) || defined(DOXYGEN_GENERATING_OUTPUT)

#include "../Input/IInputManager.h"

#include <Containers/SmallVector.h>

#if !defined(CMAKE_BUILD) && defined(__has_include)
#	if __has_include("SDL2/SDL.h")
#		define __HAS_LOCAL_SDL
#	endif
#endif
#if defined(__HAS_LOCAL_SDL)
#	include "SDL2/SDL_events.h"
#	include "SDL2/SDL_mouse.h"
#else
#	include <SDL_events.h>
#	include <SDL_mouse.h>
#endif

using namespace Death::Containers;

namespace nCine::Backends
{
	/// Utility functions to convert between engine key enumerations and SDL ones
	class SdlKeys
	{
	public:
		static Keys keySymValueToEnum(int keysym);
		static int keyModMaskToEnumMask(int keymod);
		static int enumToKeysValue(Keys keysym);
		static int enumToScancode(Keys keysym);
	};

	/// Information about SDL mouse state
	class SdlMouseState : public MouseState
	{
	public:
		SdlMouseState();

		bool isButtonDown(MouseButton button) const override;

	private:
		unsigned int buttons_;

		friend class SdlInputManager;
	};

	/// Information about an SDL scroll event
	class SdlScrollEvent : public ScrollEvent
	{
	public:
		SdlScrollEvent() {}

		friend class SdlInputManager;
	};

	/// Information about SDL keyboard state
	class SdlKeyboardState : public KeyboardState
	{
	public:
		SdlKeyboardState()
		{
			keyState_ = SDL_GetKeyboardState(nullptr);
		}

		inline bool isKeyDown(Keys key) const override
		{
			const std::int32_t sdlKey = SdlKeys::enumToScancode(key);
			if (sdlKey == SDL_SCANCODE_UNKNOWN) {
				return false;
			} else {
				return keyState_[sdlKey] != 0;
			}
		}

		friend class SdlInputManager;

	private:
		const unsigned char *keyState_;
	};

	/// Information about SDL joystick state
	class SdlJoystickState : public JoystickState
	{
	public:
		SdlJoystickState()
			: sdlJoystick_(nullptr) {}

		static constexpr short int MaxAxisValue = 32767;

		bool isButtonPressed(int buttonId) const override;
		unsigned char hatState(int hatId) const override;
		float axisValue(int axisId) const override;

	private:
		SDL_Joystick *sdlJoystick_;

		friend class SdlInputManager;
	};

	/// Class for parsing and dispatching SDL input events
	class SdlInputManager : public IInputManager
	{
	public:
		/// The constructor takes care of opening available joysticks
		SdlInputManager();
		/// The destructor releases every opened joystick
		~SdlInputManager();

		static bool shouldQuitOnRequest();
		static void parseEvent(const SDL_Event &event);

		inline const MouseState& mouseState() const override
		{
			mouseState_.buttons_ = SDL_GetMouseState(&mouseState_.x, &mouseState_.y);
			return mouseState_;
		}

		inline const KeyboardState& keyboardState() const override { return keyboardState_; }

		String getClipboardText() const override;
		bool setClipboardText(StringView text) override;
		StringView getKeyName(Keys key) const override;

		bool isJoyPresent(int joyId) const override;
		const char *joyName(int joyId) const override;
		const JoystickGuid joyGuid(int joyId) const override;
		int joyNumButtons(int joyId) const override;
		int joyNumHats(int joyId) const override;
		int joyNumAxes(int joyId) const override;
		const JoystickState &joystickState(int joyId) const override;
		bool joystickRumble(int joyId, float lowFreqIntensity, float highFreqIntensity, uint32_t durationMs) override;
		bool joystickRumbleTriggers(int joyId, float left, float right, uint32_t durationMs) override;

		void setCursor(Cursor cursor) override;

	private:
		static constexpr int MaxNumJoysticks = 16;

		static TouchEvent touchEvent_;
		static SdlMouseState mouseState_;
		static MouseEvent mouseEvent_;
		static SdlScrollEvent scrollEvent_;
		static SdlKeyboardState keyboardState_;
		static KeyboardEvent keyboardEvent_;
		static TextInputEvent textInputEvent_;

		static SDL_Joystick *sdlJoysticks_[MaxNumJoysticks];
		static SmallVector<SdlJoystickState, MaxNumJoysticks> joystickStates_;
		static JoyButtonEvent joyButtonEvent_;
		static JoyHatEvent joyHatEvent_;
		static JoyAxisEvent joyAxisEvent_;
		static JoyConnectionEvent joyConnectionEvent_;

		static char joyGuidString_[33];

		/// Deleted copy constructor
		SdlInputManager(const SdlInputManager &) = delete;
		/// Deleted assignment operator
		SdlInputManager &operator=(const SdlInputManager &) = delete;

		static void handleJoyDeviceEvent(const SDL_Event &event);
		static int joyInstanceIdToDeviceIndex(SDL_JoystickID instanceId);
	};
}

#endif