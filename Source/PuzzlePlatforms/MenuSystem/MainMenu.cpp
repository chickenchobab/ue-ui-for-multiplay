// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenu.h"
#include "UObject/ConstructorHelpers.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

#include "ServerRow.h"

UMainMenu::UMainMenu(const FObjectInitializer& ObjectInitializer)
{
  ConstructorHelpers::FClassFinder<UUserWidget> ServerRowBPClass(TEXT("/Game/MenuSystem/WBP_ServerRow"));
  if (!ServerRowBPClass.Class) return;
  ServerRowClass = ServerRowBPClass.Class;
}

bool UMainMenu::Initialize()
{
  if (!Super::Initialize()) return false;
  
  if (!QuitButton) return false;
  QuitButton->OnClicked.AddDynamic(this, &UMainMenu::QuitGameProgram);

  if (!HostMenuButton) return false;
  HostMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenHostMenu);

  if (!HostButton) return false;
  HostButton->OnClicked.AddDynamic(this, &UMainMenu::HostServer);

  if (!CancelHostMenuButton) return false;
  CancelJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

  if (!JoinMenuButton) return false;
  JoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenJoinMenu);
  
  if (!JoinButton) return false;
  JoinButton->OnClicked.AddDynamic(this, &UMainMenu::JoinServer);

  if (!CancelJoinMenuButton) return false;
  CancelJoinMenuButton->OnClicked.AddDynamic(this, &UMainMenu::OpenMainMenu);

  return true;
}

void UMainMenu::QuitGameProgram()
{
  UWorld* World = GetWorld();
  if (!World) return;

  APlayerController* PlayerController = World->GetFirstPlayerController();
  if (!PlayerController) return;

  PlayerController->ConsoleCommand("quit"); 
}

void UMainMenu::OpenMainMenu()
{
  if (!MenuSwitcher) return;
  if (!MainMenu) return;
  MenuSwitcher->SetActiveWidget(MainMenu);
}

void UMainMenu::OpenHostMenu()
{
  if (!MenuSwitcher) return;
  if (!HostMenu) return;
  MenuSwitcher->SetActiveWidget(HostMenu);
}

void UMainMenu::HostServer()
{
  if (!MenuInterface) return;
  if (!ServerHostName) return;

  MenuInterface->Host(ServerHostName->GetText().ToString());
}

void UMainMenu::OpenJoinMenu()
{
  if (!MenuSwitcher) return;
  if (!JoinMenu) return;
  MenuSwitcher->SetActiveWidget(JoinMenu);

  if (!MenuInterface) return;
  MenuInterface->RefreshServerList(); // Call FindSession
}

void UMainMenu::SetServerList(TArray<FServerData> Servers)
{
  UWorld* World = GetWorld();
  if (!World) return;

  ServerList->ClearChildren();

  uint32 i = 0;
  for (const FServerData& ServerData : Servers)
  {
    UServerRow* ServerRow = CreateWidget<UServerRow>(World, ServerRowClass);
    if (!ServerRow) return;
  
    ServerRow->ServerName->SetText(FText::FromString(ServerData.Name));
    ServerRow->HostUser->SetText(FText::FromString(ServerData.HostUsername));
    ServerRow->ConnectionFraction->SetText(FText::FromString(FString::Printf(TEXT("%d/%d"), ServerData.CurrentPlayers, ServerData.MaxPlayers)));
    ServerRow->Setup(this, i);
    ++i;
  
    ServerList->AddChild(ServerRow);
  }
}

void UMainMenu::SelectIndex(uint32 Index)
{
  SelectedIndex = Index;
  UpdateChildren();
}

void UMainMenu::UpdateChildren()
{
  for (int32 i = 0; i < ServerList->GetChildrenCount(); ++i)
  {
    UServerRow* Row = Cast<UServerRow>(ServerList->GetChildAt(i));
    if (!Row) continue;

    Row->Selected = (SelectedIndex.IsSet() && SelectedIndex.GetValue() == i);
  }
}

void UMainMenu::JoinServer()
{
  if (SelectedIndex.IsSet() && MenuInterface)
  {
    UE_LOG(LogTemp, Display, TEXT("Selected index is %d"), SelectedIndex.GetValue());
    MenuInterface->Join(SelectedIndex.GetValue());
  }
  else
  {
    UE_LOG(LogTemp, Display, TEXT("No selected index"));
  }
}

