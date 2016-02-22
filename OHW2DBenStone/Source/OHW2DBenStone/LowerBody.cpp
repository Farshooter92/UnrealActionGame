// Fill out your copyright notice in the Description page of Project Settings.

#include "OHW2DBenStone.h"
#include "LowerBody.h"
#include "PaperFlipbookComponent.h"

// Sets default values
ALowerBody::ALowerBody()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Arrow
	ArrowComponent = CreateEditorOnlyDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	ArrowComponent->ArrowColor = FColor::Red;
	RootComponent = ArrowComponent;
	ArrowComponent->bIsScreenSizeScaled = true;

	// Set up the lower body flipbook
	LowerBodyFB = CreateOptionalDefaultSubobject<UPaperFlipbookComponent>(TEXT("Lower Body FB"));
	LowerBodyFB->AlwaysLoadOnClient = true;
	LowerBodyFB->AlwaysLoadOnServer = true;
	LowerBodyFB->bOwnerNoSee = false;
	LowerBodyFB->bAffectDynamicIndirectLighting = true;
	LowerBodyFB->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	LowerBodyFB->AttachParent = ArrowComponent;
	static FName CollisionProfileName(TEXT("CharacterMesh"));
	LowerBodyFB->SetCollisionProfileName(CollisionProfileName);
	LowerBodyFB->bGenerateOverlapEvents = false;

}

// Called when the game starts or when spawned
void ALowerBody::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALowerBody::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

