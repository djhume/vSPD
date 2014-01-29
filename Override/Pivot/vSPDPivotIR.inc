*Defines the set of scenarios for the pivotal analysis
Sets
PivotScenario		/MRPL_NI, CTCT_NI, GENE_NI, MERI_SI, CTCT_SI/ 
;


Sets
ActivePivotScenario(PivotScenario)	
PivotTrader(i_Trader)
PivotIsland(i_Island)
PivotOffer(i_Offer)
o_PivotOffer(i_Offer,i_Island)

o_pivotMnodeMinmap(i_DateTime,i_Island,i_MNodeConstraint)
;

alias (i_TradeBlock,i_TradeBlock1);

Parameters
o_PivotScheduledMW_TP(i_DateTime,i_Island)
o_PivotFKMin_TP(i_DateTime,i_Island)
o_PivotMW_TP(i_DateTime,i_Island)

o_EnergyOffered(i_DateTime,i_Island) 
o_FIROffered(i_DateTime,i_Island) 
o_SIROffered(i_DateTime,i_Island) 

o_PivotEnergyOffered(i_DateTime,i_Island)
o_PivotFIROffered(i_DateTime,i_Island)
o_PivotSIROffered(i_DateTime,i_Island)

o_PivotOfferFIR_TP(i_DateTime,i_Island)
o_PivotOfferSIR_TP(i_DateTime,i_Island)

o_maxPivotNodePrice(i_DateTime,i_Island)

o_IslandAllGen_TP(i_DateTime,i_Island)
o_IslandPosLoad_TP(i_DateTime,i_Island)
o_IslandGWAP_TP(i_DateTime,i_Island)
o_IslandLWAP_TP(i_DateTime,i_Island)

o_IslandLoadAtPivotPrice(i_DateTime,i_Island)
o_IslandPivotTraderTotGen(i_DateTime,i_Island)


i_TotFIR(i_TradePeriod)
i_TotSIR(i_TradePeriod)
i_FIRProp(i_TradePeriod,i_Offer,i_TradeBlock)
i_SIRProp(i_TradePeriod,i_Offer,i_TradeBlock)
i_FIRAlloc(i_TradePeriod,i_Offer,i_TradeBlock)
i_SIRAlloc(i_TradePeriod,i_Offer,i_TradeBlock)
i_TotFIRRemain(i_TradePeriod)
i_TotSIRRemain(i_TradePeriod)
;


Scalars
i_EnergyTriggerPrice    'Prices above this will be changed to pivot price'	/-1/
i_FIRTriggerPrice       'Prices above this will be changed to pivot price'     	/-1/
i_SIRTriggerPrice       'Prices above this will be changed to pivot price'    	/-1/
i_PivotPrice            'Pivot price'         					/30000/
i_PivotPriceThreshold	'Threshold used for load at pivot price'		/0.8/
i_RemoveFKPivot		'Remove the pivot generator as the FKeeper'		/0/
;


*Set the active pivot scenario
ActivePivotScenario('GENE_NI') = yes;
*ActivePivotScenario('MRPL_NI') = yes;
*ActivePivotScenario('CTCT_NI') = yes;
*ActivePivotScenario('MERI_SI') = yes;
*ActivePivotScenario('CTCT_SI') = yes;


*Initialise
PivotTrader(i_Trader) = no;
PivotIsland(i_Island) = no;
PivotOffer(i_Offer) = no;

o_PivotFKMin_TP(i_DateTime,i_Island) = 0;

*Set up the pivot trader and pivot island

*MERI - SI - Start--------------------------------------------------
PivotTrader(i_Trader) $ (sum(sameas(i_Trader,'MERI'),1) and ActivePivotScenario('MERI_SI')) = yes;
PivotTrader(i_Trader) $ (sum(sameas(i_Trader,'38481'),1) and ActivePivotScenario('MERI_SI')) = yes;

*From 09-Mar-13 to 11-June-13 TWI IL is under MERI control
if (((InputGDXGDate >= 40976) and (InputGDXGDate <= 41070)),
    PivotTrader(i_Trader) $ (sum(sameas(i_Trader,'38430'),1) and ActivePivotScenario('MERI_SI')) = yes;
);

PivotIsland('SI') $ ActivePivotScenario('MERI_SI') = yes;
*MERI - SI - End-----------------------------------------------------


*GENE - NI - Start--------------------------------------------------
PivotTrader(i_Trader) $ (sum(sameas(i_Trader,'GENE'),1) and ActivePivotScenario('GENE_NI')) = yes;
PivotTrader(i_Trader) $ (sum(sameas(i_Trader,'38459'),1) and ActivePivotScenario('GENE_NI')) = yes;
PivotIsland('NI') $ ActivePivotScenario('GENE_NI') = yes;
*GENE - NI - End-----------------------------------------------------

*MRPL - NI - Start--------------------------------------------------
PivotTrader(i_Trader) $ (sum(sameas(i_Trader,'MRPL'),1) and ActivePivotScenario('MRPL_NI')) = yes;
PivotTrader(i_Trader) $ (sum(sameas(i_Trader,'38486'),1) and ActivePivotScenario('MRPL_NI')) = yes;
PivotIsland('NI') $ ActivePivotScenario('MRPL_NI') = yes;
*MRPL - NI - End-----------------------------------------------------

