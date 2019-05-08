// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/TriggerVolume.h"
#include "OpenDoor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDoorEvent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BUILDINGESCAPE_API UOpenDoor : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDoor();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(BlueprintAssignable)
		FDoorEvent OnOpen;

	UPROPERTY(BlueprintAssignable)
		FDoorEvent OnClose;


private:
	// UPROPERTY(VisibleAnywhere) macro means that any data on OpenDoor will be viewable in the editor
	UPROPERTY(VisibleAnywhere)
		float OpenAngle = -90.0f;
	// UPROPERTY(EditAnywhere) means that the trigger volume can be editable in the editor
	UPROPERTY(EditAnywhere)
		ATriggerVolume *PressurePlate = nullptr; // always initialize to nullptr, then have an if statement to see if it's null or not

	float TriggerMass = 20.f;
	float GetTotalMassOfActorsOnPlate();
	AActor *Owner = nullptr; // Owner is given a value at begin play, so no if statemetn required
};