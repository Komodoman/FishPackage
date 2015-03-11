// Fill out your copyright notice in the Description page of Project Settings.

#include "OceanDemo.h"
#include "FlockFish.h"
#include "FishState.h"


/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish Seek State             /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////

void SeekState::Update(float delta)
{	
	Fish->isFleeing = false;
	Fish->fleeTarget = NULL;
	Fish->preyTarget = NULL;

	if (Fish->isLeader)
	{
		// Seek target
		SeekTarget(delta);
	}
	else
	{
		// School with buddies
		Flock(delta);
	}
}

void SeekState::SeekTarget(float delta)
{	
	// Set Speed
	Fish->curSpeed = FMath::Lerp(Fish->curSpeed, Fish->speed, delta);

	// Set Rotation 
	FRotator leaderRotation = FRotationMatrix::MakeFromX(Fish->getSeekTarget() - Fish->GetActorLocation()).Rotator();
	leaderRotation = FMath::RInterpTo(Fish->GetActorRotation(), leaderRotation, delta, Fish->turnSpeed);
	Fish->setRotation(leaderRotation);

	// Set Velocity Vector
	FVector leaderVelocity = Fish->GetActorForwardVector() * (delta * Fish->speed);
	Fish->setVelocity(leaderVelocity);
}

void SeekState::Flock(float delta)
{
	// Get a list of Fish neighbors and calculate seperation
	TArray<AActor*> neighborList = Fish->neighbors;
	FVector seperation = FVector(0, 0, 0);
	int neighborCount = 0;
	for (int i = 0; i < neighborList.Num(); i++)
	{
		float dist = neighborList[i]->GetDistanceTo(Fish);
		if (dist < Fish->neighborSeperation)
		{
			seperation += neighborList[i]->GetActorLocation() - Fish->GetActorLocation();
			neighborCount++;
		}
	}
	seperation = ((seperation / neighborCount) * -1);
	seperation.Normalize();
	seperation *= Fish->neighborSeperation;

	// Maintain distance behind Leader
	FVector distBehind = (Cast<AFlockFish>(Fish->leader)->getVelocity() * -1);
	distBehind.Normalize();
	distBehind *= Fish->followDist;

	// Calculate all seperation and distance behind leader into one vector
	FVector leaderLocation = Fish->leader->GetActorLocation();
	FVector flockerVelocity = distBehind + leaderLocation + seperation;
	FRotator flockerRotation = FRotationMatrix::MakeFromX(flockerVelocity - Fish->GetActorLocation()).Rotator();

	// If fish is too far behind leader, speed up 
	float newSpeed = Fish->speed;
	if (Fish->GetDistanceTo(Fish->leader) > Fish->distBehindSpeedUpRange)
	{
		// Set Speed
		Fish->curSpeed = FMath::Lerp(Fish->curSpeed, Fish->maxSpeed, delta);
	}
	else
	{
		// Set Speed
		Fish->curSpeed = FMath::Lerp(Fish->curSpeed, Fish->speed, delta);
	}


	// Set Velocity
	FVector flockVelocity = Fish->GetActorForwardVector() * (delta * Fish->curSpeed);
	Fish->setVelocity(flockVelocity);

	// Set Rotation
	FRotator flockRotation = FMath::RInterpTo(Fish->GetActorRotation(), flockerRotation, delta, Fish->turnSpeed);
	Fish->setRotation(flockRotation);
}


/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish Flee State             /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////

void FleeState::Update(float delta)
{
	Fish->isFleeing = true;
	Fish->fleeTarget = Enemy;
	Fish->preyTarget = NULL;
	if (Fish->GetDistanceTo(Enemy) >= Fish->fleeDistance)
	{
		Fish->setState(new SeekState(Fish));
	}
	FleeFromEnemy(delta);
}

void FleeState::FleeFromEnemy(float delta)
{
	// Set Speed
	Fish->curSpeed = FMath::Lerp(Fish->curSpeed, Fish->maxSpeed, (delta * 2));
	
	// Set Velocity
	FVector fleeVelocity = Fish->GetActorForwardVector() * (delta * Fish->curSpeed);
	Fish->setVelocity(fleeVelocity);

	// Set Rotation
	FRotator fleeRotation = FRotationMatrix::MakeFromX(Fish->GetActorLocation() - Enemy->GetActorLocation()).Rotator();
	fleeRotation = FMath::RInterpTo(Fish->GetActorRotation(), fleeRotation, delta, Fish->turnSpeed);
	Fish->setRotation(fleeRotation);

}

/////////////////////////////////////////////////////////////////////////////
//////////////                                        //////////////////////            
//////////////            Fish Chase State            /////////////////////                                 
//////////////                                        ////////////////////              
/////////////////////////////////////////////////////////////////////////

void ChaseState::Update(float delta)
{
	Fish->isFleeing = false;
	Fish->fleeTarget = NULL;
	Fish->preyTarget = Prey;
	EatPrey();
	ChasePrey(delta);
}

void ChaseState::EatPrey()
{
	if (Fish->GetDistanceTo(Prey) < 1000)
	{
		float zLoc = Fish->minZ + FMath::Abs(0.25 * Fish->maxZ);
		Prey->SetActorLocation(FVector(FMath::FRandRange(Fish->minX, Fish->maxX), FMath::FRandRange(Fish->minY, Fish->maxX), zLoc));
		Fish->isFull = true;
		Fish->setState(new SeekState(Fish));
	}
}

void ChaseState::ChasePrey(float delta)
{
	// Set Speed
	Fish->curSpeed = FMath::Lerp(Fish->curSpeed, Fish->maxSpeed, (delta * 2));

	// Set Velocity
	FVector chaseVelocity = Fish->GetActorForwardVector() * (delta * Fish->curSpeed);
	Fish->setVelocity(chaseVelocity);

	// Set Rotation
	TArray<AActor*> neighborList = Fish->neighbors;
	FVector seperation = FVector(0, 0, 0);
	int neighborCount = 0;
	for (int i = 0; i < neighborList.Num(); i++)
	{
		float dist = neighborList[i]->GetDistanceTo(Fish);
		if (dist < Fish->neighborSeperation)
		{
			seperation += neighborList[i]->GetActorLocation() - Fish->GetActorLocation();
			neighborCount++;
		}
	}
	seperation = ((seperation / neighborCount) * -1);
	seperation.Normalize();
	seperation *= Fish->neighborSeperation;

	FVector preyLocation = Prey->GetActorLocation();
	FVector flockerVelocity = preyLocation + seperation;
	FRotator flockerRotation = FRotationMatrix::MakeFromX(flockerVelocity - Fish->GetActorLocation()).Rotator();
	FRotator chaseRotation = FMath::RInterpTo(Fish->GetActorRotation(), flockerRotation, delta, Fish->turnSpeed);
	Fish->setRotation(chaseRotation);
}
