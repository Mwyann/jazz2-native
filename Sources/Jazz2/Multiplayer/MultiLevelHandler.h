﻿#pragma once

#if defined(WITH_MULTIPLAYER) || defined(DOXYGEN_GENERATING_OUTPUT)

#include "../LevelHandler.h"
#include "MultiplayerGameMode.h"
#include "NetworkManager.h"

namespace Jazz2::Actors::Multiplayer
{
	class RemoteActor;
	class RemotePlayerOnServer;
}

namespace Jazz2::Multiplayer
{
	/**
		@brief Level handler of an online multiplayer game session

		@experimental
	*/
	class MultiLevelHandler : public LevelHandler
	{
		DEATH_RUNTIME_OBJECT(LevelHandler);

		friend class ContentResolver;
#if defined(WITH_ANGELSCRIPT)
		friend class Scripting::LevelScriptLoader;
#endif
		friend class Actors::Multiplayer::RemotePlayerOnServer;

	public:
		MultiLevelHandler(IRootController* root, NetworkManager* networkManager);
		~MultiLevelHandler() override;

		bool Initialize(const LevelInitialization& levelInit) override;

		bool IsLocalSession() const override;
		bool IsPausable() const override;

		float GetDefaultAmbientLight() const override;
		void SetAmbientLight(Actors::Player* player, float value) override;

		void OnBeginFrame() override;
		void OnEndFrame() override;
		void OnInitializeViewport(std::int32_t width, std::int32_t height) override;
		bool OnConsoleCommand(StringView line) override;

		void OnKeyPressed(const KeyboardEvent& event) override;
		void OnKeyReleased(const KeyboardEvent& event) override;
		void OnTouchEvent(const TouchEvent& event) override;

		void AddActor(std::shared_ptr<Actors::ActorBase> actor) override;

		std::shared_ptr<AudioBufferPlayer> PlaySfx(Actors::ActorBase* self, StringView identifier, AudioBuffer* buffer, const Vector3f& pos, bool sourceRelative, float gain, float pitch) override;
		std::shared_ptr<AudioBufferPlayer> PlayCommonSfx(StringView identifier, const Vector3f& pos, float gain = 1.0f, float pitch = 1.0f) override;
		void WarpCameraToTarget(Actors::ActorBase* actor, bool fast = false) override;
		bool IsPositionEmpty(Actors::ActorBase* self, const AABBf& aabb, TileCollisionParams& params, Actors::ActorBase** collider) override;
		void FindCollisionActorsByAABB(const Actors::ActorBase* self, const AABBf& aabb, Function<bool(Actors::ActorBase*)>&& callback) override;
		void FindCollisionActorsByRadius(float x, float y, float radius, Function<bool(Actors::ActorBase*)>&& callback) override;
		void GetCollidingPlayers(const AABBf& aabb, Function<bool(Actors::ActorBase*)>&& callback) override;

		void BroadcastTriggeredEvent(Actors::ActorBase* initiator, EventType eventType, uint8_t* eventParams) override;
		void BeginLevelChange(Actors::ActorBase* initiator, ExitType exitType, StringView nextLevel = {}) override;
		void HandleGameOver(Actors::Player* player) override;
		bool HandlePlayerDied(Actors::Player* player, Actors::ActorBase* collider) override;
		void HandlePlayerWarped(Actors::Player* player, Vector2f prevPos, WarpFlags flags) override;
		void HandlePlayerCoins(Actors::Player* player, std::int32_t prevCount, std::int32_t newCount) override;
		void HandlePlayerGems(Actors::Player* player, std::uint8_t gemType, std::int32_t prevCount, std::int32_t newCount) override;
		void SetCheckpoint(Actors::Player* player, Vector2f pos) override;
		void RollbackToCheckpoint(Actors::Player* player) override;
		void HandleActivateSugarRush(Actors::Player* player) override;
		void ShowLevelText(StringView text, Actors::ActorBase* initiator = nullptr) override;
		StringView GetLevelText(std::uint32_t textId, std::int32_t index = -1, std::uint32_t delimiter = 0) override;
		void OverrideLevelText(std::uint32_t textId, StringView value) override;
		void LimitCameraView(Actors::Player* player, std::int32_t left, std::int32_t width) override;
		void ShakeCameraView(Actors::Player* player, float duration) override;
		void ShakeCameraViewNear(Vector2f pos, float duration) override;
		void SetTrigger(std::uint8_t triggerId, bool newState) override;
		void SetWeather(WeatherType type, uint8_t intensity) override;
		bool BeginPlayMusic(StringView path, bool setDefault = false, bool forceReload = false) override;

