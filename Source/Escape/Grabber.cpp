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

	/// Find the Player controller
	PlayerController = GetWorld()->GetFirstPlayerController();

	/// Look for attached Physics Handle
	FindPhysicsHandleComponent();

	/// Look for attached Input Component - If Found Bind Actions
	SetupInputComponent();
}

void UGrabber::SetupInputComponent()
{
	InputComponent = GetOwner()->FindComponentByClass<UInputComponent>();

	// If the input component is not found
	if (InputComponent)
	{
		// Bind Actions
		InputComponent->BindAction("Grab", IE_Pressed, this, &UGrabber::Grab);
		InputComponent->BindAction("Grab", IE_Released, this, &UGrabber::Release);
	}
	else
	{
		// Log Error
		FString OwnerName = GetOwner()->GetName();
		UE_LOG(LogTemp, Error, TEXT("Input Component was not found on: %s"), *OwnerName);
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

	// Protect from Null Pointer
	if (!PhysicsHandle) { return; }

	// If the physics Handle is attached
	if (PhysicsHandle->GrabbedComponent)
	{
		// Move the object that we are holding each frame
		PhysicsHandle->SetTargetLocation(GetGrabReachEnd());
	}
}

const FHitResult UGrabber::FindFirstPhysicsBodyInReach()
{
	/// Setup querry params
	FHitResult Hit;
	FCollisionQueryParams TraceParameters(FName(TEXT("")), false, GetOwner());

	/// Raycast (LineTrace) forward viewpoint in reach (store as private)
	bool TraceResult = GetWorld()->LineTraceSingleByObjectType(
		OUT Hit,
		GetGrabReachStart(),
		GetGrabReachEnd(),
		FCollisionObjectQueryParams(ECollisionChannel::ECC_PhysicsBody), // Look for physics bodies
		TraceParameters
	);

	return Hit;
}

FVector UGrabber::GetGrabReachEnd() const
{
	FVector Location;;
	FRotator Rotation;

	// Protect from Null Pointer
	if (!PlayerController) { return FVector(); }

	PlayerController->GetPlayerViewPoint(
		OUT Location,
		OUT Rotation);

	return Location + Rotation.Vector() * GrabReach;
}

FVector UGrabber::GetGrabReachStart() const
{
	FVector Location;;
	FRotator Rotation;

	// Protect from Null Pointer
	if (!PlayerController) { return FVector(); }

	PlayerController->GetPlayerViewPoint(
		OUT Location,
		OUT Rotation);

	return Location;
}

void UGrabber::Grab()
{
	// Line Trace and see if we reach actors with physicsBody collision channel set
	auto HitResult = FindFirstPhysicsBodyInReach();
	auto ActorHit = HitResult.GetActor();

	// If we hit something
	if (ActorHit)
	{
		auto ComponentToGrab = HitResult.GetComponent(); // Returns the Mesh in our case

		// Protect from Null Pointer
		if (!PhysicsHandle) { return; } 

		// Attach the physics Body
		PhysicsHandle->GrabComponentAtLocationWithRotation(
			ComponentToGrab,
			NAME_None,
			ComponentToGrab->GetOwner()->GetActorLocation(),
			ComponentToGrab->GetOwner()->GetActorRotation()
		);	
	}
}

void UGrabber::Release()
{
	// Protect from Null Pointer
	if (!PhysicsHandle) { return; }

	PhysicsHandle->ReleaseComponent();
}

