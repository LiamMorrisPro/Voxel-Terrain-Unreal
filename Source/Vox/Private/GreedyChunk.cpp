// Fill out your copyright notice in the Description page of Project Settings.


#include "GreedyChunk.h"

#include "FastNoiseLite.h"



void AGreedyChunk::Setup()
{
	// Initialize Blocks
	Blocks.SetNum(Size * Size * Size);
}

void AGreedyChunk::Generate2DHeightMap(const FVector Position)
{
	for (int x = 0; x < Size; x++)
	{
		for (int y = 0; y < Size; y++)
		{
			const float Xpos = x + Position.X;
			const float Ypos = y + Position.Y;

			const int Height = FMath::Clamp(FMath::RoundToInt((Noise->GetNoise(Xpos, Ypos) + 1) * Size / 2), 0, Size);

			for (int z = 0; z < Size; z++)
			{
				if (z < Height - 3) Blocks[GetBlockIndex(x, y, z)] = EBlock::Stone;
				else if (z < Height - 1) Blocks[GetBlockIndex(x, y, z)] = EBlock::Dirt;
				else if (z == Height - 1) Blocks[GetBlockIndex(x, y, z)] = EBlock::Grass;
				else Blocks[GetBlockIndex(x, y, z)] = EBlock::Air;
			}
		}
	}
}

void AGreedyChunk::Generate3DHeightMap(const FVector Position)
{
	for (int x = 0; x < Size; ++x)
	{
		for (int y = 0; y < Size; ++y)
		{
			for (int z = 0; z < Size; ++z)
			{
				const auto NoiseValue = Noise->GetNoise(x + Position.X, y + Position.Y, z + Position.Z);

				if (NoiseValue >= 0)
				{
					Blocks[GetBlockIndex(x, y, z)] = EBlock::Air;
				}
				else
				{
					Blocks[GetBlockIndex(x, y, z)] = EBlock::Stone;
				}
			}
		}
	}
}


void AGreedyChunk::GenerateMesh()
{
	// Sweep over each axis (X, Y, Z)
	for (int Axis = 0; Axis < 3; ++Axis)
	{
		// 2 Perpendicular axis
		const int Axis1 = (Axis + 1) % 3;
		const int Axis2 = (Axis + 2) % 3;

		const int MainAxisLimit = Size;
		const int Axis1Limit = Size;
		const int Axis2Limit = Size;

		auto DeltaAxis1 = FIntVector::ZeroValue;
		auto DeltaAxis2 = FIntVector::ZeroValue;

		auto ChunkItr = FIntVector::ZeroValue;
		auto AxisMask = FIntVector::ZeroValue;

		AxisMask[Axis] = 1;

		TArray<FMask> Mask;
		Mask.SetNum(Axis1Limit * Axis2Limit);

		// Check each slice of the chunk
		for (ChunkItr[Axis] = -1; ChunkItr[Axis] < MainAxisLimit;)
		{
			int N = 0;

			// Compute Mask
			for (ChunkItr[Axis2] = 0; ChunkItr[Axis2] < Axis2Limit; ++ChunkItr[Axis2])
			{
				for (ChunkItr[Axis1] = 0; ChunkItr[Axis1] < Axis1Limit; ++ChunkItr[Axis1])
				{
					const auto CurrentBlock = GetBlock(ChunkItr);
					const auto CompareBlock = GetBlock(ChunkItr + AxisMask);

					const bool CurrentBlockOpaque = CurrentBlock != EBlock::Air;
					const bool CompareBlockOpaque = CompareBlock != EBlock::Air;

					if (CurrentBlockOpaque == CompareBlockOpaque)
					{
						Mask[N++] = FMask{ EBlock::Null, 0 };
					}
					else if (CurrentBlockOpaque)
					{
						Mask[N++] = FMask{ CurrentBlock, 1 };
					}
					else
					{
						Mask[N++] = FMask{ CompareBlock, -1 };
					}
				}
			}

			++ChunkItr[Axis];
			N = 0;

			// Generate Mesh From Mask
			for (int j = 0; j < Axis2Limit; ++j)
			{
				for (int i = 0; i < Axis1Limit;)
				{
					if (Mask[N].Normal != 0)
					{
						const auto CurrentMask = Mask[N];
						ChunkItr[Axis1] = i;
						ChunkItr[Axis2] = j;

						int Width;

						for (Width = 1; i + Width < Axis1Limit && CompareMask(Mask[N + Width], CurrentMask); ++Width)
						{
						}

						int Height;
						bool Done = false;

						for (Height = 1; j + Height < Axis2Limit; ++Height)
						{
							for (int k = 0; k < Width; ++k)
							{
								if (CompareMask(Mask[N + k + Height * Axis1Limit], CurrentMask)) continue;

								Done = true;
								break;
							}

							if (Done) break;
						}

						DeltaAxis1[Axis1] = Width;
						DeltaAxis2[Axis2] = Height;

						CreateQuad(
							CurrentMask, AxisMask, Width, Height,
							ChunkItr,
							ChunkItr + DeltaAxis1,
							ChunkItr + DeltaAxis2,
							ChunkItr + DeltaAxis1 + DeltaAxis2
						);

						DeltaAxis1 = FIntVector::ZeroValue;
						DeltaAxis2 = FIntVector::ZeroValue;

						for (int l = 0; l < Height; ++l)
						{
							for (int k = 0; k < Width; ++k)
							{
								Mask[N + k + l * Axis1Limit] = FMask{ EBlock::Null, 0 };
							}
						}

						i += Width;
						N += Width;
					}
					else
					{
						i++;
						N++;
					}
				}
			}
		}
	}
}

