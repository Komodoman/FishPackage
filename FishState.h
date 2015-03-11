// Fill out your copyright notice in the Description page of Project Settings.
#include "FlockFish.h"
#include "OceanDemo.h"

#pragma once

class AFlockFish;

/**
 * 
 */
class OCEANDEMO_API FishState
{
protected:
	AFlockFish* Fish;

public:
	// Constructors
	FishState(){}
	FishState(AFlockFish* aFish)
	{
		Fish = aFish;
	};
	virtual void Update(float val){};
	virtual void HandleOverlapEvent(AActor *otherActor, UPrimitiveComponent *otherComponent, FString aColliderString){};
};

class OCEANDEMO_API SeekState : public FishState
{
public:

	SeekState(AFlockFish* aFish) : FishState(aFish){};
	virtual void Update(float delta) override;

protected:

	virtual void SeekTarget(float delta);
	virtual void Flock(float delta);
};

class OCEANDEMO_API FleeState : public FishState
{
protected:
	AActor* Enemy;

public:

	FleeState(AFlockFish* aFish, AActor* aEnemy) : FishState(aFish)
	{
		Enemy = aEnemy;
	};
	virtual void Update(float delta) override;

protected:

	virtual void FleeFromEnemy(float delta);

};

class OCEANDEMO_API ChaseState : public FishState
{
protected:
	AActor* Prey;

public:

	ChaseState(AFlockFish* aFish, AActor* aPrey) : FishState(aFish)
	{
		Prey = aPrey;
	};
	virtual void Update(float delta) override;

protected:

	virtual void ChasePrey(float delta);
	virtual void EatPrey();
};
