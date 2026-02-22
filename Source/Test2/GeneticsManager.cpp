#include "GeneticsManager.h"

UGeneticsManager::UGeneticsManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGeneticsManager::BeginPlay()
{
	Super::BeginPlay();
}

void UGeneticsManager::ApplyEyeColorGenetics(FName FatherEyeColorID, FName MotherEyeColorID)
{
	if (!MetaHumanInstance || !EyeGeneticsTable) return;

	// Retrieve genetic data for both parents
	FEyeGeneticData* FatherData = EyeGeneticsTable->FindRow<FEyeGeneticData>(FatherEyeColorID, TEXT("Genetics Eye Context"));
	FEyeGeneticData* MotherData = EyeGeneticsTable->FindRow<FEyeGeneticData>(MotherEyeColorID, TEXT("Genetics Eye Context"));

	if (FatherData && MotherData)
	{
		// Determine child's eye color based on parents' dominance
		FName ChildEyeColor = SelectDominantPhenotype(FatherEyeColorID, FatherData->DominanceIndex, MotherEyeColorID, MotherData->DominanceIndex);
		FString NewEyeColor = ChildEyeColor.ToString();

		if (NewEyeColor == LastAppliedEyeColor) return;
		LastAppliedEyeColor = NewEyeColor;
		bNeedsMutableUpdate = true; 

		// Apply the determined eye color to the MetaHuman instance
		MetaHumanInstance->SetIntParameterSelectedOption(FString("EyeColor"), FString(ChildEyeColor.ToString()));
		UE_LOG(LogTemp, Warning, TEXT("Eye color applied: [%s]"), *ChildEyeColor.ToString());
	}
}

void UGeneticsManager::ApplySkinToneGenetics(FName FatherSkinID, FName MotherSkinID)
{
	if (!MetaHumanInstance || !SkinGeneticsTable) return;

	// Retrieve genetic data for both parents
	FSkinGeneticData* FatherData = SkinGeneticsTable->FindRow<FSkinGeneticData>(FatherSkinID, TEXT("Genetics Skin Context"));
	FSkinGeneticData* MotherData = SkinGeneticsTable->FindRow<FSkinGeneticData>(MotherSkinID, TEXT("Genetics Skin Context"));

	
	if (FatherData && MotherData) {
		// Determine skin tone range
		float MinMelanin = FMath::Min(FatherData->MelaninIndex, MotherData->MelaninIndex);
		float MaxMelanin = FMath::Max(FatherData->MelaninIndex, MotherData->MelaninIndex);

		// Favor darker skin tones
		const float DarknessBias = 0.4f; 
		float RandomValue = FMath::Pow(FMath::FRand(), DarknessBias);

		float ChildMelaninIndex = FMath::Lerp(MinMelanin, MaxMelanin, RandomValue);

		DebugMinSkin = MinMelanin;
		DebugMaxSkin = MaxMelanin;
		DebugChildSkin = ChildMelaninIndex;

		MetaHumanInstance->SetFloatParameterSelectedOption(FString("SkinTone"), ChildMelaninIndex);
	}
}

