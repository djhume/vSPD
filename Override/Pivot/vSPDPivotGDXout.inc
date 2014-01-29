*This section of code exports the gdx file with the pivot results

execute_unload '%OutputPath%%runName%\RunNum%VSPDRunNum%_PivotOutput_TP.gdx', PivotScenario, ActivePivotScenario, o_PivotOffer, o_PivotScheduledMW_TP, o_PivotFKMin_TP, o_PivotMW_TP
                                                                              o_EnergyOffered, o_FIROffered, o_SIROffered, o_PivotEnergyOffered
                                                                              o_PivotFIROffered, o_PivotSIROffered, o_NonPhysicalLoss,
                                                                              o_ILRO_FIR_TP, o_ILRO_SIR_TP, o_ILBus_FIR_TP, o_ILBus_SIR_TP
                                                                              o_PLRO_FIR_TP, o_PLRO_SIR_TP, o_TWRO_FIR_TP, o_TWRO_SIR_TP
                                                                              o_GenerationRiskSetter, o_GenHVDCRiskSetter, o_HVDCRiskSetter
                                                                              o_MANURiskSetter, o_MANUHVDCRiskSetter, o_FIRCleared_TP, o_SIRCleared_TP,
                                                                              o_PivotOfferFIR_TP, o_PivotOfferSIR_TP
                                                                              o_IslandPosLoad_TP, o_IslandGWAP_TP,  o_IslandLWAP_TP, o_IslandLoadAtPivotPrice, o_IslandPivotTraderTotGen, o_maxPivotNodePrice;