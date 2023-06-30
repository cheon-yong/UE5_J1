// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "J1Character.generated.h"

UCLASS()
class J1_API AJ1Character : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AJ1Character();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
	UFUNCTION()
	void IA_Move(const FInputActionValue& Value);

	UFUNCTION()
	void IA_Turn(const FInputActionValue& Value);

	UFUNCTION()
	void IA_LookUp(const FInputActionValue& Value);

public:
	UFUNCTION(BlueprintCallable, Category = Test)
	void FireTest() { }

	UFUNCTION()
	void MoveForward(float Value);

	UFUNCTION()
	void MoveRight(float Value);

	UFUNCTION()
	void MoveTurn(float Value);

	UFUNCTION(BlueprintCallable, Category = Anim)
	float GetMovementHorizontalOffset();

	UFUNCTION(BlueprintCallable, Category = Anim)
	float GetMovementVerticalOffset();

protected:
	UPROPERTY(VisibleAnywhere)
	class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	class UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere)
	float MovementHorizontalOffset;

	UPROPERTY(VisibleAnywhere)
	float MovementVerticalOffset;

private:
	UPROPERTY(EditAnywhere, Category = Input)
	class UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* TurnAction;

	UPROPERTY(EditAnywhere, Category = Input)
	class UInputAction* LookUpAction;
};
