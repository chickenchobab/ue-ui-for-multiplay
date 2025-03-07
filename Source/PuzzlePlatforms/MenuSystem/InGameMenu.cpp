// Fill out your copyright notice in the Description page of Project Settings.


#include "InGameMenu.h"
#include "Components/Button.h"

bool UInGameMenu::Initialize()
{
  if (!Super::Initialize()) return false;

  if (!CancelButton) return false;
  CancelButton->OnClicked.AddDynamic(this, &UInGameMenu::ShutDown);
  if (!QuitButton) return false;
  QuitButton->OnClicked.AddDynamic(this, &UInGameMenu::ExitGame);

  return true;
}

void UInGameMenu::ExitGame()
{
  if (!MenuInterface) return;
  ShutDown();
  MenuInterface->ReturnToMainMenu();
}
