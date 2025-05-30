#pragma once

#include "../Primitives/Vector2.h"
#include "../Primitives/Rect.h"
#include "DisplayMode.h"
#include "../AppConfiguration.h"
#include "../../Main.h"

#include <Containers/SmallVector.h>

using namespace Death::Containers;

#if defined(DEATH_TARGET_EMSCRIPTEN)
class EmscriptenUiEvent;
class EmscriptenFullscreenChangeEvent;
class EmscriptenFocusEvent;
#endif

namespace nCine
{
	/// Represents the interface to the graphics device where everything is rendered
	class IGfxDevice
	{
		friend class Application;
#if defined(WITH_SDL)
		friend class MainApplication;
#endif

	public:
		static constexpr std::uint32_t MaxMonitors = 4;
#if defined(WITH_QT5)
		// Qt5 cannot query the list of supported video modes of a monitor
		static constexpr std::uint32_t MaxVideoModes = 1;
#elif defined(DEATH_TARGET_ANDROID)
		static constexpr std::uint32_t MaxVideoModes = 16;
#else
		static constexpr std::uint32_t MaxVideoModes = 128;
#endif

		/// A structure used to initialize window properties
		struct WindowMode
		{
			WindowMode()
				: width(0), height(0), windowPositionX(AppConfiguration::WindowPositionIgnore), windowPositionY(AppConfiguration::WindowPositionIgnore), isFullscreen(false), isResizable(false), hasWindowScaling(true) { }
			WindowMode(std::int32_t w, std::int32_t h, std::int32_t posX, std::int32_t posY, bool fullscreen, bool resizable, bool windowScaling)
				: width(w), height(h), windowPositionX(posX), windowPositionY(posY), isFullscreen(fullscreen), isResizable(resizable), hasWindowScaling(windowScaling) { }

			std::int32_t width;
			std::int32_t height;
			std::int32_t windowPositionX;
			std::int32_t windowPositionY;
			bool isFullscreen;
			bool isResizable;
			bool hasWindowScaling;
		};

		/// A structure representing a video mode supported by a monitor
		struct VideoMode
		{
			VideoMode()
				: width(0), height(0), refreshRate(0.0f), redBits(8), greenBits(8), blueBits(8) { }

			inline bool operator==(const VideoMode& mode) const
			{
				return (width == mode.width && height == mode.height && refreshRate == mode.refreshRate &&
						redBits == mode.redBits && greenBits == mode.greenBits && blueBits == mode.blueBits);
			}
			inline bool operator!=(const VideoMode& mode) const {
				return !operator==(mode);
			}

			std::uint32_t width;
			std::uint32_t height;
			float refreshRate;
			std::uint32_t redBits;
			std::uint32_t greenBits;
			std::uint32_t blueBits;
		};

		/// A structure representing a connected monitor
		struct Monitor
		{
			/// The monitor name
			const char* name;
			/// The position of the monitor's viewport on the virtual screen
			Vector2i position;
			/// The content scale factor
			Vector2f scale;

			/// The number of video modes in the array
			std::int32_t numVideoModes;
			/// The array of video modes supported by the monitor
			VideoMode videoModes[MaxVideoModes];
		};

		/// Contains the attributes to create an OpenGL context
		struct GLContextInfo
		{
			GLContextInfo()
				: majorVersion(0), minorVersion(0), coreProfile(false), forwardCompatible(false), debugContext(false) { }

			explicit GLContextInfo(const AppConfiguration& appCfg)
				: majorVersion(appCfg.glMajorVersion()), minorVersion(appCfg.glMinorVersion()),
				  coreProfile(appCfg.glCoreProfile()), forwardCompatible(appCfg.glForwardCompatible()),
				  debugContext(appCfg.withGlDebugContext) { }

			std::uint32_t majorVersion;
			std::uint32_t minorVersion;
			bool coreProfile;
			bool forwardCompatible;
			bool debugContext;
		};

		IGfxDevice(const WindowMode& windowMode, const GLContextInfo& glContextInfo, const DisplayMode& displayMode);
		virtual ~IGfxDevice() { }

		/// Sets the number of vertical blanks to occur before a buffer swap
		/*! An interval of `-1` will enable adaptive v-sync if available */
		virtual void setSwapInterval(int interval) = 0;

		/// Returns true if the device renders in full screen
		inline bool isFullscreen() const { return isFullscreen_; }
		/// Sets screen resolution with two integers
		virtual void setResolution(bool fullscreen, int width = 0, int height = 0) = 0;

		/// Sets the position of the application window with two integers
		virtual void setWindowPosition(int x, int y) = 0;
		/// Sets the position of the application window with a `Vector2<int>` object
		inline void setWindowPosition(Vector2i position) {
			setWindowPosition(position.X, position.Y);
		}
		/// Sets the application window title
		virtual void setWindowTitle(StringView windowTitle) = 0;
		/// Sets the application window icon
		virtual void setWindowIcon(StringView iconFilename) = 0;

		/// Returns the window or video mode width in screen coordinates
		inline int width() const { return width_; }
		/// Returns the window or video mode height in screen coordinates
		inline int height() const { return height_; }
		/// Returns the window or video mode resolution in screen coordinates as a `Vector2i` object
		inline Vector2i resolution() const { return Vector2i(width_, height_); }
		/// Returns the window or video mode resolution in screen coordinates as a `Rectf` object
		inline Rectf screenRect() const { return Rectf(0.0f, 0.0f, static_cast<float>(width_), static_cast<float>(height_)); }
		/// Returns the window or video mode resolution aspect ratio
		inline float aspect() const { return width_ / static_cast<float>(height_); }
		/// Sets the window size with two integers
		/*! \note If the application is in full screen this method will have no effect. */
		virtual void setWindowSize(int width, int height) = 0;

