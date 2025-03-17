﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "TeamProject_WOG/TeamProject_WOG.h"
#include "KJW/Thor.h"
#include "AHS/KratosCharacter.h"
#include "ThorPattern.generated.h"


UCLASS(Blueprintable)
class TEAMPROJECT_WOG_API UThorPattern : public UObject
{
    GENERATED_BODY()

protected:
    UPROPERTY(BlueprintReadWrite)
    AThor* Owner;
public:
    UPROPERTY(EditAnywhere)
    EThorPattern ThorPattern = EThorPattern::NONE;
public:
    virtual void InitPattern(class AThor* Thor);
public:
    void StartPattern(); //시작시 호출되는
    void StopPattern(); //중단시 호출되는
    void EndPattern();  //정상 종료시 호출되는
    void TickPattern(); //매 틱마다 호출
    
    void NotifyEventPattern(int32 EventIndex);    
    void NotifyBeginPattern(int32 EventIndex, float TotalDuration);
    void NotifyEndPattrern(int32 EventIndex);
    void NotifyTickPattrern(int32 EventIndex, float FrameDeltaTime);

protected:
    //셋팅이 다끝나고 최종 시작 함수
    virtual void StartPattern_CBP(); 
    //C,BP 관련 스탑시 최종 스탑 함수
    virtual void StopPattern_CBP();
    //C,BP 관련 종료시 최종 종료 함수
    //상속할시 마지막에 호출시켜 주시요
    virtual void EndPattern_CBP();
    //C,BP Tick 끝나고 함수
    virtual void TickPattern_CBP();
protected:
    //FUNCTION -> C++ -> Blueprint
    virtual void StartPattern_C();
    virtual void StopPattern_C();
    virtual void EndPattern_C();
    virtual bool TickPattern_C();

    virtual void NotifyEventPattern_C(int32 EventIndex);
    virtual void NotifyBeginPattern_C(int32 EventIndex , float TotalDuration);
    virtual void NotifyEndPattrern_C(int32 EventIndex);
    virtual void NotifyTickPattrern_C(int32 EventIndex, float FrameDeltaTime);

    UFUNCTION(BlueprintNativeEvent , Category = "ThorPattern")
    void StartPattern_BP();
    
    UFUNCTION(BlueprintNativeEvent , Category = "ThorPattern")
    void StopPattern_BP();
   
    UFUNCTION(BlueprintNativeEvent , Category = "ThorPattern")
    void EndPattern_BP();
    
    UFUNCTION(BlueprintNativeEvent , Category = "ThorPattern")
    void TickPattern_BP();


    UFUNCTION(BlueprintNativeEvent , Category = "ThorPattern")
    void NotifyEventPattern_BP(int32 EventIndex);

    UFUNCTION(BlueprintNativeEvent , Category = "ThorPattern")
    void NotifyBeginPattern_BP(int32 EventIndex, float TotalDuration);

    UFUNCTION(BlueprintNativeEvent , Category = "ThorPattern")
    void NotifyEndPattrern_BP(int32 EventIndex);

    UFUNCTION(BlueprintNativeEvent , Category = "ThorPattern")
    void NotifyTickPattrern_BP(int32 EventIndex , float FrameDeltaTime);

private:
     void StartPattern_BP_Implementation();
     void StopPattern_BP_Implementation();
     void EndPattern_BP_Implementation();
     void TickPattern_BP_Implementation();

     void NotifyEventPattern_BP_Implementation(int32 EventIndex);
     void NotifyBeginPattern_BP_Implementation(int32 EventIndex , float TotalDuration);
     void NotifyEndPattrern_BP_Implementation(int32 EventIndex);
     void NotifyTickPattrern_BP_Implementation(int32 EventIndex, float FrameDeltaTime);


protected:
    //패턴이 종료되면 Idle 상태로 돌아가는데
    //Idle 상태에서 hit가능은 마지막 패턴에서 영향을 받는다.
    UPROPERTY(EditDefaultsOnly)
    bool bHitable = false;
    //패턴 종료후 Idle 상태 시간 0이면 기본 2초 대기
    UPROPERTY(EditDefaultsOnly)
    float IdleTime = 0.0f;

    bool bEndPattern = true;

public:
    virtual bool IsHitable() { return bHitable; }
    bool IsEndPattern() { return bEndPattern; }
    virtual bool IsStartable(){ return true; }

public:
    //특수한 값을 줄때 사용할 함수
    virtual void SetOptionValue(float Value_1 = 0, float Value_2 = 0, float Value_3 = 0);

protected :
    FVector MapPoint = FVector::ZeroVector;
    float MapSize = 1000.0f;

public:
    //기본 가중치 0이되면 다시 원상복귀
    UPROPERTY(EditDefaultsOnly)
    int32 DefaultPatternRate = 20;
    //현제 가중치
    UPROPERTY(VisibleAnywhere)
    int32 CurPatternRate = 20;
    //패턴을 사용할때 마다 내려가는 가중치
    UPROPERTY(EditDefaultsOnly)
    int32 MinusRate = 5;

};

