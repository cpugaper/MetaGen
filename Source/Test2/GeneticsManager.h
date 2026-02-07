// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "MuCO/CustomizableObjectInstance.h"
#include "GeneticsManager.generated.h"

USTRUCT(BlueprintType)
struct FEyeGeneticData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Visual representation of the phenotype
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSoftObjectPtr<UMaterialInstance> Material;

	// Dominance of the allele: 0 = recessive (Blue/Green), 10 = dominant (Brown)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DominanceIndex;

	// Default constructor
	FEyeGeneticData()
	{
		DominanceIndex = 0;
		Material = TSoftObjectPtr<UMaterialInstance>(FSoftObjectPath(TEXT("/Game/MetaHumans/Fathers/Dax/MID_MI_EyeRefractive_Inst_L_0.MID_MI_EyeRefractive_Inst_L_0")));
	}
};

USTRUCT(BlueprintType)
struct FSkinGeneticData : public FTableRowBase
{
	GENERATED_BODY()

public:
	// Exact value in the gradient texture, where 0 is the lightest skin tone and 1 is the darkest
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MelaninIndex;

	// Default constructor
	FSkinGeneticData()
	{
		MelaninIndex = 0.5f;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genetics")
	UDataTable* EyeGeneticsTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetics")
	UDataTable* SkinGeneticsTable;

	UFUNCTION(BlueprintCallable, Category="Genetics")
	void ApplyEyeColorGenetics(FName FatherPhenotype, FName MotherPhenotype); 

	UFUNCTION(BlueprintCallable, Category = "Genetics")
	void ApplySkinToneGenetics(FName FatherPhenotype, FName MotherPhenotype);

private:
	FName DetermineEyeColor(int32 DomA, FName NameA, int32 DomB, FName NameB);
};
