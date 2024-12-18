﻿#pragma once

#include "Canvas.h"
#include "Font.h"
#include "../ILevelHandler.h"

#include "../../nCine/Input/InputEvents.h"

namespace Jazz2::UI
{
	enum class MessageLevel
	{
		Unknown,
		Echo,
		Debug,
		Info,
		Important,
		Warning,
		Error,
		Assert,
		Fatal
	};

	class InGameConsole : public Canvas
	{
	public:
		InGameConsole(LevelHandler* levelHandler);
		~InGameConsole();

		void OnInitialized();
		void OnUpdate(float timeMult) override;
		bool OnDraw(RenderQueue& renderQueue) override;
		void OnKeyPressed(const KeyboardEvent& event);
		void OnTextInput(const TextInputEvent& event);

		bool IsVisible() const;
		void Show();
		void Hide();
		void WriteLine(MessageLevel level, String line);

	private:
		static constexpr std::uint16_t MainLayer = 100;
		static constexpr std::uint16_t ShadowLayer = 80;
		static constexpr std::uint16_t FontLayer = 200;
		static constexpr std::uint16_t FontShadowLayer = 120;

		static constexpr std::int32_t MaxLineLength = 256;
		
		LevelHandler* _levelHandler;
		Font* _smallFont;
		char _currentLine[MaxLineLength];
		std::size_t _textCursor;
		float _carretAnim;
		std::int32_t _historyIndex;
		bool _isVisible;

		void ProcessCurrentLine();
		void PruneLogHistory();
		void GetPreviousCommandFromHistory();
		void GetNextCommandFromHistory();
	};
}