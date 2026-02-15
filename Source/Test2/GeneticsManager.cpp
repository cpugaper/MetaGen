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

		// Apply the determined eye color to the MetaHuman instance
		MetaHumanInstance->SetIntParameterSelectedOption(FString("EyeColor"), FString(ChildEyeColor.ToString()));
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

void UGeneticsManager::ApplyHairGenetics(FName FatherHairTextID, FName MotherHairTextID)
{
	if (!MetaHumanInstance || !HairTextureGeneticsTable) return;

	FHairTextureGeneticData* FatherTex = HairTextureGeneticsTable->FindRow<FHairTextureGeneticData>(FatherHairTextID, TEXT("Hair Texture Context"));
	FHairTextureGeneticData* MotherTex = HairTextureGeneticsTable->FindRow<FHairTextureGeneticData>(MotherHairTextID, TEXT("Hair Texture Context"));

	if (FatherTex && MotherTex)
	{
		// Determine hair texture range
		float MinType = FMath::Min(FatherTex->TextureDominance, MotherTex->TextureDominance);
		float MaxType = FMath::Max(FatherTex->TextureDominance, MotherTex->TextureDominance);

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

		MetaHumanInstance->SetIntParameterSelectedOption("Hair", ResultTextureType);

		if (GEngine)
		{
			FString Msg = FString::Printf(TEXT("Padres [%d, %d] -> Raw: %.2f -> Final: %s"),
				(int32)MinType, (int32)MaxType, RawResult, *ResultTextureType);
			GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, Msg);
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


