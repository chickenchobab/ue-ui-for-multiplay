// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformsGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "PlatformTrigger.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/InGameMenu.h"

UPuzzlePlatformsGameInstance::UPuzzlePlatformsGameInstance(const FObjectInitializer & ObjectInitializer)
{
  ConstructorHelpers::FClassFinder<UMenuWidget> MenuBPClass(TEXT("/Game/MenuSystem/WBP_MainMenu"));
  if (!MenuBPClass.Class) return;
  MainMenuClass = MenuBPClass.Class;

  ConstructorHelpers::FClassFinder<UMenuWidget> InGameMenuBPClass(TEXT("/Game/MenuSystem/WBP_InGameMenu"));
  if (!InGameMenuBPClass.Class) return;
  InGameMenuClass = InGameMenuBPClass.Class;
}

void UPuzzlePlatformsGameInstance::Init()
{
  UE_LOG(LogTemp, Display, TEXT("Found class %s"), *MainMenuClass->GetName());
}

void UPuzzlePlatformsGameInstance::LoadMainMenu()
{
  if (!MainMenuClass) return;

  MainMenu = CreateWidget<UMainMenu>(this, MainMenuClass);
  if (!MainMenu) return;
  
  MainMenu->SetUp();
  MainMenu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::LoadInGameMenu()
{
  if (!InGameMenuClass) return;

  InGameMenu = CreateWidget<UInGameMenu>(this, InGameMenuClass);
  if (!InGameMenu) return;
  
  InGameMenu->SetUp();
  InGameMenu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::Host()
{
  if (MainMenu) MainMenu->ShutDown();

  UEngine* Engine = GetEngine();
  if (!Engine) return;

  Engine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Hosting"));

  UWorld* World = GetWorld();
  if (!World) return;

  World->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap");
}

void UPuzzlePlatformsGameInstance::Join(const FString& Address)
{
  if (MainMenu) MainMenu->ShutDown();
  
  UEngine* Engine = GetEngine();
  if (!Engine) return;

  Engine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Joining to %s"), *Address));

  APlayerController* PlayerController = GetFirstLocalPlayerController();
  if (!PlayerController) return;

  PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}

void UPuzzlePlatformsGameInstance::ReturnToMainMenu()
{
  APlayerController* PlayerController = GetFirstLocalPlayerController();
  if (!PlayerController) return;

  PlayerController->ClientTravel("/Game/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);
}
