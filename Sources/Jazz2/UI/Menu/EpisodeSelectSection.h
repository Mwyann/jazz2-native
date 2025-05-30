﻿#pragma once

#include "ScrollableMenuSection.h"

namespace Jazz2::UI::Menu
{
#ifndef DOXYGEN_GENERATING_OUTPUT
	enum class EpisodeDataFlags {
		None = 0x00,

		IsAvailable = 0x01,
		IsMissing = 0x02,
		IsCompleted = 0x04,
		CanContinue = 0x08,
		CheatsUsed = 0x10,
		LevelsInUnknownDirectory = 0x20,
		RedirectToCustomLevels = 0x40
	};

	DEATH_ENUM_FLAGS(EpisodeDataFlags);

	struct EpisodeData {
		Episode Description;
		EpisodeDataFlags Flags;
	};
#endif

	/** @brief Episode selection menu section */
	class EpisodeSelectSection : public ScrollableMenuSection<EpisodeData>
	{
	public:
		EpisodeSelectSection(bool multiplayer = false, bool privateServer = false);

		void OnUpdate(float timeMult) override;
		void OnDraw(Canvas* canvas) override;
		void OnDrawClipped(Canvas* canvas) override;
		void OnDrawOverlay(Canvas* canvas) override;
		void OnTouchEvent(const TouchEvent& event, Vector2i viewSize) override;

	protected:
		void OnTouchUp(std::int32_t newIndex, Vector2i viewSize, Vector2i touchPos) override;
		void OnExecuteSelected() override;
		void OnDrawEmptyText(Canvas* canvas, std::int32_t& charOffset) override;
		void OnDrawItem(Canvas* canvas, ListViewItem& item, std::int32_t& charOffset, bool isSelected) override;
		void OnHandleInput() override;

	private:
		float _expandedAnimation;
		float _transitionTime;
		std::int32_t _transitionFromEpisode;
		float _transitionFromEpisodeTime;
		bool _multiplayer;
		bool _privateServer;
		bool _expanded;
		bool _shouldStart;

		void OnAfterTransition();
		void AddEpisode(StringView episodeFile);
	};
}