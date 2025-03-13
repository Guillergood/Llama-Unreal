// Copyright 2025-current Getnamo.

#pragma once
#include <Components/ActorComponent.h>
#include <CoreMinimal.h>
#include "LlamaDataTypes.h"

#include "LlamaComponent.generated.h"

UCLASS(Category = "LLM", BlueprintType, meta = (BlueprintSpawnableComponent))
class LLAMACORE_API ULlamaComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    ULlamaComponent(const FObjectInitializer &ObjectInitializer);
    ~ULlamaComponent();

    virtual void Activate(bool bReset) override;
    virtual void Deactivate() override;
    virtual void TickComponent(float DeltaTime,
                                ELevelTick TickType,
                                FActorComponentTickFunction* ThisTickFunction) override;

    //Main callback, updates for each token generated
    UPROPERTY(BlueprintAssignable)
    FOnTokenGeneratedSignature OnTokenGenerated;

    //Only called when full response has been received (EOS/etc)
    UPROPERTY(BlueprintAssignable)
    FOnResponseGeneratedSignature OnResponseGenerated;

    //Utility split emit e.g. sentence level emits, useful for speech generation
    UPROPERTY(BlueprintAssignable)
    FOnPartialSignature OnPartialGenerated;

    UPROPERTY(BlueprintAssignable)
    FVoidEventSignature OnStartEval;

    //Whenever the model stops generating
    UPROPERTY(BlueprintAssignable)
    FOnEndOfStreamSignature OnEndOfStream;

    UPROPERTY(BlueprintAssignable)
    FVoidEventSignature OnContextReset;

    UPROPERTY(BlueprintAssignable)
    FModelNameSignature OnModelLoaded;

    //Catch internal errors
    UPROPERTY(BlueprintAssignable)
    FOnErrorSignature OnError;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LLM Model Component")
    FLLMModelParams ModelParams;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LLM Model Component")
    FLLMModelState ModelState;

    //Settings
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LLM Model Component")
    bool bDebugLogModelOutput = false;

    //toggle to pay copy cost or not, default true
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "LLM Model Component")
    bool bSyncPromptHistory = true;

    //loads model from params
    UFUNCTION(BlueprintCallable, Category = "LLM Model Component")
    void LoadModel();

    UFUNCTION(BlueprintCallable, Category = "LLM Model Component")
    void UnloadModel();


    //Clears the prompt, allowing a new context - Not yet implemented
    //UFUNCTION(BlueprintCallable, Category = "LLM Model Component")
    void ResetContextHistory();

    //Main input function
    UFUNCTION(BlueprintCallable, Category = "LLM Model Component")
    void InsertTemplatedPrompt(UPARAM(meta=(MultiLine=true)) const FString &Text, EChatTemplateRole Role = EChatTemplateRole::User, bool bAddAssistantBOS = false, bool bGenerateReply = true);

    //does not apply formatting before running inference
    UFUNCTION(BlueprintCallable, Category = "LLM Model Component")
    void InsertRawPrompt(UPARAM(meta = (MultiLine = true)) const FString& Text);

    /** 
    * Use this function to bypass input from AI, e.g. streaming input from another source. 
    * All downstream event functions will trigger from this call as if it came from the LLM.
    * Won't make a new message split until role is swapped from last. 
    * Doesn't work in v0.8 yet
    */
    UFUNCTION(BlueprintCallable, Category = "LLM Model Component")
    void UserImpersonateText(const FString& Text, EChatTemplateRole Role = EChatTemplateRole::Assistant,  bool bIsEos = false);

    //if you want to manually wrap prompt, if template is empty string, default model template is applies
    UFUNCTION(BlueprintPure, Category = "LLM Model Component")
    FString WrapPromptForRole(const FString& Text, EChatTemplateRole Role, const FString& Template);


    //Force stop generating new tokens
    UFUNCTION(BlueprintCallable, Category = "LLM Model Component")
    void StopGeneration();

    UFUNCTION(BlueprintCallable, Category = "LLM Model Component")
    void ResumeGeneration();

    //Obtain the currently formatted context
    UFUNCTION(BlueprintPure, Category = "LLM Model Component")
    FString RawContextHistory();

    UFUNCTION(BlueprintPure, Category = "LLM Model Component")
    FStructuredChatHistory GetStructuredHistory();

    //EChatTemplateRole LastRoleFromStructuredHistory();

private:
    class FLlamaNative* LlamaNative;

    TFunction<void(FString, int32)> TokenCallbackInternal;
};
