// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/RoadAnimInstance.h"
#include "Characters/RoadCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void URoadAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	RoadCharacter = Cast<ARoadCharacter>(TryGetPawnOwner());
	if (RoadCharacter)
	{
		RoadCharacterMovement = RoadCharacter->GetCharacterMovement();
	}
}

void URoadAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);

	if (RoadCharacterMovement)
	{
		GroundSpeed = UKismetMathLibrary::VSizeXY(RoadCharacterMovement->Velocity);
		IsFalling = RoadCharacterMovement->IsFalling();
		CharacterState = RoadCharacter->GetCharacterState();
	}
}