*CTCT - NI - Start--------------------------------------------------
PivotTrader(i_Trader) $ (sum(sameas(i_Trader,'CTCT'),1) and ActivePivotScenario('CTCT_NI')) = yes;
PivotTrader(i_Trader) $ (sum(sameas(i_Trader,'38433'),1) and ActivePivotScenario('CTCT_NI')) = yes;
PivotIsland('NI') $ ActivePivotScenario('CTCT_NI') = yes;
*CTCT - NI - End-----------------------------------------------------

*CTCT - SI - Start--------------------------------------------------
PivotTrader(i_Trader) $ (sum(sameas(i_Trader,'CTCT'),1) and ActivePivotScenario('CTCT_SI')) = yes;
PivotTrader(i_Trader) $ (sum(sameas(i_Trader,'38433'),1) and ActivePivotScenario('CTCT_SI'))  = yes;
PivotIsland('SI') $ ActivePivotScenario('CTCT_SI') = yes;
*CTCT - SI - End-----------------------------------------------------


*Set up the pivot trader and pivot island


PivotOffer(i_Offer) $ sum((i_TradePeriod,PivotTrader,PivotIsland,i_Node,i_Bus) $ (i_TradePeriodOfferTrader(i_TradePeriod,i_Offer,PivotTrader)
                                                                                 and i_TradePeriodOfferNode(i_TradePeriod,i_Offer,i_Node)
                                                                                 and i_TradePeriodNodeBus(i_TradePeriod,i_Node,i_Bus)
                                                                                 and i_TradePeriodBusIsland(i_TradePeriod,i_Bus,PivotIsland)), 1) = yes;


*RDN - Apply the pivot price

i_TradePeriodEnergyOffer(i_TradePeriod,PivotOffer,i_TradeBlock,'i_GenerationMWOfferPrice') $ (i_TradePeriodEnergyOffer(i_TradePeriod,PivotOffer,i_TradeBlock,'i_GenerationMWOfferPrice') >= i_EnergyTriggerPrice) = i_PivotPrice;

i_TradePeriodSustainedPLSROffer(i_TradePeriod,PivotOffer,i_TradeBlock,'i_PLSROfferPrice') $ (i_TradePeriodSustainedPLSROffer(i_TradePeriod,PivotOffer,i_TradeBlock,'i_PLSROfferPrice') >= i_SIRTriggerPrice) = i_PivotPrice;

i_TradePeriodFastPLSROffer(i_TradePeriod,PivotOffer,i_TradeBlock,'i_PLSROfferPrice') $ (i_TradePeriodFastPLSROffer(i_TradePeriod,PivotOffer,i_TradeBlock,'i_PLSROfferPrice') >= i_FIRTriggerPrice) = i_PivotPrice;

i_TradePeriodSustainedTWDROffer(i_TradePeriod,PivotOffer,i_TradeBlock,'i_TWDROfferPrice') $ (i_TradePeriodSustainedTWDROffer(i_TradePeriod,PivotOffer,i_TradeBlock,'i_TWDROfferPrice') >= i_SIRTriggerPrice) = i_PivotPrice;

i_TradePeriodFastTWDROffer(i_TradePeriod,PivotOffer,i_TradeBlock,'i_TWDROfferPrice') $ (i_TradePeriodFastTWDROffer(i_TradePeriod,PivotOffer,i_TradeBlock,'i_TWDROfferPrice') >= i_FIRTriggerPrice) = i_PivotPrice;


i_TradePeriodSustainedILROffer(i_TradePeriod,PivotOffer,i_TradeBlock,'i_ILROfferPrice') $ (i_TradePeriodSustainedILROffer(i_TradePeriod,PivotOffer,i_TradeBlock,'i_ILROfferPrice') >= i_SIRTriggerPrice) = i_PivotPrice;

i_TradePeriodFastILROffer(i_TradePeriod,PivotOffer,i_TradeBlock,'i_ILROfferPrice') $ (i_TradePeriodFastILROffer(i_TradePeriod,PivotOffer,i_TradeBlock,'i_ILROfferPrice') >= i_FIRTriggerPrice) = i_PivotPrice;


*If pivot generator is too expensive - should be able to ramp down
i_TradePeriodOfferParameter(i_TradePeriod,PivotOffer,'i_RampDnRate') = 9999;


*Set FKmin for PivotOffers to zero

i_TradePeriodMNodeConstraintRHS(i_TradePeriod,i_MNodeConstraint,'i_ConstraintLimit') $ [i_RemoveFKPivot
                                                                                        and	
                                                                                       (i_TradePeriodMNodeConstraintRHS(i_TradePeriod,i_MNodeConstraint,'i_ConstraintSense') = 1)
                                                                                       and
                                                                                       sum(PivotOffer, i_TradePeriodMNodeEnergyOfferConstraintFactors(i_TradePeriod,i_MNodeConstraint,PivotOffer))] = 0;  
                                     
           




