// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Grabber.generated.h"

class APlayerController;
class AActor;
class UPhysicsHandleComponent;
class UInputComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ESCAPE_API UGrabber : public UActorComponent	
{	
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	// Look for attached Physics Handle
	void FindPhysicsHandleComponent();

	// Look for attached Input Component - If Found Bind Actions
	void SetupInputComponent();

	// Raycast to find the first Physics body in reach
	const FHitResult FindFirstPhysicsBodyInReach();

	// Return current end point of reach
	FVector GetGrabReachEnd() const;

	// Return starting point of grab reach
	FVector GetGrabReachStart() const;

	// Raycast and Grab what's in reach
	void Grab();

	// Release the Physics Component that is grabbed
	void Release();

	UPROPERTY(EditAnywhere)
	float GrabReach = 100.0f;

	UPhysicsHandleComponent* PhysicsHandle = nullptr;
	APlayerController* PlayerController = nullptr;
	UInputComponent* InputComponent = nullptr;
		
};
