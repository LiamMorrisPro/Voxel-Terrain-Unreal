// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Enums.h"

#include "ChunkWorld.generated.h"


class AChunkBase;

UCLASS()
class AChunkWorld : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	

	UPROPERTY(EditAnywhere, Category = "Chunk World")
	TSubclassOf<AChunkBase> ChunkType;

	UPROPERTY(EditAnywhere, Category = "Chunk World")
	int DrawDistance = 5;

	UPROPERTY(EditAnywhere, Category = "Chunk World")
	TObjectPtr<UMaterialInterface> Material;

	UPROPERTY(EditAnywhere, Category = "Chunk World")
	int ChunkSize = 32;

	UPROPERTY(EditAnywhere, Category = "Chunk World")
	EGenerationType GenerationType;

	UPROPERTY(EditAnywhere, Category = "Chunk World")
	float Frequency = 0.03f;


	AChunkWorld();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	int ChunkCount;

	void Generate3DWorld();
	void Generate2DWorld();

};
