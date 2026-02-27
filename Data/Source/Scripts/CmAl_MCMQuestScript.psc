Scriptname CmAl_MCMQuestScript extends SKI_ConfigBase  

;---------------
; Initialization 
;---------------

Function ConfigInit() Native

Event OnConfigInit()
    ConfigInit()
EndEvent

;-------------
; Render Event
;-------------

Function PageReset(String page) Native

Event OnPageReset(String page)
    PageReset(page)
EndEvent

; --------------
; Generic Events
; --------------

Function OptionSelect(Int id) Native
Function OptionHighLight(Int id) Native
Function OptionDefault(Int id) Native

Event OnOptionSelect(int id)
    OptionSelect(id)
EndEvent

Event OnOptionHighLight(int id)
    OptionHighLight(id)
EndEvent

Event OnOptionDefault(int id)
    OptionDefault(id)
EndEvent

; -------------
; Slider Events
; -------------

Function OptionSliderOpen(Int id) Native
Function OptionSliderAccept(int id, float value) Native

Event OnOptionSliderOpen(int id)
    OptionSliderOpen(id)
EndEvent

Event OnOptionSliderAccept(int id, float value)
    OptionSliderAccept(id, value)
EndEvent