		bool PlayerActionPressed(std::int32_t index, PlayerActions action, bool includeGamepads = true) override;
		bool PlayerActionPressed(std::int32_t index, PlayerActions action, bool includeGamepads, bool& isGamepad) override;
		bool PlayerActionHit(std::int32_t index, PlayerActions action, bool includeGamepads = true) override;
		bool PlayerActionHit(std::int32_t index, PlayerActions action, bool includeGamepads, bool& isGamepad) override;
		float PlayerHorizontalMovement(std::int32_t index) override;
		float PlayerVerticalMovement(std::int32_t index) override;

		bool SerializeResumableToStream(Stream& dest) override;

		void OnAdvanceDestructibleTileAnimation(std::int32_t tx, std::int32_t ty, std::int32_t amount) override;

		void AttachComponents(LevelDescriptor&& descriptor) override;
		void SpawnPlayers(const LevelInitialization& levelInit) override;

		/** @brief Returns current game mode */
		MultiplayerGameMode GetGameMode() const;
		/** @brief Sets current game mode */
		bool SetGameMode(MultiplayerGameMode value);

		/** @brief Called when a peer disconnects from the server, see @ref INetworkHandler */
		bool OnPeerDisconnected(const Peer& peer);
		/** @brief Called when a packet is received, see @ref INetworkHandler */
		bool OnPacketReceived(const Peer& peer, std::uint8_t channelId, std::uint8_t packetType, ArrayView<const std::uint8_t> data);

	protected:
		void BeforeActorDestroyed(Actors::ActorBase* actor) override;
		void ProcessEvents(float timeMult) override;
		void PrepareNextLevelInitialization(LevelInitialization& levelInit) override;

		/** @brief Called when a player entered a transition to change the level */
		void HandlePlayerLevelChanging(Actors::Player* player, ExitType exitType);
		/** @brief Called when a player interacts with a spring */
		bool HandlePlayerSpring(Actors::Player* player, Vector2f pos, Vector2f force, bool keepSpeedX, bool keepSpeedY);
		/** @brief Called when a player is going to warp */
		void HandlePlayerBeforeWarp(Actors::Player* player, Vector2f pos, WarpFlags flags);
		/** @brief Called when a player takes a damage */
		void HandlePlayerTakeDamage(Actors::Player* player, std::int32_t amount, float pushForce);
		/** @brief Called when a player requests to synchronize weapon ammo */
		void HandlePlayerRefreshAmmo(Actors::Player* player, WeaponType weaponType);
		/** @brief Called when a player requests to synchronize weapon upgrades */
		void HandlePlayerRefreshWeaponUpgrades(Actors::Player* player, WeaponType weaponType);
		/** @brief Called when a player emits a weapon flare */
		void HandlePlayerEmitWeaponFlare(Actors::Player* player);
		/** @brief Called when a player changes their current weapon */
		void HandlePlayerWeaponChanged(Actors::Player* player);

	private:
		enum class PeerState {
			Unknown,
			LevelLoaded,
			LevelSynchronized
		};

#ifndef DOXYGEN_GENERATING_OUTPUT
		// Doxygen 1.12.0 outputs also private structs/unions even if it shouldn't
		struct PeerDesc {
			Actors::Multiplayer::RemotePlayerOnServer* Player;
			PeerState State;
			std::uint32_t LastUpdated;

