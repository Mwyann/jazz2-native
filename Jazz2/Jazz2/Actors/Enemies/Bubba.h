﻿#pragma once

#include "BossBase.h"

namespace Jazz2::Actors::Enemies
{
	class Bubba : public BossBase
	{
	public:
		Bubba();

		static void Preload(const ActorActivationDetails& details);

	protected:
		Task<bool> OnActivatedAsync(const ActorActivationDetails& details) override;
		bool OnActivatedBoss() override;
		void OnUpdate(float timeMult) override;
		void OnUpdateHitbox() override;
		bool OnPerish(ActorBase* collider) override;

	private:
		static constexpr int StateTransition = -1;
		static constexpr int StateWaiting = 0;
		static constexpr int StateJumping = 1;
		static constexpr int StateFalling = 2;
		static constexpr int StateTornado = 3;
		static constexpr int StateDying = 4;

		int _state = StateWaiting;
		float _stateTime;
		uint8_t _endText;

		void FollowNearestPlayer();
		void TornadoToNearestPlayer();
	};
}