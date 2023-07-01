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
	//bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

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

	ConstructorHelpers::FObjectFinder<UInputAction> LookAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Look.IA_Look'"));
	if (LookAsset.Succeeded())
		LookAction = LookAsset.Object;

	ConstructorHelpers::FObjectFinder<UInputAction> JumpAsset(TEXT("/Script/EnhancedInput.InputAction'/Game/Input/IA_Jump.IA_Jump'"));
	if (JumpAsset.Succeeded())
		JumpAction = JumpAsset.Object;
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
		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AJ1Character::IA_Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AJ1Character::IA_Look);

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
	}
}

void AJ1Character::IA_Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AJ1Character::IA_Look(const FInputActionValue& Value)
{
	FVector2D Val = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Val.X);
		AddControllerPitchInput(-Val.Y);
	}
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
