// Yoan Rock @2024


#include "DroneMovementTimelineComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"


UDroneMovementTimelineComponent::UDroneMovementTimelineComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UDroneMovementTimelineComponent::BeginPlay()
{
	Super::BeginPlay();

	SetComponentTickEnabled(false);

	if(!ensure(LocationCurve && HeightCurve))
	{
		UE_LOG(LogTemp, Warning, TEXT("LocationCurve && HeightCurve not defined"));
		return;
	}
}


// Called every frame
void UDroneMovementTimelineComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                    FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (IsDroneMoving())
	{
		return;
	}
	
	if(CheckIfNeedToStop())
	{
		Stop();
	}
	
	const float CurrentTime = GetCurrentTime();
	if(CheckIfFinished(CurrentTime))
	{
		Finished();
	}
	
	// Calculating NewLocation
	const FVector LerpLocation = FMath::Lerp(DroneStartLocation, DroneTargetLocation, GetCurrentLocationCurve(CurrentTime));
	const FVector NewLocation = LerpLocation + FVector(0.f,0.f, GetCurrentHeightCurve(CurrentTime) * ZOffset);

	// Calculating NewRotation
	const FRotator NewRotation = FMath::RInterpTo(
		DroneStaticMeshComponent->GetComponentRotation(), // Previous Rotation
		UKismetMathLibrary::FindLookAtRotation(LerpLocation, GetTargetLocation()), // New Rotation
		DeltaTime,
		InterpSpeed);

	// Updating Location & Rotation
	DroneStaticMeshComponent->SetWorldLocationAndRotation(NewLocation, NewRotation, false, nullptr, ETeleportType::TeleportPhysics);
}

float UDroneMovementTimelineComponent::GetCurrentTime() const
{
	const float CurrentTime = UGameplayStatics::GetRealTimeSeconds(GetOwner()) - TimeWhenDroneStartedToMove;
	
	return bIsDroneReturning ? MaxDurationInSeconds - CurrentTime : CurrentTime;
}

bool UDroneMovementTimelineComponent::IsDroneMoving() const
{
	return TimeWhenDroneStartedToMove == -1.f;
}

FVector UDroneMovementTimelineComponent::GetTargetLocation() const
{
	return bIsDroneReturning ? DroneTargetLocation : DroneStartLocation;
}

float UDroneMovementTimelineComponent::GetCurrentLocationCurve(const float CurrentTime) const
{
	return LocationCurve->GetFloatValue(CurrentTime);
}

float UDroneMovementTimelineComponent::GetCurrentHeightCurve(const float CurrentTime) const
{
	return HeightCurve->GetFloatValue(CurrentTime);
}

void UDroneMovementTimelineComponent::MoveDrone(
	const bool IsDroneReturningIn,
	UStaticMeshComponent* DroneStaticMeshComponentIn,
	AActor* SupplyStationIn,
	const FVector& DroneStartLocationIn,
	const FVector& DroneTargetLocationIn)
{
	bIsDroneReturning = IsDroneReturningIn;
	DroneStaticMeshComponent = DroneStaticMeshComponentIn;
	SupplyStation = SupplyStationIn;
	DroneStartLocation = DroneStartLocationIn;
	DroneTargetLocation = DroneTargetLocationIn;
	
	TimeWhenDroneStartedToMove = UGameplayStatics::GetRealTimeSeconds(GetOwner());
	
	SetComponentTickEnabled(true);
}

void UDroneMovementTimelineComponent::Stop()
{
	SetComponentTickEnabled(false);
	
	TimeWhenDroneStartedToMove = -1.f;
	OnStopEvent.Broadcast();
}

void UDroneMovementTimelineComponent::Finished()
{
	SetComponentTickEnabled(false);
	
	TimeWhenDroneStartedToMove = -1.f;
	OnFinishedEvent.Broadcast();
}

bool UDroneMovementTimelineComponent::CheckIfFinished(const float CurrentTime) const
{
	return bIsDroneReturning ?
		(CurrentTime <= 0.f) :
		(CurrentTime >= MaxDurationInSeconds);
}

bool UDroneMovementTimelineComponent::CheckIfNeedToStop() const
{
	return !SupplyStation || !DroneStaticMeshComponent;
}

