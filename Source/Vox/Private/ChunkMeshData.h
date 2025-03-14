// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ChunkMeshData.generated.h"

USTRUCT()
struct FChunkMeshData
{
	GENERATED_BODY()

public:
	TArray<FVector> Vertices;
	TArray<int> Triangles;
	TArray<FVector> Normals;
	TArray<FVector2D> UV0;

	void Clear();
};

inline void FChunkMeshData::Clear()
{
	Vertices.Empty();
	Triangles.Empty();
	Normals.Empty();
	UV0.Empty();
}