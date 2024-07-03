// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/RoadCharacter.h"
#include "Components/InputComponent.h"
#include "Components/BoxComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GroomComponent.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"

// Sets default values
ARoadCharacter::ARoadCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(SpringArm);

	Hair = CreateDefaultSubobject<UGroomComponent>(TEXT("Hair"));
	Hair->SetupAttachment(GetMesh());
	Hair->AttachmentName = FString("head");

	Eyebrows = CreateDefaultSubobject<UGroomComponent>(TEXT("Eyebrows"));
	Eyebrows->SetupAttachment(GetMesh());
	Eyebrows->AttachmentName = FString("head");
}

// Called when the game starts or when spawned
void ARoadCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	// PlayerController set up
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(RoadContext, 0);
		}
	}

	Tags.Add(FName("RoadCharacter"));
}

void ARoadCharacter::Move(const FInputActionValue& Value)
{
	//if (ActionState != EActionState::EAS_Unoccupied) return;

	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = Controller->GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	if (ActionState != EActionState::EAS_Unoccupied) return;

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);

	//const FVector Forward = GetActorForwardVector();
	//const FVector Right = GetActorRightVector();
	//AddMovementInput(Forward, MovementVector.Y);
	//AddMovementInput(Right, MovementVector.X);
}

void ARoadCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);
}

// Called every frame
void ARoadCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void ARoadCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Movement
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &ARoadCharacter::Move);
		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ARoadCharacter::Look);
		// Jump
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ARoadCharacter::Jump);
		// ItemPickup
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &ARoadCharacter::EKeyPressed);
		// Attack
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &ARoadCharacter::Attack);

		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &ARoadCharacter::Dodge);
	}
}

void ARoadCharacter::Jump()
{
	Super::Jump();
}

void ARoadCharacter::SetWeaponCollisionEnabled(ECollisionEnabled::Type CollisionEnabled)
{
	if (EquippedWeapon && EquippedWeapon->GetWeaponBox())
	{
		EquippedWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		EquippedWeapon->IgnoreActors.Empty();

	}
}


void ARoadCharacter::EKeyPressed()
{	
		AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem);
		if (OverlappingWeapon)
		{
			OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
			CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
			OverlappingItem = nullptr;
			EquippedWeapon = OverlappingWeapon;
		}
		else 
		{
			if (CanDisarm())
			{
				PlayEquipMontage(FName("Unequip"));
				CharacterState = ECharacterState::ECS_Unequipped;
				ActionState = EActionState::EAS_EquipingWeapon;
			}
			else if (CanArm())
			{
				PlayEquipMontage(FName("Equip"));
				CharacterState = ECharacterState::ECS_EquippedOneHandedWeapon;
				ActionState = EActionState::EAS_EquipingWeapon;
			}
		}
}

void ARoadCharacter::Attack()
{
	// use action state to make sure you only attack once
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

bool ARoadCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

bool ARoadCharacter::CanDisarm()
{
	return CharacterState != ECharacterState::ECS_Unequipped && 
		ActionState == EActionState::EAS_Unoccupied;
}

bool ARoadCharacter::CanArm()
{
	return CharacterState == ECharacterState::ECS_Unequipped &&
		ActionState == EActionState::EAS_Unoccupied &&
		EquippedWeapon;
}

void ARoadCharacter::Disarm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void ARoadCharacter::Arm()
{
	if (EquippedWeapon)
	{
		EquippedWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void ARoadCharacter::FinishEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void ARoadCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}


void ARoadCharacter::Dodge()
{
}


void ARoadCharacter::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		const int32 Selection = FMath::RandRange(0, 2);
		FName SectionName = FName();

		// randomly chooses Attack Animation
		switch (Selection)
		{
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		case 2:
			SectionName = FName("Attack3");
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void ARoadCharacter::PlayEquipMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipMontage)
	{
		AnimInstance->Montage_Play(EquipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipMontage);
	}
}



