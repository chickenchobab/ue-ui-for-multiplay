// Fill out your copyright notice in the Description page of Project Settings.


#include "ServerRow.h"
#include "Components/Button.h"
#include "MainMenu.h"

void UServerRow::Setup(UMainMenu* Parent, uint32 Index)
{
  MainMenu = Parent;
  RowIndex = Index;
  RowButton->OnClicked.AddDynamic(this, &UServerRow::OnClicked);
}

void UServerRow::OnClicked()
{
  if (!MainMenu) return;

  MainMenu->SelectIndex(RowIndex);
}

