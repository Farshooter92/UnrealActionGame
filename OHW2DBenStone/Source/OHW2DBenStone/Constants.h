// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

UENUM(BlueprintType)
enum class EMovementStates : uint8
{
	IDLE UMETA(DisplayName = "IDLE"),
	WALKING UMETA(DisplayName = "WALKING"),
	RUNNING UMETA(DisplayName = "RUNNING"),
	SHOOTING UMETA(DisplayName = "SHOOTING")
};

UENUM(BlueprintType)
enum class EMovementActions : uint8
{
	MOVING_FORWARDS UMETA(DisplayName = "MOVING FORWARDS"),
	STRAFING_RIGHT UMETA(DisplayName = "STRAFING RIGHT"),
	MOVING_BACKWARDS UMETA(DisplayName = "MOVING BACKWARDS"),
	STRAFING_LEFT UMETA(DisplayName = "STRAFING LEFT")
};

/**
 * 
 */
class OHW2DBENSTONE_API Constants
{
public:
	Constants();
	~Constants();
};
