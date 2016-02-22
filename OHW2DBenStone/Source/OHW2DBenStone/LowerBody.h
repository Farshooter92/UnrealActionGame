// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "LowerBody.generated.h"

UCLASS()
class OHW2DBENSTONE_API ALowerBody : public AActor
{
	GENERATED_BODY()

	UPROPERTY()
	class UArrowComponent* ArrowComponent;

	/** Flipbook for the upperbody */
	UPROPERTY(Category = LowerBody, VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UPaperFlipbookComponent* LowerBodyFB;
	
public:	
	// Sets default values for this actor's properties
	ALowerBody();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	
public:

	//////////////////////////////////////////////////////////////////////////
	// Character inlines

	/** Returns ArrowComponent subobject **/
	FORCEINLINE UArrowComponent* GetArrowComponent() const { return ArrowComponent; }
	/** Returns LowerBodyFB subobject **/
	FORCEINLINE class UPaperFlipbookComponent* GetLowerBodyFB() const { return LowerBodyFB; }
};
