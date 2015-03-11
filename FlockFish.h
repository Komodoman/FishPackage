// Kyle Bryant @Komdoman
// Use wherever your heart desires <3

#pragma once

#include "GameFramework/Pawn.h"
#include "FishManager.h"
#include "FishState.h"
#include "FlockFish.generated.h"
/**
 * 
 */
UCLASS()
class OCEANDEMO_API AFlockFish : public APawn
{
	GENERATED_BODY()

	// Current state of the Fish
	FishState* currentState;

	/** Static mesh component */
	class UStaticMeshComponent* base;

	/** Fish interaction sphere */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Interaction, meta = (AllowPrivateAccess = "true"))
	class USphereComponent* FishInteractionSphere;

public:
	// Is this fish a leader
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	bool isLeader = false;

	// Enemies that fish will flee from
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	TArray<UClass*> enemyTypes;

	// Prey that the fish will attack
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	TArray<UClass*> preyTypes;

	// Neighbor type of the fish ( typically itself, but needed for code atm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	UClass* neighborType;

	// Player the fish will avoid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Variables")
	UClass* playerType;

	// volume that is considered underwater
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Variables")
	FVector underwaterMin = FVector(-40000, -40000, -9000);

	// volume that is considered underwater
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Variables")
	FVector underwaterMax = FVector(40000, 40000, -950);

	// Weight to multiply with cohesion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float neighborSeperation = 300.0;

	// Distance to that fish will try to remain behind leader
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float followDist = 50;

	// Defualt Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float speed = 1200.0;

	// Max Speed
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float maxSpeed = 2400.0;

	// Speed at which the fish turns (Try to have predators be slightly faster)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float turnSpeed = 3.0f;

	// Frequency of turning ( turn every "turnFrequency" seconds)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float turnFrequency = 1.0;

	// time after eating until it is hungry again
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float hungerResetTime = 20.0f;

	// Go faster when fish is this distance behind leader
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fish Attributes")
	float distBehindSpeedUpRange = 3000;

	// When set to 0, update every tick, otherwise update after specified time
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Environment Variables")
	float UpdateEveryTick = 0;

	// pursueDistance
	// NOTE: Unused at the moment.
	float pursueDist = 6000;

	// Distance to realize fish is near bounds
	// NOTE: Unused at the moment
	float obstacleDist = 300.0f;

	// Flee distance
	// NOTE: CURRENTLY SET IN CODE
	float fleeDistance = 1500.0;

	// current speed
	float curSpeed = speed;

	// This fish's leader
	AActor *leader;

	// current prey in world
	TArray<AActor*> prey;

	// Enemies that fish will flee from
	TArray<AActor*> enemies;

	// These are the fish's flocking buddies
	TArray<AActor*> neighbors;

	// Nearby Enemies
	TArray<AActor*> nearbyEnemies;

	// Nearby Prey
	TArray<AActor*> nearbyPrey;

	// Flee target
	AActor *fleeTarget;

	// current prey
	AActor *preyTarget;

	// is fish currently fleeing
	// Current state of the Fish
	bool isFleeing = false;

	// is the fish currently full?
	bool isFull = false;

	/* bounds of underwater box*/
	float underwaterBoxLength = 10000;

	// Max bounds of box
	float maxX;
	float maxY;
	float maxZ;
	float minX;
	float minY;
	float minZ;

	/** Constructor */
	AFlockFish(const FObjectInitializer& ObjectInitializer);

	// Getters and Setters
	FVector getSeekTarget()
	{
		return target;
	}

	void setState(FishState* newState)
	{
		currentState = newState;
	}

	void setRotation(FRotator newRotation)
	{
		curRotation = newRotation;
	};

	void setVelocity(FVector newVelocity)
	{
		curVelocity = newVelocity;
	};

	FRotator getRotation()
	{
		return curRotation;
	};

	FVector getVelocity()
	{
		return curVelocity;
	};

protected:
	/** Overrided tick function */
	virtual void Tick(float delta) override;

	/* setup the game on first tick */
	void Setup();

	/* Manage the various necessary timers*/
	void ManageTimers(float delta);

	/* Choose what state to be in*/
	void ChooseState();

	/* Move Bounds */
	void MoveBounds();

	UFUNCTION()
	void OnBeginOverlap(AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex, bool bFromSweep, const FHitResult& sweepResult);

	UFUNCTION()
	void OnEndOverlap(AActor* otherActor, UPrimitiveComponent* otherComp, int32 otherBodyIndex);

	/* update the current state*/
	void UpdateState(float delta);

	/* places the target randomly within the postprocessvolume*/
	void spawnTarget();

	// current velocity
	FVector curVelocity = FVector(0, 0, 0);

	// current rotation
	FRotator curRotation = FRotator(0, 0, 0);

	// turning timer
	float turnTimer = 0.0;

	// Are the array's setup?
	bool isSetup = false;

	// This is the target the fish will path to
	FVector target;

	// current hunger timer
	float hungerTimer = 0.0f;

	// Tick Timer
	float updateTimer = 0.0f;

	// player holder
	AActor* player;

	// has fish manager?
	bool hasFishManager = false;

	// fish manager
	AActor* fishManager;


};
