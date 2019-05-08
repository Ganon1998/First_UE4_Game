// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "BuildingEscape.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "Components/PrimitiveComponent.h"

#define OUT

// Sets default values for this component's properties
UGrabber::UGrabber()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UGrabber::BeginPlay()
{
	Super::BeginPlay();
	FindPhysicsHandleComponent();
	SetupInputComponent();
}

const FHitResult UGrabber::GetFirstPhysicsBodyInReach()
{
	// get viewpoint of player
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotator;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotator
	);
	// this method modifies the parameters put in, but returns nothing
	//labelling the parameters as OUT will actually change the parameters' values


	// line-trace to detect what the drawlinedebug is touching
	FHitResult hit;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner()); // having false will detect just the collision box, not the actual model itself

	// ray cast or line trace (reaches out to distance)
	GetWorld()->LineTraceSingleByObjectType(
		OUT hit,
		PlayerViewPointLocation,
		GetReachLineEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody),// will only detect physics_bodies
		TraceParameters
	);

	// if the vector touches an actor
	AActor *ActorHit = hit.GetActor();
	if (ActorHit)
		UE_LOG(LogTemp, Warning, TEXT("Line trace touches %s"), *(ActorHit->GetName()));


	return hit;
}

void UGrabber::SetupInputComponent() 
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();
	if (InputComponent)
	{
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab); //UGrabber having :: means that it's location is not in the stack or heap but is in permanent storage stacks and heaps hold instances (MyGrab.Grab, MyGrabPtr->Grab)
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
								 //name of key pressed Action in UE4 editor //sense a typed key //reference itself //grab address of function that do an operation when the key is pressed
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s missing input component"), *GetOwner()->GetName())
	}
}

void UGrabber::FindPhysicsHandleComponent() 
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>(); //looks for attached physics handle
	
	if (PhysicsHandle == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Physics Handle of %s not found!"), *(GetOwner()->GetName()));
	}

}

void UGrabber::Grab() 
{
	// checking to make sure we can reach an actor with a physics body
	auto HitResult = GetFirstPhysicsBodyInReach();
	auto ComponentToGrab = HitResult.GetComponent();
	auto ActorHit = HitResult.GetActor();

	if (ActorHit) //if the player's line trace hits a physics body
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab, //ComponentToGrab
			NAME_None, //grab the bone name, if any
			ComponentToGrab->GetOwner()->GetActorLocation(), //grab location
			ComponentToGrab->GetOwner()->GetActorRotation() //grab rotation
		);
	
}

void UGrabber::Release() 
{
	PhysicsHandle->ReleaseComponent();
}

FVector UGrabber::GetReachLineEnd()
{
	FVector PlayerViewPointLocation;
	FRotator PlayerViewPointRotator;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(
		OUT PlayerViewPointLocation,
		OUT PlayerViewPointRotator
	);
	
	// returns the length of the grabber line
	return PlayerViewPointLocation + PlayerViewPointRotator.Vector() * Reach;
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!PhysicsHandle) // pointer protection
		return;

	// essentially, every frame will have the length of the physics handle based on return value of GetReachLineEnd()
	if (PhysicsHandle->GrabbedComponent) 
	{
		PhysicsHandle->SetTargetLocation(GetReachLineEnd());
	}

}
