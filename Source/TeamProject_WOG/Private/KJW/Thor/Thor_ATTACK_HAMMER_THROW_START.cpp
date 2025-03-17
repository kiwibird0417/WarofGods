﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "KJW/Thor/Thor_ATTACK_HAMMER_THROW_START.h"
#include "KJW/ThorHammer.h"
#include "GameFramework/CharacterMovementComponent.h"

void UThor_ATTACK_HAMMER_THROW_START::InitPattern(AThor* Thor)
{
	Super::InitPattern(Thor);

	bHitable = false;
}

void UThor_ATTACK_HAMMER_THROW_START::StartPattern_C()
{
	Super::StartPattern_C();
	//망치를 받는 패턴에 rate를 추가
	UThorPattern* end = Owner->GetPattern(EThorPattern::ATTACK_HAMMER_THROW_END);
	if ( end )
	{
		end->CurPatternRate = 50;
	}

	//패턴시작시 히트 상태 전환불가
	bHitable = false;

	//타겟과의 거리 계산
	FVector Distance = Owner->GetActorLocation() - Owner->Target->GetActorLocation();
	Distance.Z = 0;

	float DisSize = Distance.Size();
	Distance.Normalize();
	UE_LOG(LogTemp, Warning, TEXT("DisSize : %f") , DisSize);

	Owner->ShowHammerEffect(true);

	bEndPattern = false;
	//멀리 있을때
	if (DisSize >= AttackRange*0.9f)
	{	
		bEndPattern = true;
		ParrtenIndex = 0;
	}
	else
	{
		//뒤로 갈수있는지 체크
		FVector backRot = Distance;
		float dashDistance = AttackRange;
		TArray<FVector> MoveableVector;
		for (int32 i = -3; i < 4; i++)
		{
			float addAngleDeg = 15 * i;
			FVector checkDir = Distance.RotateAngleAxis(addAngleDeg, FVector::UpVector);
			
			FVector checkLocation = Owner->GetActorLocation() + (checkDir * dashDistance);
			if (FVector::Distance(checkLocation, MapPoint) < MapSize - 100)
			{
				MoveableVector.Add(checkLocation);
			}

			 checkLocation = Owner->GetActorLocation() + ( checkDir * (dashDistance * 0.5f) );
			if ( FVector::Distance(checkLocation , MapPoint) < MapSize - 100 )
			{
				MoveableVector.Add(checkLocation);
			}
		}
		//뒤로 갈 수 있으면 가고
		if (MoveableVector.Num() > 0)
		{
			ParrtenIndex = 1;

			int32 moveIndex = FMath::RandRange(0, MoveableVector.Num() -1);
			//디버그
			//for (int32 i = 0; i < MoveableVector.Num(); i++)
			//{
			//	DrawDebugLine(Owner->GetWorld() , MoveableVector[i],MoveableVector[i] + FVector(0, 0, 100), i == moveIndex ?FColor::Green : FColor::Red ,false,	2.0f , 0,2.0f);
			//}
			
			//디버그	
			StartPos = Owner->GetActorLocation();
			MovePos = MoveableVector[moveIndex];

		}
		else	//아니면 킥
		{
			ParrtenIndex = 2;
		}
		
	}

}

void UThor_ATTACK_HAMMER_THROW_START::StartPattern_CBP()
{
	if (ParrtenIndex == 0)
	{
		Owner->GetSkeletalMesh()->GetAnimInstance()->Montage_Play(AnimMontage);
	}
	else if (ParrtenIndex == 1)
	{
		DashTimer = Owner->GetSkeletalMesh()->GetAnimInstance()->Montage_Play(DashAnimMontage);
		DashDuringTimer = 0.0f;
		UE_LOG(LogTemp, Warning, TEXT("%f") , DashTimer);
	}
	else if (ParrtenIndex == 2)
	{
		Owner->GetSkeletalMesh()->GetAnimInstance()->Montage_Play(KickAnimMontage);
	}

}

bool UThor_ATTACK_HAMMER_THROW_START::TickPattern_C()
{
	if (ParrtenIndex == 1)
	{
		DashDuringTimer += Owner->GetWorld()->GetDeltaSeconds();
		if (DashDuringTimer < DashTimer)
		{
			float alpha = DashDuringTimer / DashTimer;
			FVector newMovePos = FMath::InterpEaseOut(StartPos, MovePos, alpha, 2.0f);
			FVector distance = Owner->Target->GetActorLocation() - Owner->GetActorLocation();
			distance.Normalize();
			FRotator rot = distance.Rotation();

			Owner->SetActorLocationAndRotation(newMovePos, rot);
			
		}
		else
		{
			StartPattern();
		}
	}


	return true;
}

