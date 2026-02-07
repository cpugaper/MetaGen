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
	if (!MetaHumanInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: MetaHumanInstance not assigned"));
		return;
	}
	if (!EyeGeneticsTable)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: EyeGeneticsTable not assigned"));
		return;
	}

	// Retrieve genetic data for both parents
	FEyeGeneticData* FatherData = EyeGeneticsTable->FindRow<FEyeGeneticData>(FatherEyeColorID, TEXT("Genetics Eye Context"));
	FEyeGeneticData* MotherData = EyeGeneticsTable->FindRow<FEyeGeneticData>(MotherEyeColorID, TEXT("Genetics Eye Context"));

	if (!FatherData || !MotherData)
	{
		UE_LOG(LogTemp, Error, TEXT("Genetics: Could not find eye data for '%s' or '%s'"), *FatherEyeColorID.ToString(), *MotherEyeColorID.ToString());
		return;
	}

	// Determine child's eye color based on parents' dominance
	FName ChildEyeColor = DetermineEyeColor(FatherData->DominanceIndex, FatherEyeColorID, MotherData->DominanceIndex, MotherEyeColorID);

	// Apply the determined eye color to the MetaHuman instance
	MetaHumanInstance->SetIntParameterSelectedOption(FString("EyeColor"), FString(ChildEyeColor.ToString()));

	UE_LOG(LogTemp, Warning, TEXT("Genetics: Applied eye '%s' (Parents: %s + %s)"), *ChildEyeColor.ToString(), *FatherEyeColorID.ToString(), *MotherEyeColorID.ToString());
}

FName UGeneticsManager::DetermineEyeColor(int32 DomA, FName NameA, int32 DomB, FName NameB)
{
	float ChanceForA = 0.5f;

	// Mendel's laws: 75% chance for the dominant trait, 25% for the recessive
	if (DomA > DomB) {
		ChanceForA = 0.75f; 
	}
	else if (DomB > DomA) {
		ChanceForA = 0.25f;
	}
	
	float RandomValue = FMath::FRand(); 

	if (RandomValue <= ChanceForA) {
		return NameA; 
	} else {
		return NameB; 
	}
}


void UGeneticsManager::ApplySkinToneGenetics(FName FatherSkinID, FName MotherSkinID)
{
	if (!MetaHumanInstance || !SkinGeneticsTable) return;

	// Retrieve genetic data for both parents
	FSkinGeneticData* FatherData = SkinGeneticsTable->FindRow<FSkinGeneticData>(FatherSkinID, TEXT("Genetics Skin Context"));
	FSkinGeneticData* MotherData = SkinGeneticsTable->FindRow<FSkinGeneticData>(MotherSkinID, TEXT("Genetics Skin Context"));

	// Determine child's skin tone based on parents' dominance
	
	if (FatherData && MotherData) {
		float ChildMelaninIndex = (FatherData->MelaninIndex + MotherData->MelaninIndex) / 2.0f;
		float Mutation = FMath::FRandRange(-0.05f, 0.05f);
		ChildMelaninIndex = FMath::Clamp(ChildMelaninIndex + Mutation, 0.0f, 1.0f);

		MetaHumanInstance->SetFloatParameterSelectedOption(FString("SkinTone"), ChildMelaninIndex);
		UE_LOG(LogTemp, Warning, TEXT("Applied skin tone with melanin index '%.2f' to MetaHuman based on parents '%s' and '%s'"), ChildMelaninIndex, *FatherSkinID.ToString(), *MotherSkinID.ToString());
	}
}


