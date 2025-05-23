#pragma once

#include "InputEvents.h"
#include "../Primitives/Vector2.h"

#include <Containers/String.h>
#include <Containers/StringView.h>

using namespace Death::Containers;

namespace nCine
{
	class IInputEventHandler;
	class JoyMapping;

	/** @brief Well-known joystick types */
	enum class JoystickGuidType {
		Unknown,
		Standard,
		Default,
		Hidapi,
		Xinput
	};

	/** @brief Parsed GUID of a joystick */
	class JoystickGuid
	{
	public:
		JoystickGuid();
		inline JoystickGuid(JoystickGuidType type) {
			fromType(type);
		}
		inline JoystickGuid(StringView string) {
			fromString(string);
		}
		void fromType(JoystickGuidType type);
		void fromString(StringView string);
		bool isValid() const;

		bool operator==(const JoystickGuid& guid) const;

		std::uint8_t data[16];
	};

	/// Interface for parsing and dispatching input events
	class IInputManager
	{
	public:
		enum class Cursor
		{
			/// Mouse cursor behaves normally with default Arrow
			Arrow,
			/// Mouse cursor is hidden but behaves normally
			Hidden,
			/// Mouse cursor is hidden and locked to the window
			/*! \note Mouse movement will be relative if supported (with no acceleration and no scaling) */
			HiddenLocked
		};

		/** @{ @name Constants */

		/** @brief Maximum number of joysticks */
		static const std::int32_t MaxNumJoysticks;

		// From `XInput.h` in DirectX SDK
		static constexpr float LeftStickDeadZone = 7849 / 32767.0f;
		static constexpr float RightStickDeadZone = 8689 / 32767.0f;
		static constexpr float TriggerDeadZone = 30 / 32767.0f;
		static constexpr float AnalogInButtonDeadZone = 0.85f;
		static constexpr float AnalogOutButtonDeadZone = 0.4f;
		static constexpr float TriggerButtonDeadZone = 0.02f;

		/** @} */

		IInputManager() {}
		virtual ~IInputManager() {}
		/// Gets the current input event handler for the manager
		static inline IInputEventHandler* handler() {
			return inputEventHandler_;
		}
		/// Sets the input event handler for the manager
		static void setHandler(IInputEventHandler* inputEventHandler);

		/// Returns current mouse state
		virtual const MouseState& mouseState() const = 0;
		/// Returns current keyboard state
		virtual const KeyboardState& keyboardState() const = 0;

		/// Returns text from the clipboard if any
		virtual String getClipboardText() const;
		/// Sets the clipboard to the specified text
		virtual bool setClipboardText(StringView text);
		/// Returns display name of the specified key
		virtual StringView getKeyName(Keys key) const;

		/// Returns true if the specified joystick is connected
		virtual bool isJoyPresent(int joyId) const = 0;
		/// Returns the name of the specified joystick
		virtual const char* joyName(int joyId) const = 0;
		/// Returns the GUID of the specified joystick
		virtual const JoystickGuid joyGuid(int joyId) const = 0;
		/// Returns the number of available buttons for the specified joystick
		virtual int joyNumButtons(int joyId) const = 0;
		/// Returns the number of available hats for the specified joystick
		virtual int joyNumHats(int joyId) const = 0;
		/// Returns the number of available axes for the specified joystick
		virtual int joyNumAxes(int joyId) const = 0;
		/// Returns the state of the joystick
		virtual const JoystickState& joystickState(int joyId) const = 0;
		/// Starts a main rumble effect with specified duration
		virtual bool joystickRumble(int joyId, float lowFreqIntensity, float highFreqIntensity, uint32_t durationMs) = 0;
		/// Starts a rumble effect on triggers with specified duration
		virtual bool joystickRumbleTriggers(int joyId, float left, float right, uint32_t durationMs) = 0;

		/// Returns `true` if the joystick has a valid mapping configuration
		bool isJoyMapped(int joyId) const;
		/// Returns the state of the mapped joystick
		const JoyMappedState& joyMappedState(int joyId) const;
		/// Modify the joystick axis vector to account for a dead zone
		void deadZoneNormalize(Vector2f& joyVector, float deadZoneValue) const;

		/// Adds joystick mapping configurations from a text file
		void addJoyMappingsFromFile(StringView path);
		/// Adds joystick mapping configurations from a string
		void addJoyMappingsFromString(StringView mappingStrings);
		/// Returns the current number of valid joystick mappings
		unsigned int numJoyMappings() const;
		/// Returns true if mapping exists for specified joystick by GUID
		bool hasMappingByGuid(const JoystickGuid& guid) const;
		/// Returns true if mapping exists for specified joystick by name
		bool hasMappingByName(const char* name) const;

		/// Returns current mouse cursor mode
		inline Cursor cursor() const {
			return cursor_;
		}
		/// Sets the mouse cursor mode
		virtual void setCursor(Cursor cursor);

	protected:
#ifndef DOXYGEN_GENERATING_OUTPUT
		static IInputEventHandler* inputEventHandler_;
		static Cursor cursor_;

		static JoyMapping joyMapping_;
#endif
	};

#ifndef DOXYGEN_GENERATING_OUTPUT
	/// A fake input manager which doesn't process any input
	class NullInputManager : public IInputManager
	{
	public:
		NullInputManager() : mouseState_{}, keyState_{}, joyState_{} {}

		virtual const MouseState& mouseState() const override { return mouseState_; }
		virtual const KeyboardState& keyboardState() const override { return keyState_; }

		String getClipboardText() const override { return {}; }
		StringView getKeyName(Keys key) const override { return {}; }

		bool isJoyPresent(int joyId) const override { return false; }
		const char* joyName(int joyId) const override { return nullptr; }
		const JoystickGuid joyGuid(int joyId) const override { return {}; }
		int joyNumButtons(int joyId) const override { return 0; }
		int joyNumHats(int joyId) const override { return 0; }
		int joyNumAxes(int joyId) const override { return 0; }

		const JoystickState& joystickState(int joyId) const override { return joyState_; }
		bool joystickRumble(int joyId, float lowFreqIntensity, float highFreqIntensity, uint32_t durationMs) override { return false; }
		bool joystickRumbleTriggers(int joyId, float left, float right, uint32_t durationMs) override { return false; }

		void setCursor(Cursor cursor) override {}

	private:
		class NullMouseState : public MouseState
		{
		public:
			bool isButtonDown(MouseButton button) const override { return false; }
		};

		class NullKeyboardState : public KeyboardState
		{
		public:
			bool isKeyDown(Keys key) const override { return false; }
		};

		class NullJoystickState : public JoystickState
		{
		public:
			bool isButtonPressed(int buttonId) const override { return false; }
			unsigned char hatState(int hatId) const override { return 0; }
			float axisValue(int axisId) const override { return 0.0f; }
		};

		NullMouseState mouseState_;
		NullKeyboardState keyState_;
		NullJoystickState joyState_;
	};
#endif
}
