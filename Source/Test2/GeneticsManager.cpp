#include "GeneticsManager.h"

UGeneticsManager::UGeneticsManager()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGeneticsManager::BeginPlay()
{
	Super::BeginPlay();
}

void UGeneticsManager::ApplyEyeColorGenetics(FName FatherPhenotype, FName MotherPhenotype)
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
	static const FString ContextString(TEXT("Genetics Context"));
	FGeneticData* FatherData = EyeGeneticsTable->FindRow<FGeneticData>(FatherPhenotype, ContextString);
	FGeneticData* MotherData = EyeGeneticsTable->FindRow<FGeneticData>(MotherPhenotype, ContextString);

	if (!FatherData || !MotherData)
	{
		UE_LOG(LogTemp, Error, TEXT("ERROR: Could not find genetic data '%s' or '%s' in the table"), *FatherPhenotype.ToString(), *MotherPhenotype.ToString());
		return;
	}

	// Determine child's eye color based on parents' dominance
	FName ChildEyeColor = DetermineEyeColor(FatherData->DominanceIndex, FatherPhenotype, MotherData->DominanceIndex, MotherPhenotype);

	// Apply the determined eye color to the MetaHuman instance
	MetaHumanInstance->SetIntParameterSelectedOption(FString("EyeColor"), FString(ChildEyeColor.ToString()));
	MetaHumanInstance->UpdateSkeletalMeshAsync(true, true);

	UE_LOG(LogTemp, Warning, TEXT("Applied eye color '%s' to MetaHuman based on parents '%s' and '%s'"), *ChildEyeColor.ToString(), *FatherPhenotype.ToString(), *MotherPhenotype.ToString());
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


