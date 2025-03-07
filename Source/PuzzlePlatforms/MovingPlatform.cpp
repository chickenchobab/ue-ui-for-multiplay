// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingPlatform.h"

AMovingPlatform::AMovingPlatform()
{
  PrimaryActorTick.bCanEverTick = true;
  SetMobility(EComponentMobility::Movable);
  Speed = 20.f;
}

void AMovingPlatform::BeginPlay()
{
  Super::BeginPlay();

  if (HasAuthority())
  {
    SetReplicates(true);
    SetReplicateMovement(true);
  }

  StartLocation = GetActorLocation();
}

void AMovingPlatform::Tick(float DeltaTime)
{
  Super::Tick(DeltaTime);

  if (ActiveTrigger <= 0) return;
  
  if (HasAuthority())
  {
    static FVector OffsetMoved = FVector::ZeroVector;
    
    if (OffsetMoved.Length() >= Offset.Length())
    {
      SetActorLocation(StartLocation + Offset, true);
      StartLocation = GetActorLocation();
      OffsetMoved = FVector::ZeroVector;
      Offset *= -1;
    }

    FVector CurLocation = GetActorLocation();
    FVector TargetLocation = GetTransform().TransformPosition(Offset);
    FVector NewLocation = FMath::VInterpConstantTo(CurLocation, TargetLocation, DeltaTime, Speed);

    SetActorLocation(NewLocation, true);
    OffsetMoved += (NewLocation - CurLocation);
  }
}

void AMovingPlatform::AddActiveTrigger()
{
  ++ActiveTrigger;
}

void AMovingPlatform::RemoveActiveTrigger()
{
  if (ActiveTrigger > 0)
  {
    --ActiveTrigger;
  }
}

