// Copyright 2025-current Getnamo.

#pragma once

#include "LlamaDataTypes.h"
#include "CoreMinimal.h"
//#include "LlamaNative.generated.h"


/** 
* C++ native wrapper in Unreal styling for Llama.cpp with threading and callbacks. Embed in final place
* where it should be used e.g. ActorComponent, UObject, or Subsystem subclass.
*/
class LLAMACORE_API FLlamaNative
{
public:

	//Callbacks
	TFunction<void(const FString& Token)> OnTokenGenerated;
	TFunction<void(const FString& Response)> OnResponseGenerated;	//per round
	TFunction<void()> OnGenerationStarted;
	TFunction<void(const FLlamaRunTimings& Timings)> OnGenerationFinished;
	TFunction<void(const FString& ModelPath)> OnModelLoaded;
	TFunction<void(const FString& ErrorMessage)> OnError;
	TFunction<void(const FLLMModelState& UpdatedModelState)> OnModelStateChanged;

	//Expected to be set before load model
	void SetModelParams(const FLLMModelParams& Params);

	//Loads the model found at ModelParams.PathToModel, use SetModelParams to specify params before loading
	bool LoadModel();
	bool UnloadModel();
	bool IsModelLoaded();

	//Prompt input
	void InsertPrompt(const FString& Prompt);
	bool IsGenerating();
	void StopGeneration();
	void ResumeGeneration();

	//tick forward for safely consuming messages
	void OnTick(float DeltaTime);

	//Context change - not yet implemented
	void ResetContextHistory();	//full reset
	void RemoveLastInput();		//chat rollback to undo last user input
	void RemoveLastReply();		//chat rollback to undo last assistant input.
	void RegenerateLastReply(); //removes last reply and regenerates (changing seed?)

	//Pure query of current context - not threadsafe, be careful when these get called - TBD: make it safe
	int32 RawContextHistory(FString& OutContextString);
	void GetStructuredChatHistory(FStructuredChatHistory& OutChatHistory);
	int32 UsedContextLength();

	FLlamaNative();
	~FLlamaNative();
private:

	FLLMModelParams ModelParams;
	FLLMModelState ModelState;

	FThreadSafeBool bThreadIsActive = false;
	FThreadSafeBool bCallbacksAreValid = false;

	TQueue<FString> TokenQueue;

	class FLlamaInternal* Internal = nullptr;
};