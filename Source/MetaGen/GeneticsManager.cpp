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
FName UGeneticsManager::CalculateEyeColor(FName P1EyeColorID, FName P2EyeColorID)
{
	if (!EyeGeneticsTable) return NAME_None;

	// Retrieve genetic data for both parents
	FEyeGeneticData* P1Data = EyeGeneticsTable->FindRow<FEyeGeneticData>(P1EyeColorID, TEXT("Genetics Eye Context"));
	FEyeGeneticData* P2Data = EyeGeneticsTable->FindRow<FEyeGeneticData>(P2EyeColorID, TEXT("Genetics Eye Context"));

	if (P1Data && P2Data)
	{
		// Determine child's eye color based on parents' dominance
		return SelectDominantPhenotype(P1EyeColorID, P1Data->DominanceIndex, P2EyeColorID, P2Data->DominanceIndex);
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
float UGeneticsManager::CalculateSkinTone(FName P1SkinToneID, FName P2SkinToneID)
{
	if (!SkinGeneticsTable) return 0.5f;

	FSkinGeneticData* P1Data = SkinGeneticsTable->FindRow<FSkinGeneticData>(P1SkinToneID, TEXT("Genetics Skin Context"));
	FSkinGeneticData* P2Data = SkinGeneticsTable->FindRow<FSkinGeneticData>(P2SkinToneID, TEXT("Genetics Skin Context"));

	if (P1Data && P2Data) {
		// Determine skin tone range
		float MinMelanin = FMath::Min(P1Data->MelaninIndex, P2Data->MelaninIndex);
		float MaxMelanin = FMath::Max(P1Data->MelaninIndex, P2Data->MelaninIndex);

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
int32 UGeneticsManager::CalculateHairTexture(FName P1HairTexID, FName P2HairTexID)
{
	if (!HairTextureGeneticsTable) return 1;

	FHairTextureGeneticData* P1Data = HairTextureGeneticsTable->FindRow<FHairTextureGeneticData>(P1HairTexID, TEXT("Hair Texture Context"));
	FHairTextureGeneticData* P2Data = HairTextureGeneticsTable->FindRow<FHairTextureGeneticData>(P2HairTexID, TEXT("Hair Texture Context"));

	if (P1Data && P2Data)
	{
		// Determine hair texture range
		float MinType = FMath::Min(P1Data->DominanceIndex, P2Data->DominanceIndex);
		float MaxType = FMath::Max(P1Data->DominanceIndex, P2Data->DominanceIndex);

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
FName UGeneticsManager::CalculateHairColor(FName P1HairColorID, FName P2HairColorID)
{
	if (!HairColorGeneticsTable) return NAME_None;

	FHairColorGeneticData* P1Data = HairColorGeneticsTable->FindRow<FHairColorGeneticData>(P1HairColorID, TEXT("Hair Color Context"));
	FHairColorGeneticData* P2Data = HairColorGeneticsTable->FindRow<FHairColorGeneticData>(P2HairColorID, TEXT("Hair Color Context"));

	if (P1Data && P2Data)
	{
		return SelectDominantPhenotype(P1HairColorID, P1Data->DominanceIndex, P2HairColorID, P2Data->DominanceIndex);
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