void UThor_ATTACK_HAMMER_THROW_START::NotifyTickPattrern_C(int32 EventIndex, float FrameDeltaTime)
{
	if ( ParrtenIndex == 0 )
	{
		FVector distance = Owner->Target->GetActorLocation() - Owner->GetActorLocation();
		FRotator rot = distance.Rotation();
		Owner->SetActorRotation(rot);
	}
	else if (ParrtenIndex == 2)
	{
		FVector distance = Owner->Target->GetActorLocation() - Owner->GetActorLocation();
		if (distance.Size() < 150) { return; }
		distance.Normalize();
		
		FRotator rot = distance.Rotation();
		FVector newMovePos = Owner->GetActorLocation() + (distance * FrameDeltaTime * 500.f);
		Owner->SetActorLocationAndRotation(newMovePos, rot);
	}
	

}

void UThor_ATTACK_HAMMER_THROW_START::NotifyEventPattern_C(int32 EventIndex)
{
	if ( ParrtenIndex == 0)
	{
		Owner->ShowHammer(false);
		FVector MoveLocation = Owner->HammerComp->GetComponentLocation();
		Owner->ThorHammer->SetActorLocation(MoveLocation);

		FVector flyDir = (Owner->Target->GetActorLocation() + FVector(0.0f , 0.0f , 30.0f)) - MoveLocation;
		flyDir.Normalize();
		Owner->ShowHammerEffect(false);
		Owner->ThorHammer->StartHammerFly(flyDir);

	}
	else if (ParrtenIndex == 2 && EventIndex == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Kick !!!!"));

		FHitResult OutHit;
		// 레이의 시작 지점
		FVector Start = FVector(0.f , 0.f , 0.f);
		Start = Owner->GetActorLocation() + Owner->GetActorForwardVector() * KickAttackRadius;

		// 충돌 쿼리 파라미터
		FCollisionQueryParams CollisionParams;
		bool bHit = Owner->GetWorld()->SweepSingleByChannel(OutHit , Start , Start , FQuat::Identity , EWOGTraceChannel::EnemyAttackTrace , FCollisionShape::MakeSphere(KickAttackRadius) , CollisionParams);

		FColor SphereColor = bHit ? FColor::Red : FColor::Green;
		DrawDebugSphere(Owner->GetWorld() , Start , KickAttackRadius , 12 , SphereColor , false , 1.0f , 0 , 2.0f);

		if ( bHit )
		{
			FWOG_DamageEvent DamageData;
			DamageData.DamageValue = 10;
			DamageData.HitPoint = OutHit.ImpactPoint;
			Owner->Target->TakeKDamage(DamageData , Owner);
			FVector PlayerVelocity = Owner->Target->GetActorForwardVector();
			Owner->Target->GetCharacterMovement()->Velocity = PlayerVelocity * -1.0f * 20000.0f;
		}

	}
	else if (ParrtenIndex == 2 && EventIndex == 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Kick End!!!!"));
		StartPattern();
	}
	
}

void UThor_ATTACK_HAMMER_THROW_START::NotifyBeginPattern_C(int32 EventIndex , float TotalDuration)
{
	if ( ParrtenIndex == 0 )
	{
		Owner->GetSkeletalMesh()->GetAnimInstance()->Montage_SetPlayRate(AnimMontage , 0.3f);
	}
}

void UThor_ATTACK_HAMMER_THROW_START::NotifyEndPattrern_C(int32 EventIndex)
{
	if ( ParrtenIndex == 0 )
	{
		Owner->GetSkeletalMesh()->GetAnimInstance()->Montage_SetPlayRate(AnimMontage , 1.0f);
	}
}

bool UThor_ATTACK_HAMMER_THROW_START::IsStartable()
{
	//현제 토르 해머를 가지고 있다면
	return Owner->IsHammer;
}

void UThor_ATTACK_HAMMER_THROW_START::OnOverlapHammer()
{
	FWOG_DamageEvent DamageData;
	DamageData.DamageValue = 10;
	DamageData.HitPoint = Owner->ThorHammer->GetActorLocation();
	Owner->Target->TakeKDamage(DamageData , Owner);
}
