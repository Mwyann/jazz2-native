﻿#pragma once

#if defined(WITH_MULTIPLAYER) || defined(DOXYGEN_GENERATING_OUTPUT)

#include "../../Main.h"

namespace Jazz2::Multiplayer
{
	/** @brief Packet type broadcasted on the local network */
	enum class BroadcastPacketType
	{
		Null,
		DiscoveryRequest,
		DiscoveryResponse
	};

	/** @brief Packet type going from client to server */
	enum class ClientPacketType
	{
		Null,
		Ping,
		Reserved,

		Auth = 10,
		LevelReady,
		ChatMessage,
		RequestLevelAssets,

		PlayerReady = 30,
		PlayerUpdate,
		PlayerKeyPress,
		PlayerChangeWeaponRequest,
		PlayerSpectate				// TODO
	};

	/** @brief Packet type going from server to client */
	enum class ServerPacketType
	{
		Null,
		Pong,
		Reserved,

		AuthResponse = 70,
		PeerSetProperty = 71,
		AssetChunk = 72,			// TODO

		LoadLevel = 80,
		LevelSetProperty,
		LevelResetProperties,		// TODO
		ShowInGameLobby,
		FadeOut,
		PlaySfx,
		PlayCommonSfx,
		ShowAlert,					// TODO
		ChatMessage,
		SyncTileMap,
		SetTrigger,
		AdvanceTileAnimation,
		RevertTileAnimation,		// TODO

		CreateControllablePlayer = 110,
		CreateRemoteActor,
		CreateMirroredActor,
		DestroyRemoteActor,
		UpdateAllActors,
		MarkRemoteActorAsPlayer,
		UpdatePositionsInRound,

		PlayerSetProperty = 130,
		PlayerResetProperties,
		PlayerRespawn,
		PlayerMoveInstantly,
		PlayerAckWarped,			// TODO
		PlayerActivateForce,		// TODO
		PlayerEmitWeaponFlare,
		PlayerChangeWeapon,
		PlayerTakeDamage,
		PlayerActivateSpring,
		PlayerWarpIn
	};

	/** @brief Peer property type from @ref ServerPacketType::PeerSetProperty */
	enum class PeerPropertyType
	{
		Unknown,

		Connected,
		Disconnected,
		Roasted,

		Count
	};

	/** @brief Level property type from @ref ServerPacketType::LevelSetProperty */
	enum class LevelPropertyType
	{
		Unknown,

		State = 1,
		GameMode,
		
		LevelText = 10,

		Count
	};

	/** @brief Player property type from @ref ServerPacketType::PlayerSetProperty */
	enum class PlayerPropertyType
	{
		Unknown,

		Lives = 1,
		Health,
		Controllable,
		Invulnerable,
		Modifier,
		Dizzy,
		Freeze,
		Shield,

		WeaponAmmo = 30,
		WeaponUpgrades,

		Coins = 60,
		Gems,

		Points = 90,
		PositionInRound,	// TODO: Unused
		Deaths,
		Kills,
		Laps,
		TreasureCollected,

		Count
	};
}

#endif