		/// Returns window position as a `Vector2i` object
		inline virtual const Vector2i windowPosition() const { return Vector2i(0, 0); }

		/// Returns the window width in pixels
		/*! It may differs from `width()` on HiDPI screens */
		inline int drawableWidth() const { return drawableWidth_; }
		/// Returns the window height in pixels
		/*! It may differs from `height()` on HiDPI screens */
		inline int drawableHeight() const { return drawableHeight_; }
		/// Returns the window resolution in pixels as a `Vector2i` object
		inline Vector2i drawableResolution() const { return Vector2i(drawableWidth_, drawableHeight_); }
		/// Returns the window resolution in pixels as a `Rectf` object
		inline Rectf drawableScreenRect() const { return Rectf(0.0f, 0.0f, static_cast<float>(drawableWidth_), static_cast<float>(drawableHeight_)); }
		/// Returns the window drawable resolution aspect ratio
		inline float drawableAspect() const { return drawableWidth_ / static_cast<float>(drawableHeight_); }

		/// Highlights the application window to notify the user
		inline virtual void flashWindow() const { }

		/// Returns the OpenGL context creation attributes
		inline const GLContextInfo& glContextInfo() const { return glContextInfo_; }
		/// Returns display mode
		inline const DisplayMode& displayMode() const { return displayMode_; }

		/// Returns the number of connected monitors
		unsigned int numMonitors() const;
		/// Returns the array index of the primary monitor
		inline virtual unsigned int primaryMonitorIndex() const { return 0; }
		/// Returns the array index of the monitor associated with the window
		inline virtual unsigned int windowMonitorIndex() const { return 0; }
		/// Returns the specified monitor
		const Monitor &monitor(unsigned int index) const;
		/// Returns the monitor that hosts the window
		inline const Monitor& monitor() const { return monitor(windowMonitorIndex()); }

		/// Returns the current video mode for the specified monitor
		virtual const VideoMode& currentVideoMode(unsigned int monitorIndex) const = 0;
		/// Returns the current video mode for the monitor that hosts the window
		inline const VideoMode& currentVideoMode() const { return currentVideoMode(windowMonitorIndex()); }
		/// Sets the video mode that will be used in full screen by the monitor that hosts the window
		/*! \note Call this method <b>before</b> enabling full screen */
		inline virtual bool setVideoMode(unsigned int modeIndex) { return false; }

		/// Returns the scaling factor for application window
		float windowScalingFactor() const;

	protected:
		static constexpr float DefaultDPI = 96.0f;

#ifndef DOXYGEN_GENERATING_OUTPUT
		/// Window width in screen coordinates
		std::int32_t width_;
		/// Window height in screen coordinates
		std::int32_t height_;
		/// Window width in pixels (for HiDPI screens)
		std::int32_t drawableWidth_;
		/// Window height in pixels (for HiDPI screens)
		std::int32_t drawableHeight_;
		/// Whether rendering occurs in full screen
		bool isFullscreen_;
		/// OpenGL context creation attributes
		GLContextInfo glContextInfo_;
		/// Display properties
		DisplayMode displayMode_;

		Monitor monitors_[MaxMonitors];
		std::uint32_t numMonitors_;
		/// Used as a cache to avoid searching the current video mode in a monitor's array
		mutable VideoMode currentVideoMode_;
#endif

		/// Inits the OpenGL viewport based on the drawable resolution
		void initGLViewport();

		/// Updates the array of connected monitors
		inline virtual void updateMonitors() { }

		virtual void setResolutionInternal(int width, int height) = 0;

	private:
		/// Sets up the initial OpenGL state for the scenegraph
		virtual void setupGL();

		/// Updates the screen swapping back and front buffers
		virtual void update() = 0;

#if defined(DEATH_TARGET_EMSCRIPTEN)
		static bool emscriptenHandleResize(int eventType, const EmscriptenUiEvent* event, void* userData);
		static bool emscriptenHandleFullscreen(int eventType, const EmscriptenFullscreenChangeEvent* event, void* userData);

#	if defined(WITH_GLFW)
		static bool emscriptenHandleFocus(int eventType, const EmscriptenFocusEvent* event, void* userData);
#	endif
#endif
	};

#ifndef DOXYGEN_GENERATING_OUTPUT
	/// A fake graphics device which doesn't render anything
	class NullGfxDevice : public IGfxDevice
	{
	public:
		NullGfxDevice()
			: IGfxDevice(WindowMode{}, GLContextInfo{}, DisplayMode{}) {}

		void setSwapInterval(int interval) override {}

		void setResolution(bool fullscreen, int width = 0, int height = 0) override {}

		void setWindowPosition(int x, int y) override {}
		void setWindowTitle(StringView windowTitle) override {}
		void setWindowIcon(StringView iconFilename) override {}

		void setWindowSize(int width, int height) override {}

		void flashWindow() const override {}

		const VideoMode& currentVideoMode(unsigned int monitorIndex) const override { return currentVideoMode_; }

	protected:
		void setResolutionInternal(int width, int height) override {}

	private:
		void setupGL() override {}
		void update() override {}
	};
#endif
}
