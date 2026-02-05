// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "MuCO/CustomizableObjectInstance.h"
#include "GeneticsManager.generated.h"

USTRUCT(BlueprintType)
struct FGeneticData : public FTableRowBase
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
	FGeneticData()
	{
		DominanceIndex = 0;
		Material = TSoftObjectPtr<UMaterialInstance>(FSoftObjectPath(TEXT("/Game/MetaHumans/Fathers/Dax/MID_MI_EyeRefractive_Inst_L_0.MID_MI_EyeRefractive_Inst_L_0")));
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

	UFUNCTION(BlueprintCallable, Category="Genetics")
	void ApplyEyeColorGenetics(FName FatherPhenotype, FName MotherPhenotype); 

private:
	FName DetermineEyeColor(int32 DomA, FName NameA, int32 DomB, FName NameB);
};
