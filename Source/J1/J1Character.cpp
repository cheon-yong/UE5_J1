// Fill out your copyright notice in the Description page of Project Settings.


#include "J1Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"

// Sets default values
AJ1Character::AJ1Character()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

	SpringArm->SetupAttachment(RootComponent);
	Camera->SetupAttachment(SpringArm);

	ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshResource(TEXT("/Script/Engine.SkeletalMesh'/Game/ParagonSunWukong/Characters/Heroes/Wukong/Meshes/Wukong.Wukong'"));
	if (MeshResource.Succeeded())
		GetMesh()->SetSkeletalMesh(MeshResource.Object);

	GetMesh()->SetRelativeLocationAndRotation(FVector(0, 0, -88), FRotator(0, -90, 0));
	 
	// Pawn
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// SpringArm
	SpringArm->SetRelativeRotation(FRotator(-30, 0, 0));
	SpringArm->TargetArmLength = 700;
	SpringArm->bUsePawnControlRotation = true;
	SpringArm->bInheritPitch = true;
	SpringArm->bInheritYaw = true;
	SpringArm->bInheritRoll = true;

	// Input
	ConstructorHelpers::FObjectFinder<UInputMappingContext> MappingContext(TEXT("/Script/EnhancedInput.InputMappingContext'/Game/Input/IMC_Default.IMC_Default'"));
	if (MappingContext.Succeeded())
		InputMappingContext = MappingContext.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> MoveAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Move.IA_Move'"));
	if (MoveAsset.Succeeded())
		MoveAction = MoveAsset.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> TurnAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Turn.IA_Turn'"));
	if (TurnAsset.Succeeded())
		TurnAction = TurnAsset.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> LookUpAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_LookUp.IA_LookUp'"));
	if (LookUpAsset.Succeeded())
		LookUpAction = LookUpAsset.Object;

}

// Called when the game starts or when spawned
void AJ1Character::BeginPlay()
{
	Super::BeginPlay();

	// EnhancedInput
	APlayerController* PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		auto* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (Subsystem)
		{
			Subsystem->AddMappingContext(InputMappingContext, 0);
		}
	}
}

// Called every frame
void AJ1Character::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AJ1Character::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AR1Player::FireTest);
	//PlayerInputComponent->BindAxis("MoveForward", this, &AR1Player::MoveForward);
	//PlayerInputComponent->BindAxis("MoveRight", this, &AR1Player::MoveRight);
	//PlayerInputComponent->BindAxis("Turn", this, &AR1Player::MoveTurn);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AJ1Character::IA_Move);
		EnhancedInputComponent->BindAction(TurnAction, ETriggerEvent::Triggered, this, &AJ1Character::IA_Turn);
		EnhancedInputComponent->BindAction(LookUpAction, ETriggerEvent::Triggered, this, &AJ1Character::IA_LookUp);
	}
}

void AJ1Character::IA_Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	MovementHorizontalOffset = MovementVector.Y;
	if (MovementVector.X != 0)
	{
		FRotator Rotator = GetControlRotation();
		FVector Direction = UKismetMathLibrary::GetForwardVector(FRotator(0, Rotator.Yaw, 0));
		AddMovementInput(Direction, MovementVector.X);
	}
	
	MovementVerticalOffset = MovementVector.X;
	if (MovementVector.Y != 0)
	{
		FRotator Rotator = GetControlRotation();
		FVector Direction = UKismetMathLibrary::GetRightVector(FRotator(0, Rotator.Yaw, 0));
		AddMovementInput(Direction, MovementVector.Y);
	}
}

void AJ1Character::IA_Turn(const FInputActionValue& Value)
{
	float Val = Value.Get<float>();
	AddControllerYawInput(Val);
}

void AJ1Character::IA_LookUp(const FInputActionValue& Value)
{
	float Val = Value.Get<float>();
	AddControllerPitchInput(Val);
}

void AJ1Character::MoveForward(float Value)
{
	FRotator Rotator = GetControlRotation();
	FVector Direction = UKismetMathLibrary::GetForwardVector(FRotator(0, Rotator.Yaw, 0));
	AddMovementInput(Direction, Value);
}

void AJ1Character::MoveRight(float Value)
{
	FRotator Rotator = GetControlRotation();
	FVector Direction = UKismetMathLibrary::GetRightVector(FRotator(0, Rotator.Yaw, 0));
	AddMovementInput(Direction, Value);
}

void AJ1Character::MoveTurn(float Value)
{
	AddControllerYawInput(Value);
}

float AJ1Character::GetMovementHorizontalOffset()
{
	return MovementHorizontalOffset;
}

float AJ1Character::GetMovementVerticalOffset()
{
	return MovementVerticalOffset;
}