void UGeneticsManager::ApplyHairTextureGenetics(FName FatherHairTexID, FName MotherHairTexID)
{
	if (!MetaHumanInstance || !HairTextureGeneticsTable) return;

	FHairTextureGeneticData* FatherData = HairTextureGeneticsTable->FindRow<FHairTextureGeneticData>(FatherHairTexID, TEXT("Hair Texture Context"));
	FHairTextureGeneticData* MotherData = HairTextureGeneticsTable->FindRow<FHairTextureGeneticData>(MotherHairTexID, TEXT("Hair Texture Context"));

	if (FatherData && MotherData)
	{
		// Determine hair texture range
		float MinType = FMath::Min(FatherData->DominanceIndex, MotherData->DominanceIndex);
		float MaxType = FMath::Max(FatherData->DominanceIndex, MotherData->DominanceIndex);

		// Favor curlier textures
		const float CurlBias = 0.5f;
		float RandomValue = FMath::Pow(FMath::FRand(), CurlBias);

		float RawResult = FMath::Lerp(MinType, MaxType, RandomValue);
		int32 ChildTexture = FMath::RoundToInt(RawResult);

		FString ResultTextureType;
		switch (ChildTexture)
		{
			case 0: ResultTextureType = "Straight"; break;
			case 1: ResultTextureType = "Wavy"; break;
			case 2: ResultTextureType = "Curly"; break;
			default: ResultTextureType = "Wavy"; break; 
		}

		if (ResultTextureType == LastAppliedHairTexture) return;
		LastAppliedHairTexture = ResultTextureType;
		bNeedsMutableUpdate = true;

		MetaHumanInstance->SetIntParameterSelectedOption("Hair", ResultTextureType);
		UE_LOG(LogTemp, Warning, TEXT("Hair texture applied: [%s]"), *ResultTextureType);
	}
}

void UGeneticsManager::CalculateHairColor(FName FatherHairColorID, FName MotherHairColorID)
{
	if (!MetaHumanInstance || !HairColorGeneticsTable) return;

	FHairColorGeneticData* FatherData = HairColorGeneticsTable->FindRow<FHairColorGeneticData>(FatherHairColorID, TEXT("Hair Color Context"));
	FHairColorGeneticData* MotherData = HairColorGeneticsTable->FindRow<FHairColorGeneticData>(MotherHairColorID, TEXT("Hair Color Context"));

	if (FatherData && MotherData)
	{
		CurrentWinnerHairID = SelectDominantPhenotype(FatherHairColorID, FatherData->DominanceIndex, MotherHairColorID, MotherData->DominanceIndex);

		FHairColorGeneticData* WinnerData = HairColorGeneticsTable->FindRow<FHairColorGeneticData>(CurrentWinnerHairID, TEXT("Hair Color Context"));
		if (WinnerData)
		{
			CachedHairMaterial = WinnerData->HairMaterial.LoadSynchronous();
		}
	}
}

void UGeneticsManager::ApplyHairColorGenetics()
{
	if (!MetaHumanInstance || !HairColorGeneticsTable) return;
	if (!CachedHairMaterial) return;

	AActor* OwnerActor = GetOwner();
	if (OwnerActor)
	{
		TArray<UGroomComponent*> GroomComponents;
		OwnerActor->GetComponents<UGroomComponent>(GroomComponents);

		for (UGroomComponent* Groom : GroomComponents)
		{
			if (Groom->GroomAsset)
			{
				FString AssetName = Groom->GroomAsset->GetName();

				if (!AssetName.Contains("Eyelash") && !AssetName.Contains("Fuzz"))
				{
					int32 TargetSlot = 0;
					if (AssetName.Contains("Coil")) TargetSlot = 1; // In curly hair assets material is in slot 1 not 0

					if (Groom->GetMaterial(TargetSlot) != CachedHairMaterial)
					{
						Groom->SetMaterial(TargetSlot, CachedHairMaterial);
						Groom->MarkRenderStateDirty();
						UE_LOG(LogTemp, Warning, TEXT("Material updated in [%s]"), *AssetName);
					}
				}
			}
		}
	}
}

FName UGeneticsManager::SelectDominantPhenotype(FName PhenotypeA, int32 DominanceA, FName PhenotypeB, int32 DominanceB) const
{
	float ChanceForA = 0.5f;

	// Mendelian inheritance: the more dominant trait has a higher chance (3/4) to be expressed in the child
	if (DominanceA > DominanceB)
	{
		ChanceForA = 0.75f;
	}
	else if (DominanceB > DominanceA)
	{
		ChanceForA = 0.25f;
	}

	return (FMath::FRand() <= ChanceForA) ? PhenotypeA : PhenotypeB;
}


