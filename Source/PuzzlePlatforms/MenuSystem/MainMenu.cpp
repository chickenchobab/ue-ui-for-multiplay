// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"

bool UMainMenu::Initialize()
{
  if (!Super::Initialize()) return false;

  if (!HostButton) return false;
  HostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);
  if (!JoinMenuButton) return false;
  JoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);
  if (!QuitButton) return false;
  QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitGameProgram);
  if (!JoinButton) return false;
  JoinButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);
  if (!CancelJoinMenuButton) return false;
  CancelJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

  return true;
}

void UMainMenu::HostServer()
{
  if (!MenuInterface) return;

  MenuInterface->Host();
}

void UMainMenu::JoinServer()
{
  if (!MenuInterface) return;
  if (!IPAddressEntry) return;

  MenuInterface->Join(IPAddressEntry->GetText().ToString());
}

void UMainMenu::QuitGameProgram()
{
  UWorld* World = GetWorld();
  if (!World) return;

  APlayerController* PlayerController = World->GetFirstPlayerController();
  if (!PlayerController) return;

  PlayerController->ConsoleCommand("quit"); 
}

void UMainMenu::OpenJoinMenu()
{
  if (!MenuSwitcher) return;
  if (!JoinMenu) return;
  
  MenuSwitcher->SetActiveWidget(JoinMenu);
}

void UMainMenu::OpenMainMenu()
{
  if (!MenuSwitcher) return;
  if (!MainMenu) return;

  MenuSwitcher->SetActiveWidget(MainMenu);
}
