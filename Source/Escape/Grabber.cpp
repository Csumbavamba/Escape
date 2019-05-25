// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/InputComponent.h"

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

	// ...
	PlayerController = GetWorld()->GetFirstPlayerController();

	/// Look for attached Physics Handle
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	// If the handle is not found
	if (!PhysicsHandle)
	{
		// Log Error
		FString OwnerName = GetOwner()->GetName();
		UE_LOG(LogTemp, Error, TEXT("Physics Handle was not found on: %s"), *OwnerName);
	}

	/// Look for attached Input Component
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	// If the input component is not found
	if (!InputComponent)
	{
		// Log Error
		FString OwnerName = GetOwner()->GetName();
		UE_LOG(LogTemp, Error, TEXT("Input Component was not found on: %s"), *OwnerName);
	}
	else
	{
		// Bind Actions
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
	}
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	FindActorInRange();
	

	
}

AActor* UGrabber::FindActorInRange()
{
	/// Get the player viewpoint
	FVector Location;;
	FRotator Rotation;

	PlayerController->GetPlayerViewPoint(
		OUT Location,
		OUT Rotation);

	FVector LineTraceEnd = Location + Rotation.Vector() * GrabReach;

	/// Debug Draw the raycast
	DrawDebugLine(
		GetWorld(),
		Location,
		LineTraceEnd,
		FColor(0, 255, 0),
		false,
		0.0f,
		0.0f,
		10.0f
	);

	/// Setup querry params
	FHitResult Hit;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	/// Raycast (LineTrace) forward viewpoint in reach (store as private)
	bool TraceResult = GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		Location,
		LineTraceEnd,
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), // Look for physics bodies
		TraceParameters
	);

	/// See what we hit
	// Check if there was a hit
	if (TraceResult)
	{
		// TODO Remove this when no longer needed
		FString HitActorName = Hit.GetActor()->GetName();
		UE_LOG(LogTemp, Warning, TEXT("Line trace hit: %s"), *HitActorName);

		// Return the Actor that was hit
		return Hit.GetActor();
	}

	// If nothing was found return null
	return nullptr;
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab Pressed!"));
}

