// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/HitInterface.h"
#include "GameFramework/Character.h"
#include "Characters/CharacterTypes.h"
#include "Enemy.generated.h"

class UAnimMontage;
class UAttributeComponent;
class UHealthBarComponent;
class UPawnSensingComponent;

// test dfe ldfe

UCLASS()
class THEROAD2_API AEnemy : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual void Tick(float DeltaTime) override;
	void CheckPatrolTarget();
	void CheckCombatTarget();
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	void DirectionalHitReact(const FVector& ImpactPoint);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
protected:
	virtual void BeginPlay() override;
	void Die();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	AActor* ChoosePatrolTarget();

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	
	/*
	Play Montage Functions
	*/
	void PlayHitReactMontage(const FName& SectionName);
private:
	UPROPERTY(EditAnywhere, Category = Sounds)
	USoundBase* HitSound;
	UPROPERTY(EditAnywhere, Category = VisualEffects)
	UParticleSystem* HitParticles;

	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 150.f;

	/*
	* Components
	*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;
	/*
	* Navigation 
	*/
	UPROPERTY()
	class AAIController* EnemyController;

	// Current Patrol Target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 5.f;
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMax = 10.f;

	/*
	* Animation Montages
	*/
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* HitReactMontage;
	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DeathMontage;
public:
};
