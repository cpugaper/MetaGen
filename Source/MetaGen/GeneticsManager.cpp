#include "GeneticsManager.h"

UGeneticsManager::UGeneticsManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGeneticsManager::BeginPlay()
{
	Super::BeginPlay();
}

// GENDER
void UGeneticsManager::SetChildGender(EChildGender NewGender)
{
	if (!MetaHumanInstance) return;
	
	FString OptionName = (NewGender == EChildGender::Male) ? TEXT("Male") : TEXT("Female");
	MetaHumanInstance->SetIntParameterSelectedOption(TEXT("Gender"), OptionName);

	bNeedsMutableUpdate = true;
}

// EYE COLOR 
FName UGeneticsManager::CalculateEyeColor(FName FatherEyeColorID, FName MotherEyeColorID)
{
	if (!EyeGeneticsTable) return NAME_None;

	// Retrieve genetic data for both parents
	FEyeGeneticData* FatherData = EyeGeneticsTable->FindRow<FEyeGeneticData>(FatherEyeColorID, TEXT("Genetics Eye Context"));
	FEyeGeneticData* MotherData = EyeGeneticsTable->FindRow<FEyeGeneticData>(MotherEyeColorID, TEXT("Genetics Eye Context"));

	if (FatherData && MotherData)
	{
		// Determine child's eye color based on parents' dominance
		return SelectDominantPhenotype(FatherEyeColorID, FatherData->DominanceIndex, MotherEyeColorID, MotherData->DominanceIndex);
	}
	return NAME_None;
}

void UGeneticsManager::ApplyEyeColorGenetics(FName ChildEyeColorID)
{
	if (!MetaHumanInstance) return;
	bNeedsMutableUpdate = true; 

	// Apply result to MetaHuman instance
	MetaHumanInstance->SetIntParameterSelectedOption(FString("EyeColor"), FString(ChildEyeColorID.ToString()));
}

// SKIN TONE
float UGeneticsManager::CalculateSkinTone(FName FatherSkinToneID, FName MotherSkinToneID)
{
	if (!SkinGeneticsTable) return 0.5f;

	FSkinGeneticData* FatherData = SkinGeneticsTable->FindRow<FSkinGeneticData>(FatherSkinToneID, TEXT("Genetics Skin Context"));
	FSkinGeneticData* MotherData = SkinGeneticsTable->FindRow<FSkinGeneticData>(MotherSkinToneID, TEXT("Genetics Skin Context"));

	if (FatherData && MotherData) {
		// Determine skin tone range
		float MinMelanin = FMath::Min(FatherData->MelaninIndex, MotherData->MelaninIndex);
		float MaxMelanin = FMath::Max(FatherData->MelaninIndex, MotherData->MelaninIndex);

		// Favor darker skin tones
		const float DarknessBias = 0.4f;
		float RandomValue = FMath::Pow(FMath::FRand(), DarknessBias);

		float ChildMelaninIndex = FMath::Lerp(MinMelanin, MaxMelanin, RandomValue);

		return ChildMelaninIndex;
	}
	return 0.5f;
}

void UGeneticsManager::ApplySkinToneGenetics(float ChildSkinToneID)
{		
	if (!MetaHumanInstance) return;
	bNeedsMutableUpdate = true;
	MetaHumanInstance->SetFloatParameterSelectedOption(FString("SkinTone"), ChildSkinToneID);
}

//HAIR TEXTURE
int32 UGeneticsManager::CalculateHairTexture(FName FatherHairTexID, FName MotherHairTexID)
{
	if (!HairTextureGeneticsTable) return 1;

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

		return ChildTexture;
	}
	return 1;
}

void UGeneticsManager::ApplyHairTextureGenetics(int32 ChildHairTexID)
{
	if (!MetaHumanInstance) return;

	FString ResultTextureType;
	switch (ChildHairTexID)
	{
		case 0: ResultTextureType = "Straight"; break;
		case 1: ResultTextureType = "Wavy"; break;
		case 2: ResultTextureType = "Curly"; break;
		default: ResultTextureType = "Wavy"; break; 
	}
	bNeedsMutableUpdate = true;

	MetaHumanInstance->SetIntParameterSelectedOption("Hair", ResultTextureType);
}

// HAIR COLOR
FName UGeneticsManager::CalculateHairColor(FName FatherHairColorID, FName MotherHairColorID)
{
	if (!HairColorGeneticsTable) return NAME_None;

	FHairColorGeneticData* FatherData = HairColorGeneticsTable->FindRow<FHairColorGeneticData>(FatherHairColorID, TEXT("Hair Color Context"));
	FHairColorGeneticData* MotherData = HairColorGeneticsTable->FindRow<FHairColorGeneticData>(MotherHairColorID, TEXT("Hair Color Context"));

	if (FatherData && MotherData)
	{
		return SelectDominantPhenotype(FatherHairColorID, FatherData->DominanceIndex, MotherHairColorID, MotherData->DominanceIndex);
	}
	return NAME_None;
}

void UGeneticsManager::ApplyHairColorGenetics(FName ChildHairColorID)
{
	if (!MetaHumanInstance || !HairColorGeneticsTable) return;

	FHairColorGeneticData* WinnerData = HairColorGeneticsTable->FindRow<FHairColorGeneticData>(ChildHairColorID, TEXT("Hair Color Context"));
	if (!WinnerData) return;
	UMaterialInterface* LoadedHairMaterial = WinnerData->HairMaterial.LoadSynchronous();
	if (!LoadedHairMaterial) return;

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
					int32 NumMaterials = Groom->GetNumMaterials();
					for (int32 SlotIndex = 0; SlotIndex < NumMaterials; ++SlotIndex)
					{
						if (Groom->GetMaterial(SlotIndex) != LoadedHairMaterial)
						{
							Groom->SetMaterial(SlotIndex, LoadedHairMaterial);
						}
					}
					Groom->MarkRenderStateDirty();
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


