// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "GroomComponent.h"
#include "GroomAsset.h"
#include "MuCO/CustomizableObjectInstance.h"
#include "Materials/MaterialInterface.h"
#include "GeneticsManager.generated.h"

USTRUCT(BlueprintType)
struct FEyeGeneticData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UMaterialInstance> EyeMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DominanceIndex;

	FEyeGeneticData()
	{
		DominanceIndex = 0;
		EyeMaterial = TSoftObjectPtr<UMaterialInstance>(FSoftObjectPath(TEXT("/Game/Mutable/EyeColors/MI_EyeColor_Brown.MI_EyeColor_Brown")));
	}
};

USTRUCT(BlueprintType)
struct FSkinGeneticData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 0 is the lightest skin tone and 1 is the darkest
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MelaninIndex;

	FSkinGeneticData()
	{
		MelaninIndex = 0.5f;
	}
};

USTRUCT(BlueprintType)
struct FHairColorGeneticData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UMaterialInterface> HairMaterial;

	// 0 = Red, 1 = Blonde, 2 = Brown/Black
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DominanceIndex;

	FHairColorGeneticData()
	{
		HairMaterial = TSoftObjectPtr<UMaterialInterface>(FSoftObjectPath(TEXT("/Game/MetaHumans/Children/Amelia/Materials/MI_Hair.MI_Hair")));
		DominanceIndex = 2;
	}
};

USTRUCT(BlueprintType)
struct FHairTextureGeneticData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// 0 = Straight, 1 = Wavy, 2 = Curly
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DominanceIndex;

	FHairTextureGeneticData()
	{
		DominanceIndex = 2;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEST2_API UGeneticsManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGeneticsManager();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genetics")
	UCustomizableObjectInstance* MetaHumanInstance;

	// Genetics data tables for each trait
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genetics")
	UDataTable* EyeGeneticsTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetics")
	UDataTable* SkinGeneticsTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetics")
	UDataTable* HairColorGeneticsTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetics")
	UDataTable* HairTextureGeneticsTable;

	// Debug values 
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	float DebugMinSkin = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	float DebugMaxSkin = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Debug")
	float DebugChildSkin = 0.0f;


	UFUNCTION(BlueprintCallable, Category="Genetics")
	void ApplyEyeColorGenetics(FName FatherPhenotype, FName MotherPhenotype); 

	UFUNCTION(BlueprintCallable, Category = "Genetics")
	void ApplySkinToneGenetics(FName FatherPhenotype, FName MotherPhenotype);

	UFUNCTION(BlueprintCallable, Category = "Genetics")
	void ApplyHairTextureGenetics(FName FatherPhenotype, FName MotherPhenotype);

	UFUNCTION(BlueprintCallable, Category = "Genetics")
	void ApplyHairColorGenetics();

	UFUNCTION(BlueprintCallable, Category = "Genetics")
	void CalculateHairColor(FName FatherPhenotype, FName MotherPhenotype);

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Genetics")
	bool bNeedsMutableUpdate = false;

private:
	UFUNCTION(BlueprintPure, Category = "Genetics Logic")
	FName SelectDominantPhenotype(FName PhenotypeA, int32 DominanceA, FName PhenotypeB, int32 DominanceB) const;

	FName CurrentWinnerHairID;

	// Optimization variables
	FString LastAppliedEyeColor;
	FString LastAppliedHairTexture;

	UPROPERTY()
	UMaterialInterface* CachedHairMaterial = nullptr;
};
