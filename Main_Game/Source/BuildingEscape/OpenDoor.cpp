// Fill out your copyright notice in the Description page of Project Settings.

#include "OpenDoor.h"
#include "BuildingEscape.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UOpenDoor::UOpenDoor()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UOpenDoor::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();

	if (!PressurePlate) // pointer protection
		return;
}

float UOpenDoor::GetTotalMassOfActorsOnPlate() 
{
	float TotalMass = 0.f;

	// if there is a trigger volume, then check to see if any actors are on it
	if (PressurePlate) 
	{
		TArray<AActor*> OverlappingActors; // an array of actor pointers
		PressurePlate->GetOverlappingActors(OUT OverlappingActors); // OverlappingActor will be out parameter

		for (const auto *Actor : OverlappingActors)
		{
			UE_LOG(LogTemp, Warning, TEXT("%s"), *Actor->GetName());
			TotalMass += Actor->FindComponentByClass<UPrimitiveComponent>()->GetMass();
		}
	}
	else 
	{
		return 0.f;
	}

	return TotalMass;
}

// Called every frame
void UOpenDoor::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (GetTotalMassOfActorsOnPlate() > TriggerMass) // in other words, if trigger volume detects an AActor named ActorThatOpens, then opendoor()
	{
		OnOpen.Broadcast(); // this means to go into UE4's blueprint andrun OnOpen from there
	}
	else
	{
		OnClose.Broadcast(); // this means to go into UE4's blueprint andrun OnClose from there
	}
}

