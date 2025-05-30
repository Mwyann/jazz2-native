﻿#pragma once

#include "ScrollableMenuSection.h"

namespace Jazz2::UI::Menu
{
#ifndef DOXYGEN_GENERATING_OUTPUT
	enum class UserProfileOptionsItemType {
#if (defined(DEATH_TARGET_WINDOWS) && !defined(DEATH_TARGET_WINDOWS_RT)) || defined(DEATH_TARGET_UNIX)
		EnableDiscordIntegration,
#endif
		PlayerName,
#if defined(WITH_MULTIPLAYER)
		UniquePlayerID
#endif
	};

	struct UserProfileOptionsItem {
		UserProfileOptionsItemType Type;
		StringView DisplayName;
		bool HasBooleanValue;
		bool IsReadOnly;
	};
#endif

	class UserProfileOptionsSection : public ScrollableMenuSection<UserProfileOptionsItem>
	{
	public:
		UserProfileOptionsSection();
		~UserProfileOptionsSection();

		void OnShow(IMenuContainer* root) override;
		void OnUpdate(float timeMult) override;
		void OnDraw(Canvas* canvas) override;
		void OnDrawOverlay(Canvas* canvas) override;
		void OnKeyPressed(const nCine::KeyboardEvent& event) override;
		void OnTextInput(const nCine::TextInputEvent& event) override;
		NavigationFlags GetNavigationFlags() const override;

	protected:
		void OnHandleInput() override;
		void OnTouchEvent(const nCine::TouchEvent& event, Vector2i viewSize) override;
		void OnTouchUp(std::int32_t newIndex, Vector2i viewSize, Vector2i touchPos) override;
		void OnLayoutItem(Canvas* canvas, ListViewItem& item) override;
		void OnDrawItem(Canvas* canvas, ListViewItem& item, std::int32_t& charOffset, bool isSelected) override;
		void OnExecuteSelected() override;
		void OnBackPressed() override;

	private:
		constexpr static std::uint32_t MaxPlayerNameLength = 24;

		bool _isDirty;
		bool _waitForInput;
		std::size_t _textCursor;
		float _carretAnim;
		String _localPlayerName;
		String _prevPlayerName;
#if defined(DEATH_TARGET_ANDROID)
		String _deviceId;
		Vector2i _initialVisibleSize;
		Recti _currentVisibleBounds;
		float _recalcVisibleBoundsTimeLeft;
#endif

		void RecalcLayoutForScreenKeyboard();
	};
}