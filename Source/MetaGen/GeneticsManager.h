#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "GroomComponent.h"
#include "GroomAsset.h"
#include "MuCO/CustomizableObjectInstance.h"
#include "Materials/MaterialInterface.h"
#include "GeneticsManager.generated.h"

UENUM(BlueprintType)
enum class EChildGender : uint8
{
	Female = 0 UMETA(DisplayName = "Female"),
	Male = 1 UMETA(DisplayName = "Male")
};

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
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DominanceIndex;

	FHairTextureGeneticData()
	{
		DominanceIndex = 2;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class METAGEN_API UGeneticsManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGeneticsManager();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genetics")
	UCustomizableObjectInstance* MetaHumanInstance;

	// -- Genetic data tables --
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Genetics")
	UDataTable* EyeGeneticsTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetics")
	UDataTable* SkinGeneticsTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetics")
	UDataTable* HairColorGeneticsTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Genetics")
	UDataTable* HairTextureGeneticsTable;

	// -- Calculation methods --
	UFUNCTION(BlueprintCallable, Category = "Genetics")
	FName CalculateEyeColor(FName FatherPhenotype, FName MotherPhenotype);

	UFUNCTION(BlueprintCallable, Category = "Genetics")
	float CalculateSkinTone(FName FatherPhenotype, FName MotherPhenotype);

	UFUNCTION(BlueprintCallable, Category = "Genetics")
	int32 CalculateHairTexture(FName FatherPhenotype, FName MotherPhenotype);

	UFUNCTION(BlueprintCallable, Category = "Genetics")
	FName CalculateHairColor(FName FatherPhenotype, FName MotherPhenotype);

	// -- Aplication methods --
	UFUNCTION(BlueprintCallable, Category="Genetics")
	void ApplyEyeColorGenetics(FName ChildEyeColorID); 

	UFUNCTION(BlueprintCallable, Category = "Genetics")
	void ApplySkinToneGenetics(float ChildSkinToneID);

	UFUNCTION(BlueprintCallable, Category = "Genetics")
	void ApplyHairTextureGenetics(int32 ChildHairTextureID);

	UFUNCTION(BlueprintCallable, Category = "Genetics")
	void ApplyHairColorGenetics(FName ChildHairColorID);


	UFUNCTION(BlueprintCallable, Category = "Genetics")
	void SetChildGender(EChildGender NewGender);


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Genetics")
	bool bNeedsMutableUpdate = false;

private:
	UFUNCTION(BlueprintPure, Category = "Genetics Logic")
	FName SelectDominantPhenotype(FName PhenotypeA, int32 DominanceA, FName PhenotypeB, int32 DominanceB) const;
};
