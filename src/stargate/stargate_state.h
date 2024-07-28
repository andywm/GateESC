#pragma once

enum class EStargateState
{
	Idle,
	DialingAddress,
	Wormhole,
	Reset
};

using uint8 = unsigned char;
static constexpr uint8 NullAddress = 0xFF;
static constexpr uint8 NullPosition = 0xFF;
