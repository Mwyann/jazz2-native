﻿#include "Copter.h"
#include "../../ILevelHandler.h"
#include "../Player.h"

namespace Jazz2::Actors::Environment
{
	Copter::Copter()
		: _phase(0.0f), _state(State::Free)
	{
	}

	Copter::~Copter()
	{
#if defined(WITH_AUDIO)
		if (_noise != nullptr) {
			_noise->stop();
			_noise = nullptr;
		}
#endif
	}

	Task<bool> Copter::OnActivatedAsync(const ActorActivationDetails& details)
	{
		_state = (details.Params[0] != 0 ? State::Mounted : State::Free);

		SetState(ActorState::CanBeFrozen | ActorState::ApplyGravitation, false);

		async_await RequestMetadataAsync("Enemy/LizardFloat"_s);

		SetAnimation((AnimState)1);

		_originPos = _pos;

		async_return true;
	}

	void Copter::OnUpdate(float timeMult)
	{
		switch (_state) {
			case State::Free: {
				_phase += timeMult * 0.05f;
				MoveInstantly(_originPos + Vector2f(0.0f, sinf(_phase) * 4.0f), MoveType::Absolute);
				OnUpdateHitbox();
				break;
			}
			case State::Unmounted: {
				ActorBase::OnUpdate(timeMult);

				_speed.Y = _levelHandler->GetGravity() * -0.5f;
				_renderer.setAlphaF(_renderer.alpha() - (0.004f * timeMult));
				_phase -= timeMult;
				if (_phase <= 0.0f) {
					DecreaseHealth(INT32_MAX);
				}
				break;
			}
		}

#if defined(WITH_AUDIO)
		if (_noise != nullptr) {
			float newGain = _noise->gain() - (_noiseDec * timeMult);
			if (newGain <= 0.0f) {
				_noise->stop();
				_noise = nullptr;
			} else {
				_noise->setGain(newGain);
				_noise->setPosition(Vector3f(_pos.X, _pos.Y, 0.8f));
			}
		}
#endif
	}

	void Copter::OnDetach(ActorBase* parent)
	{
		DecreaseHealth(INT32_MAX);
	}

	void Copter::Preload(const ActorActivationDetails& details)
	{
		PreloadMetadataAsync("Enemy/LizardFloat"_s);
	}

	bool Copter::OnHandleCollision(std::shared_ptr<ActorBase> other)
	{
		if (_state == State::Free || _state == State::Unmounted) {
			if (auto* player = runtime_cast<Player>(other.get())) {
				if (player->GetModifier() == Player::Modifier::None && player->SetModifier(Player::Modifier::LizardCopter, shared_from_this())) {
					_state = State::Mounted;
					_renderer.setAlphaF(1.0f);

					PlaySfx("CopterPre"_s);
					return true;
				}
			}
		}

		return ActorBase::OnHandleCollision(std::move(other));
	}

	void Copter::Unmount(float timeLeft)
	{
		if (_state == State::Mounted) {
			_state = State::Unmounted;
			_phase = timeLeft;

#if defined(WITH_AUDIO)
			_noise = PlaySfx("Copter"_s, 0.8f, 0.8f);
			if (_noise != nullptr) {
				_noise->setLooping(true);
				_noiseDec = _noise->gain() * 0.005f;
			}
#endif

			SetState(ActorState::ApplyGravitation, true);
		}
	}
}