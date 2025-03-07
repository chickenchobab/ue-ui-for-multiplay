// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"


void UMenuWidget::SetMenuInterface(IMenuInterface* Menu)
{
  MenuInterface = Menu;
}

void UMenuWidget::SetUp()
{
  AddToViewport();

  UWorld* World = GetWorld();
  if (!World) return;

  APlayerController* PlayerController = World->GetFirstPlayerController();
  if (!PlayerController) return;

  bIsFocusable = true;

  FInputModeUIOnly InputModeData;
  InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
  PlayerController->SetInputMode(InputModeData);
  PlayerController->bShowMouseCursor = true;
}

void UMenuWidget::ShutDown()
{
  RemoveFromParent();

  UWorld* World = GetWorld();
  if (!World) return;

  APlayerController* PlayerController = World->GetFirstPlayerController();
  if (!PlayerController) return;

  bIsFocusable = false;

  FInputModeGameOnly InputModeData;
  PlayerController->SetInputMode(InputModeData);
  PlayerController->bShowMouseCursor = false;
}
