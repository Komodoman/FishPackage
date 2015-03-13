// Kyle Bryant @Komdoman
// Use wherever your heart desires <3

#include "OceanDemo.h"
#include "FlockFish.h"
#include "FishManager.h"


AFlockFish::AFlockFish(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	base = ObjectInitializer.CreateDefaultSubobject<UStaticMeshComponent>(this, TEXT("FishMesh"));
	RootComponent = base;

	// Fish interaction sphere
	FishInteractionSphere = ObjectInitializer.CreateDefaultSubobject<USphereComponent>(this, TEXT("FishInteractionSphere"));
	FishInteractionSphere->SetSphereRadius(10);
	FishInteractionSphere->AttachTo(RootComponent);
	FishInteractionSphere->OnComponentBeginOverlap.AddDynamic(this, &AFlockFish::OnBeginOverlap);
	FishInteractionSphere->OnComponentEndOverlap.AddDynamic(this, &AFlockFish::OnEndOverlap);

	if (isLeader == true)
	{
		spawnTarget();
	}
}

void AFlockFish::Tick(float delta)
{

	// Setup the fish (happens on first tick only)
	// NOTE TO SELF: consider creating a beginplay event that does this stuff (although beginplay is buggy as hell x.x)
	Setup();

	// Move Bounds based on location of FishManager (if applicable)
	MoveBounds();

	// Manage Timers (hungerTimer, updateTimer, and turnTimer)
	ManageTimers(delta);

	// Decide what state to be in
	ChooseState();

	// Update curVelocity and curRotation through current state
	UpdateState(delta);

	// Update world rotation and velocity
	this->SetActorRotation(curRotation);
	this->AddActorWorldOffset(curVelocity);

	Super::Tick(delta);
}

void AFlockFish::UpdateState(float delta)
{
	if (UpdateEveryTick == 0)
	{
		currentState->Update(delta);
	}
	else if (updateTimer >= UpdateEveryTick)
	{
		currentState->Update(delta);
		updateTimer = 0;
	}
}


void AFlockFish::OnBeginOverlap(AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult)
{
	// Is overlapping with enemy?
	if (enemyTypes.Find(otherActor->GetClass()) >= 0)
	{	
		nearbyEnemies.Add(otherActor);
	}
	else if (preyTypes.Find(otherActor->GetClass()) >= 0)
	{	
		if (otherActor->GetClass() == this->GetClass())
		{
			if (!Cast<AFlockFish>(otherActor)->isLeader)
			{
				nearbyPrey.Add(otherActor);
			}
		}
		else
		{
			nearbyPrey.Add(otherActor);
		}
	}
	else if (otherActor->GetClass() == this->GetClass())
	{
		nearbyFriends.Add(otherActor);
	}
}

void AFlockFish::OnEndOverlap(AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex)
{
	if (nearbyEnemies.Find(otherActor) >= 0)
	{
		nearbyEnemies.Remove(otherActor);
	}
	else if (nearbyPrey.Find(otherActor) >= 0)
	{
		nearbyPrey.Remove(otherActor);
	}
	else if (nearbyFriends.Find(otherActor) >= 0)
	{
		nearbyFriends.Remove(otherActor);
	}
}

void AFlockFish::ChooseState()
{
	if (nearbyEnemies.IsValidIndex(0))
	{
		currentState = new FleeState(this, nearbyEnemies[0]);
	}
	else if (nearbyPrey.IsValidIndex(0) && isFull == false)
	{
		currentState = new ChaseState(this, nearbyPrey[0]);
	}
	else
	{
		currentState = new SeekState(this);
	}
}

void AFlockFish::ManageTimers(float delta)
{
	// Check if the fish is full or not
	if (isFull)
	{
		hungerTimer += delta;

		if (hungerTimer >= hungerResetTime)
		{
			hungerTimer = 0.0f;
			isFull = false;
		}
	}

	// decide on wether to move target this tick
	if (turnTimer >= turnFrequency && isLeader == true)
	{
		spawnTarget();
		turnTimer = 0.0;
	}

	updateTimer += delta;
	turnTimer += delta;
}



void AFlockFish::MoveBounds()
{
	if (hasFishManager)
	{
		FVector fishManagerPosition = fishManager->GetActorLocation();
		maxX = fishManagerPosition.X + underwaterBoxLength;
		minX = fishManagerPosition.X - underwaterBoxLength;
		maxY = fishManagerPosition.Y + underwaterBoxLength;
		minY = fishManagerPosition.Y - underwaterBoxLength;
	}
}

void AFlockFish::spawnTarget()
{
	target = FVector(FMath::FRandRange(minX, maxX), FMath::FRandRange(minY, maxY), FMath::FRandRange(minZ, maxZ));
}


void AFlockFish::Setup()
{
	// Setup the enemies list on first tick
	if (isSetup == false)
	{
		maxX = underwaterMax.X;
		maxY = underwaterMax.Y;
		minX = underwaterMin.X;
		minY = underwaterMin.Y;

		if (CustomZSeekMax == NULL)
		{
			minZ = underwaterMin.Z;
			maxZ = underwaterMax.Z;
		}
		else
		{
			minZ = CustomZSeekMin;
			maxZ = CustomZSeekMax;
		}

		fleeDistance = FishInteractionSphere->GetScaledSphereRadius() * FleeDistanceMultiplier;
		neighborSeperation = FishInteractionSphere->GetScaledSphereRadius() * SeperationDistanceMultiplier;

		currentState = new SeekState(this);

		// Setup Neighbors
		TArray<AActor*> aNeighborList;
		UGameplayStatics::GetAllActorsOfClass(this, neighborType, aNeighborList);
		neighbors.Append(aNeighborList);
		for (int i = 0; i < neighbors.Num(); i++)
		{
			if (Cast<AFlockFish>(neighbors[i])->isLeader)
			{
				leader = neighbors[i];
				break;
			}
		}
		nearbyFriends.Append(neighbors);


		TArray<AActor*> aFishManagerList;
		UGameplayStatics::GetAllActorsOfClass(this, AFishManager::StaticClass(), aFishManagerList);
		if (aFishManagerList.Num() > 0)
		{
			hasFishManager = true;
			fishManager = aFishManagerList[0];
		}

		isSetup = true;
	}
}