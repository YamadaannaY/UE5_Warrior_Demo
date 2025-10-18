#pragma once
namespace Debug
{
	//打印信息到Log/Screen
	static void Print(
		const FString& Msg,
		const FColor& Color=FColor::MakeRandomColor(),
		int32 Inkey= -1)
	{
		if (GEngine)
		{
			//如果项目启动，将msg信息打印到屏幕
			GEngine->AddOnScreenDebugMessage(Inkey,7.f, Color, Msg);
			UE_LOG(LogTemp,Warning,TEXT("%s"),*Msg);
		}
	}

	static void Print(const FString& FloatTitle,float FloatValueToPrint,int32 InKey=-1,const FColor& Color=FColor::MakeRandomColor())
	{
		if(GEngine)
		{
			const FString FinalMsg=FloatTitle+TEXT(":")+FString::SanitizeFloat(FloatValueToPrint);

			GEngine->AddOnScreenDebugMessage(InKey,7.f,Color,FinalMsg);
			UE_LOG(LogTemp,Warning,TEXT("%s"),*FinalMsg);
			
		}
	}
}