			PeerDesc() {}
			PeerDesc(Actors::Multiplayer::RemotePlayerOnServer* player, PeerState state) : Player(player), State(state), LastUpdated(0) {}
		};
#endif

		enum class PlayerFlags {
			None = 0,

			SpecialMoveMask = 0x07,

			IsFacingLeft = 0x10,
			IsVisible = 0x20,
			IsActivelyPushing = 0x40,

			JustWarped = 0x100
		};

		DEATH_PRIVATE_ENUM_FLAGS(PlayerFlags);

#ifndef DOXYGEN_GENERATING_OUTPUT
		// Doxygen 1.12.0 outputs also private structs/unions even if it shouldn't
		struct PlayerState {
			PlayerFlags Flags;
			std::uint64_t PressedKeys;
			std::uint64_t PressedKeysLast;
			std::uint32_t UpdatedFrame;
			//std::uint64_t WarpSeqNum;
			//float WarpTimeLeft;

			PlayerState();
			PlayerState(Vector2f pos, Vector2f speed);
		};

		struct MultiplayerSpawnPoint {
			Vector2f Pos;
			std::uint8_t Team;

			MultiplayerSpawnPoint(Vector2f pos, std::uint8_t team)
				: Pos(pos), Team(team) {}
		};
#endif

		static constexpr float UpdatesPerSecond = 16.0f; // ~62 ms interval
		static constexpr std::int64_t ServerDelay = 64;

		NetworkManager* _networkManager;
		MultiplayerGameMode _gameMode;
		float _updateTimeLeft;
		bool _isServer;
		bool _initialUpdateSent;
		bool _enableSpawning;
		HashMap<Peer, PeerDesc> _peerDesc; // Server: Per peer description
		HashMap<std::uint8_t, PlayerState> _playerStates; // Server: Per (remote) player state
		HashMap<std::uint32_t, std::shared_ptr<Actors::ActorBase>> _remoteActors; // Client: Actor ID -> Remote Actor created by server
		HashMap<Actors::ActorBase*, std::uint32_t> _remotingActors; // Server: Local Actor created by server -> Actor ID
		SmallVector<MultiplayerSpawnPoint, 0> _multiplayerSpawnPoints;
		std::uint32_t _lastSpawnedActorId;	// Server: last assigned actor/player ID, Client: ID assigned by server
		std::uint64_t _seqNum; // Client: sequence number of the last update
		std::uint64_t _seqNumWarped; // Client: set to _seqNum from HandlePlayerWarped() when warped
		bool _suppressRemoting; // Server: if true, actor will not be automatically remoted to other players
		bool _ignorePackets;

		void SynchronizePeers();
		std::uint32_t FindFreeActorId();
		std::uint8_t FindFreePlayerId();
		bool IsLocalPlayer(Actors::ActorBase* actor);
		void ApplyGameModeToAllPlayers(MultiplayerGameMode gameMode);
		void ApplyGameModeToPlayer(MultiplayerGameMode gameMode, Actors::Player* player);

		static bool ActorShouldBeMirrored(Actors::ActorBase* actor);
		static StringView GameModeToString(MultiplayerGameMode mode);

#if defined(DEATH_DEBUG) && defined(WITH_IMGUI)
		static constexpr std::int32_t PlotValueCount = 512;
		
		std::int32_t _plotIndex;
		float _actorsMaxCount;
		float _actorsCount[PlotValueCount];
		float _remoteActorsCount[PlotValueCount];
		float _remotingActorsCount[PlotValueCount];
		float _mirroredActorsCount[PlotValueCount];
		float _updatePacketMaxSize;
		float _updatePacketSize[PlotValueCount];
		float _compressedUpdatePacketSize[PlotValueCount];

		void ShowDebugWindow();
#endif
	};
}

#endif