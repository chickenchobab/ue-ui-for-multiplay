// Fill out your copyright notice in the Description page of Project Settings.


#include "PuzzlePlatformsGameInstance.h"

#include "Engine/Engine.h"
#include "UObject/ConstructorHelpers.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

#include "PlatformTrigger.h"
#include "MenuSystem/MainMenu.h"
#include "MenuSystem/InGameMenu.h"

const static FName SESSION_NAME = TEXT("My Session Game");
const static FName KEY_FOR_CUSTOM_SERVER_NAME = TEXT("Server Name");



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
  IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
  if (Subsystem)
  {
    UE_LOG(LogTemp, Warning, TEXT("Found subsystem %s"), *Subsystem->GetSubsystemName().ToString());
    SessionInterface = Subsystem->GetSessionInterface();
    if (SessionInterface.IsValid()) 
    {
      // Bind callback functions to delegates.
      SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnCreateSessionComplete);
      SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnDestroySessionComplete);
      SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnFindSessionsComplete);
      SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UPuzzlePlatformsGameInstance::OnJoinSessionComplete);
    }
  }
  else
  {
    UE_LOG(LogTemp, Warning, TEXT("Found no subsystem"));
  }
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

  UInGameMenu* InGameMenu = CreateWidget<UInGameMenu>(this, InGameMenuClass);
  if (!InGameMenu) return;
  
  InGameMenu->SetUp();
  InGameMenu->SetMenuInterface(this);
}

void UPuzzlePlatformsGameInstance::ReturnToMainMenu()
{
  APlayerController* PlayerController = GetFirstLocalPlayerController();
  if (!PlayerController) return;

  PlayerController->ClientTravel("/Game/MenuSystem/MainMenu", ETravelType::TRAVEL_Absolute);
}



void UPuzzlePlatformsGameInstance::OnCreateSessionComplete(FName SessionName, bool Success)
{
  if (!Success)
  {
    UE_LOG(LogTemp, Warning, TEXT("Could not create session"));
    return;
  }
  if (MainMenu) MainMenu->ShutDown();

  UEngine* Engine = GetEngine();
  if (!Engine) return;

  Engine->AddOnScreenDebugMessage(-1, 2, FColor::Green, TEXT("Hosting"));

  UWorld* World = GetWorld();
  if (!World) return;

  World->ServerTravel("/Game/ThirdPerson/Maps/ThirdPersonMap?listen");
}

void UPuzzlePlatformsGameInstance::OnDestroySessionComplete(FName SessionName, bool Success)
{
  if (Success)
  {
    CreateSession();
  }  
}

void UPuzzlePlatformsGameInstance::OnFindSessionsComplete(bool Success)
{
  if (!(Success && SessionSearch.IsValid() && MainMenu)) return;
  
  UE_LOG(LogTemp, Display, TEXT("Found Session"));
  TArray<FServerData> Servers;
  for (const FOnlineSessionSearchResult& SearchResult : SessionSearch->SearchResults)
  {
    UE_LOG(LogTemp, Display, TEXT("Session ID : %s"), *SearchResult.GetSessionIdStr());
    FServerData Data;
    Data.Name = SearchResult.GetSessionIdStr();
    Data.MaxPlayers = SearchResult.Session.SessionSettings.NumPublicConnections;
    Data.CurrentPlayers = Data.MaxPlayers - SearchResult.Session.NumOpenPublicConnections;
    Data.HostUsername = SearchResult.Session.OwningUserName;

    FString ServerName;
    if (SearchResult.Session.SessionSettings.Get(KEY_FOR_CUSTOM_SERVER_NAME, ServerName))
    {
      Data.Name = ServerName;
    }
    Servers.Add(Data);
  }
  MainMenu->SetServerList(Servers);
}

void UPuzzlePlatformsGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
  if (!SessionInterface.IsValid()) return;

  FString Address;
  if (!SessionInterface->GetResolvedConnectString(SessionName, Address))
  {
    UE_LOG(LogTemp, Display, TEXT("Could not get connect string."));
    return;
  }

  UEngine* Engine = GetEngine();
  if (!Engine) return;

  Engine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString::Printf(TEXT("Joining to %s"), *Address));

  APlayerController* PlayerController = GetFirstLocalPlayerController();
  if (!PlayerController) return;

  PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
}



void UPuzzlePlatformsGameInstance::Host(FString ServerName)
{
  CustomServerName = ServerName;

  if (SessionInterface.IsValid()) 
  {
    if (SessionInterface->GetNamedSession(SESSION_NAME))
    {
      SessionInterface->DestroySession(SESSION_NAME);
    }
    else
    {
      CreateSession();
    }
  }
}

void UPuzzlePlatformsGameInstance::CreateSession()
{
  if (!SessionInterface.IsValid()) return;

  FOnlineSessionSettings SessionSettings;
  if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
  {
    SessionSettings.bIsLANMatch = true;
  }
  else
  {
    SessionSettings.bIsLANMatch = false;
  }

  SessionSettings.NumPublicConnections = 2;
  SessionSettings.bShouldAdvertise = true;
  SessionSettings.bUsesPresence = true;
  SessionSettings.bUseLobbiesIfAvailable = true;
  if (!CustomServerName.IsEmpty())
  {
    SessionSettings.Set(KEY_FOR_CUSTOM_SERVER_NAME, CustomServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
  }

  SessionInterface->CreateSession(0, SESSION_NAME, SessionSettings);
}



void UPuzzlePlatformsGameInstance::RefreshServerList()
{
  if (!SessionInterface.IsValid()) return;

  SessionSearch = MakeShareable(new FOnlineSessionSearch());
  if (!SessionSearch.IsValid()) return;

  SessionSearch->bIsLanQuery = true;
  SessionSearch->MaxSearchResults = 100;
  SessionSearch->QuerySettings.Set(FName("bUsesPresence"), true, EOnlineComparisonOp::Equals);
  UE_LOG(LogTemp, Display, TEXT("Start finding session"));
  SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UPuzzlePlatformsGameInstance::Join(uint32 Index)
{
  if (!SessionInterface) return;
  if (!SessionSearch.IsValid()) return;

  if (MainMenu) MainMenu->ShutDown();

  SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}