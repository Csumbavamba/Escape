// Fill out your copyright notice in the Description page of Project Settings.


#include "Grabber.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Actor.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/InputComponent.h"
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

	// Find the Player controller
	PlayerController = GetWorld()->GetFirstPlayerController();

	/// Look for attached Physics Handle
	FindPhysicsHandleComponent();

	/// Look for attached Input Component
	FindAndBindInputComponent();
}

void UGrabber::FindAndBindInputComponent()
{
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
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
}

void UGrabber::FindPhysicsHandleComponent()
{
	PhysicsHandle = GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();

	// If the handle is not found
	if (!PhysicsHandle)
	{
		// Log Error
		FString OwnerName = GetOwner()->GetName();
		UE_LOG(LogTemp, Error, TEXT("Physics Handle was not found on: %s"), *OwnerName);
	}
}


// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	
	// If the physics Handle is attached
	if (PhysicsHandle->GrabbedComponent)
	{
		// Find the  end of the Reach Line
		FVector LineTraceEnd = FindGrabReachEnd();

		// Move the object that we are holding each frame
		PhysicsHandle->SetTargetLocation(LineTraceEnd);
	}
}

const FHitResult UGrabber::FindFirstPhysicsBodyInReach()
{
	/// Get the player viewpoint
	FVector Location;;
	FRotator Rotation;

	PlayerController->GetPlayerViewPoint(
		OUT Location,
		OUT Rotation);

	FVector LineTraceEnd = Location + Rotation.Vector() * GrabReach;

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
	}

	// If nothing was found return null
	return Hit;
}

FVector UGrabber::FindGrabReachEnd()
{
	/// Get the player viewpoint
	FVector Location;;
	FRotator Rotation;

	PlayerController->GetPlayerViewPoint(
		OUT Location,
		OUT Rotation);

	FVector LineTraceEnd = Location + Rotation.Vector() * GrabReach;

	return LineTraceEnd;
}

void UGrabber::Grab()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab Pressed!"));

	// Line Trace and see if we reach actors with physicsBody collision channel set
	auto HitResult = FindFirstPhysicsBodyInReach();
	auto ActorHit = HitResult.GetActor();

	// If we hit something
	if (ActorHit)
	{
		FString ActorName = ActorHit->GetName();
		UE_LOG(LogTemp, Warning, TEXT("%s can be picked up!"), *ActorName);

		auto ComponentToGrab = HitResult.GetComponent();

		// Attach the physics Body
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			ComponentToGrab->GetOwner()->GetActorRotation()
		);	
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("There is nothing in range to be picked up."));
	}

	
}

void UGrabber::Release()
{
	UE_LOG(LogTemp, Warning, TEXT("Grab Released!"));

	PhysicsHandle->ReleaseComponent();
}

