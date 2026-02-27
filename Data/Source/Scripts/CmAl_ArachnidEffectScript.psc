Scriptname CmAl_ArachnidEffectScript extends activemagiceffect  

Event OnEffectStart(Actor target, Actor caster)
    float loadSize = CmAl.GetFloat("Inflation/Arachnid")
    (_CSN_CumHandlerQuest as _CSN_CumHandler).DoCumSwallow(target, target.GetName(), 1.0, loadSize, true, true, false, true, -1)
EndEvent

Quest property _CSN_CumHandlerQuest auto
