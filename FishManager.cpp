// Fill out your copyright notice in the Description page of Project Settings.

#include "OceanDemo.h"
#include "FishManager.h"
#include "FlockFish.h"


AFishManager::AFishManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFishManager::Tick(float val)
{
	setup();

	if (attachToPlayer)
	{
		moveToPlayer();
	}
}

void AFishManager::moveToPlayer()
{
	if (player)
		this->SetActorLocation(player->GetActorLocation());
}

float AFishManager::getMinZ()
{
	return minZ;
}

float AFishManager::getMaxZ()
{
	return maxZ;
}

void AFishManager::setup()
{
	if (isSetup == false){
		if (!areFishSpawned)
		{
			maxX = GetActorLocation().X + underwaterBoxLength;
			maxY = GetActorLocation().Y + underwaterBoxLength;
			minX = GetActorLocation().X - underwaterBoxLength;
			minY = GetActorLocation().Y - underwaterBoxLength;

			UWorld* const world = GetWorld();
			int numFlocks = flockTypes.Num();
			for (int i = 0; i < numFlocks; i++)
			{
				//FString TheFloatStr = FString::SanitizeFloat(FMath::FRandRange(minZ, maxZ));
				//GEngine->AddOnScreenDebugMessage(-1, 1.0, FColor::Red, *TheFloatStr);
				FVector spawnLoc = FVector(FMath::FRandRange(minX, maxX), FMath::FRandRange(minY, maxY), FMath::FRandRange(minZ, maxZ));
				for (int j = 0; j < numInFlock[i]; j++)
				{
					AFlockFish *aFish = Cast<AFlockFish>(world->SpawnActor(flockTypes[i]));
					aFish->isLeader = false;
					aFish->underwaterMax = FVector(maxX, maxY, maxZ);
					aFish->underwaterMin = FVector(minX, minY, minZ);
					aFish->underwaterBoxLength = underwaterBoxLength;
					if (j == 0)
					{
						aFish->isLeader = true;
					}
					aFish->SetActorLocation(spawnLoc);
				}
			}
			areFishSpawned = true;
		}

		if (attachToPlayer)
		{
			TArray<AActor*> aPlayerList;
			UGameplayStatics::GetAllActorsOfClass(this, playerType, aPlayerList);
			if (aPlayerList.Num() > 0)
			{	
				player = aPlayerList[0];
				isSetup = true;
			}
		} 
		else
		{
			isSetup = true;
		}

	}
}
