// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "UnrealEngineCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AUnrealEngineCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

public:
	AUnrealEngineCharacter();

	/** 프로퍼티 리플리케이션 */		
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/** 업데이트되는 체력에 반응. 서버에서는 수정 즉시 호출, 클라이언트에서는 RepNotify에 반응하여 호출*/
	void OnHealthUpdate();

	/** 최대 체력 게터
		BlueprintPure : 블루프린트에서 읽기 전용
		FORCEINLINE : 인라인을 강제 수행 => 비용 줄이고 성능 최적화
	*/
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

	/** 현재 체력 게터*/
	UFUNCTION(BlueprintPure, Category = "Health")
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }

	/** 현재 체력 세터. 값을 0과 MaxHealth 사이로 범위제한하고 OnHealthUpdate를 호출합니다. 서버에서만 호출되어야 합니다.
		BlueprintCallable : 블루프린트에서 호출 가능 
	*/
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetCurrentHealth(float healthValue);

	/** 대미지를 받는 이벤트. APawn에서 오버라이드됩니다.*/
	UFUNCTION(BlueprintCallable, Category = "Health")
	float TakeDamage(float DamageTaken, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	//////////////////////////////////////////////////////////////////////////
	// 플레이어 

	/** 플레이어의 최대 체력. 체력의 최댓값입니다. 이 값은 스폰 시 시작되는 캐릭터의 체력 값입니다.*/
	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth;

	/** 플레이어의 현재 체력. 0이 되면 죽은 것으로 간주됩니다.
		UPROPERTY : 변수 속성 설정
		ReplicatedUsing : 네트워크를 통해 복제될 때 호출 함수
	*/
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth)	
	float CurrentHealth;

	/** 현재 체력에 가해진 변경에 대한 RepNotify
		UFUNCTION : 기본적인 리플렉션 기능 활성화 
	*/
	UFUNCTION()
	void OnRep_CurrentHealth();

	//////////////////////////////////////////////////////////////////////////
	// 발사체

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|Projectile")
	TSubclassOf<class AThirdPersonMPProjectile> ProjectileClass;

	/** 발사 딜레이, 단위는 초. 테스트 발사체의 발사 속도를 제어하는 데 사용되지만, 서버 함수의 추가분이 SpawnProjectile을 입력에 직접 바인딩하지 않게 하는 역할도 합니다.*/
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	float FireRate;

	/** true인 경우 발사체를 발사하는 프로세스 도중입니다. */
	bool bIsFiringWeapon;

	/** 무기 발사 시작 함수*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StartFire();

	/** 무기 발사 종료 함수. 호출되면 플레이어가 StartFire를 다시 사용할 수 있습니다.*/
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void StopFire();

	/** 발사체를 스폰하는 서버 함수
		Server : 서버에서 실행되어야 함
		Reliable : 함수 호출이 잃어버리지 않고 반드시 전달 보장
		서버에서 호출되는 RPC
			RPC(Remote Procedure Call) 원격 프로시저 호출 
	*/
	UFUNCTION(Server, Reliable)
	void HandleFire();

	/** 스폰 사이에 발사 속도 딜레이를 넣는 타이머 핸들*/
	FTimerHandle FiringTimer;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);
			

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	// To add mapping context
	virtual void BeginPlay();

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

