// Fill out your copyright notice in the Description page of Project Settings.

#include "SExplosiveBarrel.h"
#include "Components/SHealthComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	ExplosionDamage = 50.0f;
	ExplosionRadius = 200.0f;
	ExplosionImpulse = 400.0f;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	// Set to physics body so exploding barrels can affect each other
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = MeshComp;

	ForceComp = CreateDefaultSubobject< URadialForceComponent>(TEXT("ForceComp"));
	ForceComp->SetupAttachment(MeshComp);
	ForceComp->Radius = ExplosionRadius;
	ForceComp->bImpulseVelChange = true;
	ForceComp->bAutoActivate = false; // So comp doesn't tick and only happens when FireImpulse is called
	ForceComp->bIgnoreOwningActor = true; // ignore self

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &ASExplosiveBarrel::OnHealthChanged);
	
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

		// Boost the barrel upwards
		FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
		MeshComp->AddImpulse(BoostIntensity, NAME_None, true);

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


