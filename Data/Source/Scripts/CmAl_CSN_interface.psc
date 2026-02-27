Scriptname CmAl_CSN_interface extends Quest  

Function CmAl_CSN_native_handler(Form akactor, bool swallowed, string cumDonorName, Form cumDonor, float cumAmount, int cumID) Global Native

Function Register()
	RegisterForModEvent("_CSN_CumEvent", "CmAl_CNS_Handler")
;	Debug.Notification("installed CmAl_CNS_Handler")
EndFunction

Event OnPlayerLoadGame()
	Register()
EndEvent

Event OnInit()
	Register()
EndEvent

Event CmAl_CNS_Handler(Form akactor, bool swallowed, string cumDonorName, Form cumDonor, float cumAmount, int cumID)
	CmAl_CSN_native_handler(akactor, swallowed, cumDonorName, cumDonor, cumAmount, cumID)
EndEvent