void AGreedyChunk::CreateQuad(FMask Mask, FIntVector AxisMask, const int width, const int height, FIntVector V1, FIntVector V2, FIntVector V3, FIntVector V4)
{
	const auto Normal = FVector(AxisMask * Mask.Normal);
	

	MeshData.Vertices.Append({ FVector(V1) * 100, FVector(V2) * 100, FVector(V3) * 100, FVector(V4) * 100 });

	MeshData.Triangles.Append({ VertexCount, VertexCount + 2 + Mask.Normal, VertexCount + 2 - Mask.Normal, VertexCount + 3,  VertexCount + 1 - Mask.Normal,  VertexCount + 1 + Mask.Normal});



	MeshData.Normals.Append({ Normal, Normal, Normal, Normal });

	if (Normal.X == 1 || Normal.X == -1)
	{
		MeshData.UV0.Append({
			FVector2D(width, height),
			FVector2D(0, height),
			FVector2D(width, 0),
			FVector2D(0, 0)
			});
	}
	else
	{
		MeshData.UV0.Append({
			FVector2D(height, width),
			FVector2D(height, 0),
			FVector2D(0, width),
			FVector2D(0, 0)
			});
	}



	VertexCount += 4;

}


void AGreedyChunk::ModifyVoxelData(const FIntVector Position, EBlock Block)
{
	const int Index = GetBlockIndex(Position.X, Position.Y, Position.Z);

	Blocks[Index] = Block;
}


int AGreedyChunk::GetBlockIndex(int X, int Y, int Z) const
{
	return Z * Size * Size + Y * Size + X;
}

EBlock AGreedyChunk::GetBlock(FIntVector Index) const
{
	if (Index.X >= Size || Index.Y >= Size || Index.Z >= Size || Index.X < 0 || Index.Y < 0 || Index.Z < 0)
		return EBlock::Air;
	return Blocks[GetBlockIndex(Index.X, Index.Y, Index.Z)];
}

bool AGreedyChunk::CompareMask(FMask M1, FMask M2) const
{
	return M1.Block == M2.Block && M1.Normal == M2.Normal;
}

