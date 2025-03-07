// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MenuSystem/MenuInterface.h"
#include "PuzzlePlatformsGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PUZZLEPLATFORMS_API UPuzzlePlatformsGameInstance : public UGameInstance, public IMenuInterface
{
	GENERATED_BODY()

public:  
	UPuzzlePlatformsGameInstance(const FObjectInitializer & ObjectInitializer);
	
	virtual void Init();

	UFUNCTION(BlueprintCallable)
	void LoadMainMenu();

	UFUNCTION(BlueprintCallable)
	void LoadInGameMenu();

	UFUNCTION(Exec)
	void Host() override;

	UFUNCTION(Exec)
	void Join(const FString& Address) override;

	UFUNCTION(Exec)
	void ReturnToMainMenu() override;

private:
	UPROPERTY()
	TSubclassOf<class UMenuWidget> MainMenuClass;

	UPROPERTY()
	TSubclassOf<class UMenuWidget> InGameMenuClass;

	UPROPERTY()
	class UMainMenu* MainMenu;

	UPROPERTY()
	class UInGameMenu* InGameMenu;
};
