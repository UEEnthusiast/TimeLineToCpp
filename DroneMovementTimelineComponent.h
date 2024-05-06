// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Curves/CurveVector.h"
#include "DroneMovementTimelineComponent.generated.h"

// Declare a delegate type
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleEventDelegate);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class TESTPROJECT_API UDroneMovementTimelineComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDroneMovementTimelineComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curves")
	UCurveFloat* LocationCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Curves")
	UCurveFloat* HeightCurve;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float MaxDurationInSeconds = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float InterpSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	float ZOffset = 1000.f;

	UFUNCTION(BlueprintCallable, Category = "Events")
	void MoveDrone(
		const bool IsDroneReturning,
		UStaticMeshComponent* DroneStaticMeshComponent,
		AActor* SupplyStation,
		const FVector& DroneStartLocation,
		const FVector& DroneTargetLocation);

	UFUNCTION(BlueprintCallable, Category = "Events")
	void Stop();

	UFUNCTION(BlueprintCallable, Category = "Events")
	void Finished();

	// Declare a Blueprint event using the delegate type
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FSimpleEventDelegate OnFinishedEvent;

	// Declare a Blueprint event using the delegate type
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FSimpleEventDelegate OnStopEvent;

private:

	inline bool CheckIfFinished(const float CurrentTime) const;

	inline bool CheckIfNeedToStop() const;

	inline float GetCurrentTime() const;

	inline bool IsDroneMoving() const;

	inline FVector GetTargetLocation() const;
	
	inline float GetCurrentLocationCurve(const float CurrentTime) const;

	inline float GetCurrentHeightCurve(const float CurrentTime) const;

	UPROPERTY(VisibleAnywhere)
	float TimeWhenDroneStartedToMove = -1.f;

	UPROPERTY(VisibleAnywhere)
	bool bIsDroneReturning = false;

	UPROPERTY(EditAnywhere)
	AActor* SupplyStation = nullptr;

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* DroneStaticMeshComponent = nullptr;
	
	UPROPERTY(EditAnywhere)
	FVector DroneStartLocation = FVector::Zero();

	UPROPERTY(EditAnywhere)
	FVector DroneTargetLocation = FVector::Zero();
};
