// Fill out your copyright notice in the Description page of Project Settings.

#include "SExplosiveBarrel.h"
#include "Components/SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	ForceComp = CreateDefaultSubobject< URadialForceComponent>(TEXT("ForceComp"));
	ForceComp->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	ExplosionDamage = 50.0f;
	ExplosionRadius = 200.0f;
}

// Called when the game starts or when spawned
void ASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);

	if (MeshComp && StartingMat)
	{
		MeshComp->SetMaterial(0, StartingMat);
	}
}

void ASExplosiveBarrel::OnHealthChanged(USHealthComponent* OwningHealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if (Health <= 0.0f)
	{
		Explode();
	}
}

void ASExplosiveBarrel::Explode()
{
	if (!bExploded)
	{
		// Sets it as exploded
		bExploded = true;

		// Play particle effect
		if (ExplosionEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());
		}

		// Change material
		if (ExplodedMat && MeshComp)
		{
			MeshComp->SetMaterial(0, ExplodedMat);
		}

		// Apply radial damage
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage, GetActorLocation(), ExplosionRadius, DamageType, TArray<AActor*>(), this, this->GetInstigatorController(), true, ECC_Visibility);

		// Apply radial force
		ForceComp->FireImpulse();

		UE_LOG(LogTemp, Log, TEXT("BOOM!"));
	}

}


