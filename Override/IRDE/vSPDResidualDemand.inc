
*=====================================================================================
* Name:                 vSPDResidualDemand.inc
* Function:             Residual demand analysis - Offer adjustment
* Developed by:         Electricity Authority, New Zealand
* Source:               https://github.com/djhume/vSPD.git
* Contact:              humed@ea.govt.nz
* Last modified on:     29 January 2014 (code developed by Dr Ramu Naidoo)
*=====================================================================================

$ontext
This code adjusts the offer stack for selected scenario
Specified quantity of the offer stack is moved to a tranch with a specified price

Residual demand curve modelling - Adjust offers for selected trader
Allow for moving (at island or national level):
    a) Percentage of island cleared generation to specified offer price band
    b) MW of island cleared generation to specified offer price band

Steps
    a) Determine the MW amount to be moved (convert the percentage to a MW amount)
    b) loop over all offers for trader
    c) Determine the highest priced tranch of cleared energy
    d) Move the select the MW pf this tranch and move to a higher priced tranch
    e) If more than unit needs to be moved pro-rate the MW amount between the different offers with the same price
    f) If the movement violates the FK min constraint then readjust the FK allocation and recalculate MW to move
    g) If the quantity moved reaches the specified thershold, then exit else continue to move MW

$offtext


*Allow empty data set declaration
$onempty

*Define the trader names - these are mapped to the number names later on
Set
i_TraderName /CTCT, GENE, MERI, MRPL, TRUS, NZAS, Other/

*Defines the set of scenarios for the residual demand analysis
RDAScenario             /Base, MRPL_NI, CTCT_NI, GENE_NI, GENE_SI, MERI_SI, CTCT_SI, TRUS_SI, TRUS_NI/
;



Sets
TraderNameMap(i_Trader,i_TraderName)
TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island)
o_TraderName(i_DateTime,i_TraderName)
ActiveRDAScenario(RDAScenario)
FKTraderIslandMinConstraint(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint)
;

Parameters
TraderIslandOfferedMW(i_TradePeriod,i_Island,i_Trader)
MWToMove(i_TradePeriod,i_Trader)

TraderOfferAdjustment(i_TraderName)

IslandAdjustment(i_Island)
MaxPriceBand(i_TradePeriod,i_Island,i_Trader)
TraderMaxPriceBand(i_TradePeriod,i_Trader)
zeroMaxPrice(i_TradePeriod,i_Island,i_Trader) 
TotMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader)
OfferMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock)
PropOfferAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock)
TotMWMoved(i_TradePeriod,i_Trader)
OfferMWToMove(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock)
RemainMWToMove(i_TradePeriod,i_Trader)
TraderMWMoved(i_TradePeriod,i_Trader,i_Island) 'MW moved for each trader in each island as part of the RDA'
i_TradePeriodEnergyOffer_temp(i_TradePeriod,i_Offer,i_TradeBlock,i_EnergyOfferComponent)
i_TradePeriodEnergyOffer_orig(i_TradePeriod,i_Offer,i_TradeBlock,i_EnergyOfferComponent)

*RDN - Summary reporting for the residual demand analysis
o_NodeGenCost_TP(i_DateTime,i_Node)               'Generator cost - $'
o_IslandPosLoad_TP(i_DateTime,i_Island)           'Positive only load in each island - MW'
o_IslandNegLoad_TP(i_DateTime,i_Island)           'Negative only load in each island - MW'


o_TraderIslandSuppliedMW(i_DateTime,i_TraderName,i_Island)       'MW supplied by each trader in each island - MW'
o_TraderIslandGenRevenenue(i_DateTime,i_TraderName,i_Island)     'Generation revenue of each trader in each island - $'
o_TraderIslandGenCost(i_DateTime,i_TraderName,i_Island)          'Generation cost of each trader in each island - $'
o_TraderIslandOfferedMW(i_DateTime,i_TraderName,i_Island)        'MW offereed by each trader in each island - MW'
o_TraderIslandOfferedPQ(i_DateTime,i_TraderName,i_Island)        'PricexQuantity offereed by each trader in each island for each trading period'
o_TraderQMoved(i_DateTime,i_TraderName,i_Island)                 'Quantity of MW moved of trader offered MW in each island'

o_EnergyOffer_diff(i_DateTime,i_Offer,i_TradeBlock,i_EnergyOfferComponent) 'Difference in the final and original offers'

o_IslandScenarioMW(i_DateTime,i_Island)                  'Scheduled MW from adjusted offers'
o_IslandScenarioPrice(i_DateTime,i_Island)               'Reference price from adjusted offers'
o_IslandBaseMW(i_DateTime,i_Island)                      'Base MW'
o_IslandBasePrice(i_DateTime,i_Island)                   'Base price'

o_IslandAllGen(i_DateTime,i_Island)                      'Island generation for the GWAP calculation'
o_IslandPosLoad(i_DateTime,i_Island)                     'Island load for the GWAP calculation'

o_IslandScenarioGWAP(i_DateTime,i_Island)                'Island scenario GWAP'
o_IslandBaseGWAP(i_DateTime,i_Island)                    'Island base GWAP'

prop_Qchange(i_DateTime,i_Island)                        'Proportion Q change'
o_IRDE_refprice(i_DateTime,i_Island)                     'Residual demand elasticity using reference price'
o_IRDE_gwap(i_DateTime,i_Island)                         'Residual demand elasticity using gwap price'

*RDN - Cleared block generation
o_OfferTradeBlockEnergy_TP(i_DateTime,i_Offer,i_TradeBlock)      'Cleared gen by trade block'
o_ClearedBlockGen(*,i_DateTime,i_Offer,i_TradeBlock)              'Cleared block generation'
OfferAdjustment(i_Offer)                                          'Offer to adjust'

*RDN - Base nodal price
o_BaseNodePrice_TP(i_DateTime,i_Node)                    'Nodal price from the base solve'

o_IslandScenarioPQ(i_DateTime,i_Island)                  'Scenario price*quantity'
o_IslandBasePQ(i_DateTime,i_Island)                      'Base price*quantity'

*RDN - Trade period energy offer - base
o_TradePeriodBaseEnergyOffer(i_DateTime,i_Offer,i_TradeBlock,i_EnergyOfferComponent)
o_Qoffer(i_DateTime,i_Island)
o_PQoffer(i_DateTime,i_Island)
o_QWOP(i_DateTime,i_Island)
o_QWOPcleared(i_DateTime,i_Island)
o_PQoffercleared(i_DateTime,i_Island)
o_maxPcleared(i_DateTime,i_Island)

IsFK(i_TradePeriod,i_Offer)     				'Flag if offer is the frequency keeper in trading period'
IncludeFKOffer(i_TradePeriod,i_Offer) 			'Flag to include frequency keeper in offer stack is included in the calc'
*IncludeFK(i_TradePeriod,i_Trader,i_Island) 		'Flag to include frequency keeper in offer stack is included in the calc'
*FKgen(i_TradePeriod,i_Trader,i_Island) 			'Total unadjusted cleared offers from FKeeper'
*FKMWAtMinorMaxPriceBand(i_TradePeriod,i_Trader,i_Island) 'FK MW at maximum price band'

*FKminGen(i_TradePeriod,i_Trader,i_Island)
*FKOfferMWToMove(i_TradePeriod,i_Trader,i_Island)
*AdjFKOfferMWToMove(i_TradePeriod,i_Trader,i_Island)
FKAdjalloc(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock)
FKAdjprop(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock)
*FKAvailMWToMove(i_TradePeriod,i_Trader,i_Island)

FKgen(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint)
FKminGen(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint)
FKMWAtMinorMaxPriceBand(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint)
FKAvailMWToMove(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint)
FKOfferMWToMove(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint)
AdjFKOfferMWToMove(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint)
IncludeFK(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint)
;

alias(*,dim1);

Scalars
BaseSolve                /0/
PercentageAdjustment     /0/
MWAdjustment             /50/
LoopIteration            /1/
MaxLoopIteration         /50/
TargetPrice              'Target price to use rather than the offer price'                              /20000/
ManualOfferSelect        'Flag to manually select the offers'                                           /0/
i_ExcludeACBranchLimits	 'Exclude AC branch limits'														/1/
;

*Set the active pivot scenario
*ActiveRDAScenario('GENE_NI') = yes;
*ActiveRDAScenario('GENE_SI') = yes;
*ActiveRDAScenario('MRPL_NI') = yes;
ActiveRDAScenario('CTCT_NI') = yes;
*ActiveRDAScenario('MERI_SI') = yes;
*ActiveRDAScenario('CTCT_SI') = yes;
*ActiveRDAScenario('TRUS_SI') = yes;
*ActiveRDAScenario('TRUS_NI') = yes;

ActiveRDAScenario('Base') $ BaseSolve = yes;
ActiveRDAScenario('GENE_NI') $ BaseSolve = no;
ActiveRDAScenario('GENE_SI') $ BaseSolve = no;
ActiveRDAScenario('MRPL_NI') $ BaseSolve = no;
ActiveRDAScenario('CTCT_NI') $ BaseSolve = no;
ActiveRDAScenario('MERI_SI') $ BaseSolve = no;
ActiveRDAScenario('CTCT_SI') $ BaseSolve = no;
ActiveRDAScenario('TRUS_SI') $ BaseSolve = no;
ActiveRDAScenario('TRUS_NI') $ BaseSolve = no;

*Set up mapping to trader names from the trader id
TraderNameMap(i_Trader,i_TraderName) = no;
TraderNameMap(i_Trader,'CTCT') $ sum(sameas(i_Trader,'38433'),1) = yes;
TraderNameMap(i_Trader,'GENE') $ sum(sameas(i_Trader,'38459'),1) = yes;
TraderNameMap(i_Trader,'MERI') $ sum(sameas(i_Trader,'38481'),1) = yes;
TraderNameMap(i_Trader,'MRPL') $ sum(sameas(i_Trader,'38486'),1) = yes;
TraderNameMap(i_Trader,'TRUS') $ sum(sameas(i_Trader,'38546'),1) = yes;

*Use this to map NZAS IR offers to MERI during IL contract
TraderNameMap(i_Trader,'NZAS') $ sum(sameas(i_Trader,'38430'),1) = yes;

TraderNameMap(i_Trader,'Other') $ (not (sum(i_TraderName $ TraderNameMap(i_Trader,i_TraderName), 1))) = yes;

*Initialise some parameters
MWToMove(i_TradePeriod,i_Trader) = 0;
IslandAdjustment(i_Island) = 0;
TraderOfferAdjustment(i_TraderName) = 0;
IslandAdjustment(i_Island) = 0;
OfferAdjustment(i_Offer) = 0;


*++++++++++Load the base cleared offers and base nodal prices if it exists
$if exist "%system.fp%..\..\BASE\IRDE\basePQ\%VSPDInputData%_basePQ_TP.gdx" $GDXIN "%system.fp%..\..\BASE\IRDE\basePQ\%VSPDInputData%_basePQ_TP.gdx"
$If exist "%system.fp%..\..\BASE\IRDE\basePQ\%VSPDInputData%_basePQ_TP.gdx" $LOAD o_OfferTradeBlockEnergy_TP o_BaseNodePrice_TP o_TradePeriodBaseEnergyOffer
*Close the gdx
$If exist "%system.fp%..\..\BASE\IRDE\basePQ\%VSPDInputData%_basePQ_TP.gdx" $GDXIN

*Initialise if data exists
o_OfferTradeBlockEnergy_TP(i_DateTime,i_Offer,i_TradeBlock) $ BaseSolve = 0;
o_BaseNodePrice_TP(i_DateTime,i_Node) $ BaseSolve = 0;
o_TradePeriodBaseEnergyOffer(i_DateTime,i_Offer,i_TradeBlock,i_EnergyOfferComponent) $ BaseSolve = 0;

o_IslandAllGen(i_DateTime,i_Island) $ BaseSolve = 0;
o_IslandPosLoad(i_DateTime,i_Island) $ BaseSolve = 0;
o_IslandBasePrice(i_DateTime,i_Island) $ BaseSolve = 0;
o_IslandBaseGWAP(i_DateTime,i_Island) $ BaseSolve = 0;
*++++++++++Load the base cleared offers and base nodal prices if it exists

*++++++++++Identify the frequency keeper
IsFK(i_TradePeriod,i_Offer) = 0;
IsFK(i_TradePeriod,i_Offer) $ sum(i_MNodeConstraint, i_TradePeriodMNodeEnergyOfferConstraintFactors(i_TradePeriod,i_MNodeConstraint,i_Offer))
 = 1;

IncludeFKOffer(i_TradePeriod,i_Offer) = 1;
IncludeFK(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint) = 1;

*++++++++++Identify the frequency keeper


*++++++++++Map the scenarios to the traders and islands
if(BaseSolve = 0,
*MERI - SI - Start--------------------------------------------------
TraderOfferAdjustment('MERI') $ ActiveRDAScenario('MERI_SI') = yes;

*From 09-Mar-13 to 11-June-13 TWI IL is under MERI control
if (((InputGDXGDate >= 40976) and (InputGDXGDate <= 41070)),
    TraderOfferAdjustment('NZAS') $ ActiveRDAScenario('MERI_SI') = yes;
);
IslandAdjustment('SI') $ ActiveRDAScenario('MERI_SI') = 1;
*MERI - SI - End-----------------------------------------------------

*GENE - NI - Start--------------------------------------------------
TraderOfferAdjustment('GENE') $ ActiveRDAScenario('GENE_NI') = yes;
IslandAdjustment('NI') $ ActiveRDAScenario('GENE_NI') = 1;
*GENE - NI - End-----------------------------------------------------

*GENE - SI - Start--------------------------------------------------
TraderOfferAdjustment('GENE') $ ActiveRDAScenario('GENE_SI') = yes;
IslandAdjustment('SI') $ ActiveRDAScenario('GENE_NI') = 1;
*GENE - SI - End-----------------------------------------------------

*MRPL - NI - Start--------------------------------------------------
TraderOfferAdjustment('MRPL') $ ActiveRDAScenario('MRPL_NI') = yes;
IslandAdjustment('NI') $ ActiveRDAScenario('MRPL_NI') = 1;
*MRPL - NI - End-----------------------------------------------------

*CTCT - NI - Start--------------------------------------------------
TraderOfferAdjustment('CTCT') $ ActiveRDAScenario('CTCT_NI') = yes;
IslandAdjustment('NI') $ ActiveRDAScenario('CTCT_NI') = 1;
*CTCT - NI - End-----------------------------------------------------

*CTCT - SI - Start--------------------------------------------------
TraderOfferAdjustment('CTCT') $ ActiveRDAScenario('CTCT_SI') = yes;
IslandAdjustment('SI') $ ActiveRDAScenario('CTCT_SI') = 1;
*CTCT - SI - End-----------------------------------------------------

*TRUS - SI - Start--------------------------------------------------
TraderOfferAdjustment('TRUS') $ ActiveRDAScenario('TRUS_SI') = yes;
IslandAdjustment('SI') $ ActiveRDAScenario('TRUS_SI') = 1;
*TRUS - SI - End-----------------------------------------------------

*TRUS - NI - Start--------------------------------------------------
TraderOfferAdjustment('TRUS') $ ActiveRDAScenario('TRUS_NI') = yes;
IslandAdjustment('NI') $ ActiveRDAScenario('TRUS_NI') = 1;
*TRUS - SI - End-----------------------------------------------------

*++++++++++Map the scenarios to the traders and islands

*++++++++++Set up the offers to adjust
if (ManualOfferSelect,

    OfferAdjustment('HLY2201 HLY1') = 1;
    OfferAdjustment('HLY2201 HLY2') = 1;
    OfferAdjustment('HLY2201 HLY3') = 1;
    OfferAdjustment('HLY2201 HLY4') = 1;
    OfferAdjustment('HLY2201 HLY5') = 1;
    OfferAdjustment('HLY2201 HLY6') = 1;

else

    OfferAdjustment(i_Offer) $ sum((i_TradePeriod,i_Trader,i_TraderName,i_Island,i_Node,i_Bus) $ (i_TradePeriodOfferTrader(i_TradePeriod,i_Offer,i_Trader)
                                                                                 and TraderNameMap(i_Trader,i_TraderName) and TraderOfferAdjustment(i_TraderName) and IslandAdjustment(i_Island)
                                                                                 and i_TradePeriodOfferNode(i_TradePeriod,i_Offer,i_Node)
                                                                                 and i_TradePeriodNodeBus(i_TradePeriod,i_Node,i_Bus)
                                                                                 and i_TradePeriodBusIsland(i_TradePeriod,i_Bus,i_Island)), 1) = yes;
);

*++++++++++Set up the offers to adjust

*++++++++++Determine mapping of offer to trader and island
TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island) = no;
TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island) $ sum((i_Node,i_Bus) $ (i_TradePeriodOfferTrader(i_TradePeriod,i_Offer,i_Trader) 
																					  and i_TradePeriodOfferNode(i_TradePeriod,i_Offer,i_Node) 
																					  and i_TradePeriodNodeBus(i_TradePeriod,i_Node,i_Bus) 
																					  and i_TradePeriodBusIsland(i_TradePeriod,i_Bus,i_Island) 
																					  and i_TradePeriodNodeBusAllocationFactor(i_TradePeriod,i_Node,i_Bus)
																					  ),1) = yes;

*Initialise some parameters
MaxPriceBand(i_TradePeriod,i_Island,i_Trader) = -99999;
TraderMaxPriceBand(i_TradePeriod,i_Trader) = 0;
TotMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader) = 0;
OfferMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) = 0;
PropOfferAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) = 0;
RemainMWToMove(i_TradePeriod,i_Trader) = 0;
TotMWMoved(i_TradePeriod,i_Trader) = 0;
OfferMWToMove(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) = 0;
TraderMWMoved(i_TradePeriod,i_Trader,i_Island) = 0;

*Update the energy offer to the cleared offer from the base solve for the relevant trader
i_TradePeriodEnergyOffer(i_TradePeriod,i_Offer,i_TradeBlock,'i_GenerationMWOffer')
   $ ((sum((i_Island,i_Trader,i_TraderName) $ (TraderOfferAdjustment(i_TraderName)
                                       and IslandAdjustment(i_Island)
                                       and TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island)
                                       and TraderNameMap(i_Trader,i_TraderName)
                                       and i_TradePeriodOfferTrader(i_TradePeriod,i_Offer,i_Trader)
                                       ), 1)))
    = sum(i_DateTime $ i_DateTimeTradePeriodMap(i_DateTime,i_TradePeriod), o_OfferTradeBlockEnergy_TP(i_DateTime,i_Offer,i_TradeBlock));

i_TradePeriodEnergyOffer_temp(i_TradePeriod,i_Offer,i_TradeBlock,i_EnergyOfferComponent) = i_TradePeriodEnergyOffer(i_TradePeriod,i_Offer,i_TradeBlock,i_EnergyOfferComponent);
i_TradePeriodEnergyOffer_orig(i_TradePeriod,i_Offer,i_TradeBlock,i_EnergyOfferComponent) = i_TradePeriodEnergyOffer(i_TradePeriod,i_Offer,i_TradeBlock,i_EnergyOfferComponent);

*Calculate the total MW offered by each trader in each island during each trading period
TraderIslandOfferedMW(i_TradePeriod,i_Island,i_Trader) = sum((i_Offer,i_TradeBlock) $ (OfferAdjustment(i_Offer) and TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island)), i_TradePeriodEnergyOffer(i_TradePeriod,i_Offer,i_TradeBlock,'i_GenerationMWOffer'));

MWToMove(i_TradePeriod,i_Trader) $ (sum(i_TraderName $ TraderNameMap(i_Trader,i_TraderName), TraderOfferAdjustment(i_TraderName)) and PercentageAdjustment) = (PercentageAdjustment/100) * sum(i_Island $ IslandAdjustment(i_Island), TraderIslandOfferedMW(i_TradePeriod,i_Island,i_Trader));

MWToMove(i_TradePeriod,i_Trader) $ (sum(i_TraderName $ TraderNameMap(i_Trader,i_TraderName), TraderOfferAdjustment(i_TraderName)) and MWAdjustment) = min(MWAdjustment, sum(i_Island $ IslandAdjustment(i_Island), TraderIslandOfferedMW(i_TradePeriod,i_Island,i_Trader)));

*Loop until the required MW are moved to the higher price tranch
loop((i_TradePeriod,i_Trader) $ [sum(i_TraderName $ TraderNameMap(i_Trader,i_TraderName), TraderOfferAdjustment(i_TraderName))],

    LoopIteration = 1;
    RemainMWToMove(i_TradePeriod,i_Trader) = MWToMove(i_TradePeriod,i_Trader);

     while((RemainMWToMove(i_TradePeriod,i_Trader)>0) and (LoopIteration <= MaxLoopIteration),


          MaxPriceBand(i_TradePeriod,i_Island,i_Trader) = 0;
          TraderMaxPriceBand(i_TradePeriod,i_Trader) = 0;
          zeroMaxPrice(i_TradePeriod,i_Island,i_Trader) = 0;
		  
          TotMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader) = 0;
          OfferMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) = 0;
          PropOfferAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) = 0;
          OfferMWToMove(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) =  0;
          TotMWMoved(i_TradePeriod,i_Trader) = 0;

          MaxPriceBand(i_TradePeriod,i_Island,i_Trader) $ TraderIslandOfferedMW(i_TradePeriod,i_Island,i_Trader) 
			= smax((i_Offer,i_TradeBlock) $ (OfferAdjustment(i_Offer) 
											 and TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island) 
											 and i_TradePeriodEnergyOffer_temp(i_TradePeriod,i_Offer,i_TradeBlock,'i_GenerationMWOffer') 
											 and (ord(i_TradeBlock)<20)
											 and IncludeFKOffer(i_TradePeriod,i_Offer))
											 , i_TradePeriodEnergyOffer_temp(i_TradePeriod,i_Offer,i_TradeBlock,'i_GenerationMWOfferPrice'));

          zeroMaxPrice(i_TradePeriod,i_Island,i_Trader) $ [TraderIslandOfferedMW(i_TradePeriod,i_Island,i_Trader) 
														   and (MaxPriceBand(i_TradePeriod,i_Island,i_Trader) = 0)
														  ] = 1; 

*Do this to set the max price when zero since 0 does not seem to work in the ffg smax
          MaxPriceBand(i_TradePeriod,i_Island,i_Trader) $ [TraderIslandOfferedMW(i_TradePeriod,i_Island,i_Trader)
														  and (zeroMaxPrice(i_TradePeriod,i_Island,i_Trader) = 1)]	
			= -1;
														  
          TraderMaxPriceBand(i_TradePeriod,i_Trader) $ MWToMove(i_TradePeriod,i_Trader) 
			= smax(i_Island $ [IslandAdjustment(i_Island)
							  ], MaxPriceBand(i_TradePeriod,i_Island,i_Trader));          

          							  
	  
		  OfferMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) $ (OfferAdjustment(i_Offer) 
																					and (ord(i_TradeBlock) < 20) 
																					and MWToMove(i_TradePeriod,i_Trader)
																					) = sum(i_Island $ (IslandAdjustment(i_Island) 
																										and TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island)
																										and (i_TradePeriodEnergyOffer_temp(i_TradePeriod,i_Offer,i_TradeBlock,'i_GenerationMWOfferPrice') = TraderMaxPriceBand(i_TradePeriod,i_Trader)) 
																										and IncludeFKOffer(i_TradePeriod,i_Offer)
																										), i_TradePeriodEnergyOffer_temp(i_TradePeriod,i_Offer,i_TradeBlock,'i_GenerationMWOffer'));


          OfferMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) $ (OfferAdjustment(i_Offer) 
																					and (ord(i_TradeBlock) < 20) 
																					and MWToMove(i_TradePeriod,i_Trader)
																					and (TraderMaxPriceBand(i_TradePeriod,i_Trader) = -1)				
																					) = sum(i_Island $ (IslandAdjustment(i_Island) 
																										and TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island)
																										and IncludeFKOffer(i_TradePeriod,i_Offer)
																										), i_TradePeriodEnergyOffer_temp(i_TradePeriod,i_Offer,i_TradeBlock,'i_GenerationMWOffer'));


*display MaxPriceBand, TraderMaxPriceBand, ZeroMaxPrice, OfferMWAtMinOrMaxPriceBand;

          TotMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader) $ (MWToMove(i_TradePeriod,i_Trader)) 
			= sum((i_Offer,i_Island,i_TradeBlock) $ (OfferAdjustment(i_Offer) 
													and IslandAdjustment(i_Island) 
													and TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island) 
													and IncludeFKOffer(i_TradePeriod,i_Offer)
													), OfferMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock));


          PropOfferAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) $ (OfferAdjustment(i_Offer) 
																					   and TotMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader) 
																					   and (ord(i_TradeBlock) < 20)
																					   ) = OfferMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock)/TotMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader);

*If TotMWAtMinOrMaxPriceBand <= Remaining MW to move, then move the entire quantity to the maximum price band otherwise limit the amount to the remaining MW to move
          TotMWMoved(i_TradePeriod,i_Trader) = min(TotMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader),RemainMWToMove(i_TradePeriod,i_Trader));

*Pro-rate the MW to move by offer depending on the proportion of its MW at the total minimum offer price
          OfferMWToMove(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) $ OfferAdjustment(i_Offer) =  TotMWMoved(i_TradePeriod,i_Trader) * PropOfferAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock);


*If no offers to move then exit		  
		  LoopIteration $ (TotMWMoved(i_TradePeriod,i_Trader) = 0) = MaxLoopIteration; 
		  
*display OfferMWToMove;		  
		  
*+++++++++This section ensures that the minimum generation of the fkeeper is not adjusted below its minimum requirement

		  FKgen(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint) = sum((i_Offer,i_TradeBlock) $ [TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island) 
																								   and i_TradePeriodMNodeEnergyOfferConstraintFactors(i_TradePeriod,i_MNodeConstraint,i_Offer)
																								   and (ord(i_TradeBlock) < 20)
																								   ], i_TradePeriodEnergyOffer_temp(i_TradePeriod,i_Offer,i_TradeBlock,'i_GenerationMWOffer'));
																				 
          FKTraderIslandMinConstraint(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint) $ [(i_TradePeriodMNodeConstraintRHS(i_TradePeriod,i_MNodeConstraint,'i_ConstraintSense') = 1) and sum(i_Offer $ TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island), i_TradePeriodMNodeEnergyOfferConstraintFactors(i_TradePeriod,i_MNodeConstraint,i_Offer))]
           = yes;
		   
          FKminGen(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint) $ FKTraderIslandMinConstraint(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint) 
		     = i_TradePeriodMNodeConstraintRHS(i_TradePeriod,i_MNodeConstraint,'i_ConstraintLimit');
		   
 		  FKMWAtMinorMaxPriceBand(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint) = sum((i_Offer,i_TradeBlock) $ [TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island) 
																													 and i_TradePeriodMNodeEnergyOfferConstraintFactors(i_TradePeriod,i_MNodeConstraint,i_Offer)
																													], OfferMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock));
																			   
																		   
*This is the available MW that can be moved          
		  FKAvailMWToMove(FKTraderIslandMinConstraint) = FKgen(FKTraderIslandMinConstraint) - FKminGen(FKTraderIslandMinConstraint);
		  
*This is the offered MW that should be moved		  
		  FKOfferMWToMove(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint) $ FKTraderIslandMinConstraint(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint) 
				= sum((i_Offer,i_TradeBlock) $ [TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island) 
											   and i_TradePeriodMNodeEnergyOfferConstraintFactors(i_TradePeriod,i_MNodeConstraint,i_Offer)
												], OfferMWToMove(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock));

																											
*If the offered MW that should be moved exceeds the available MW that can be moved then exclude the FK from future calcs - at the min gen
		  IncludeFK(FKTraderIslandMinConstraint) $ [FKOfferMWToMove(FKTraderIslandMinConstraint) >= FKAvailMWToMove(FKTraderIslandMinConstraint)] = 0;

		  
*Exclude the offers mapped as the Fkeeper if the IncludeFK is set to zero
          IncludeFKOffer(i_TradePeriod,i_Offer) $ sum((i_Island,i_MNodeConstraint) $ (TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island)
																					  and FKTraderIslandMinConstraint(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint)
																					  and i_TradePeriodMNodeEnergyOfferConstraintFactors(i_TradePeriod,i_MNodeConstraint,i_Offer)
																					  ), 1-IncludeFK(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint)) = 0;	  

*display FKAvailMWToMove, FKOfferMWToMove, IncludeFK, IncludeFKOffer;
																					  
*Adjust the offered MW that can be moved - limited to the available MW that can be moved
		  AdjFKOfferMWToMove(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint)
            = max(0,min(FKAvailMWToMove(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint), FKOfferMWToMove(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint)));

			
*Now if there are multiple offers that need adjustment
*Determine the proportion of the adjusted FK offer movement if need to allocate to several FK offers
		  FKAdjprop(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) = 0;		 
		  FKAdjprop(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) $ sum((i_Island,i_MNodeConstraint) $ (TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island)
																									   and i_TradePeriodMNodeEnergyOfferConstraintFactors(i_TradePeriod,i_MNodeConstraint,i_Offer) 
																						               ), FKOfferMWToMove(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint)) 
		    = OfferMWToMove(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) / sum((i_Island,i_MNodeConstraint) $ (TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island)
																											   and i_TradePeriodMNodeEnergyOfferConstraintFactors(i_TradePeriod,i_MNodeConstraint,i_Offer) 
																											   ), FKOfferMWToMove(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint));
			
*Determine the adjusted allocation
 		  FKAdjalloc(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) = 0;		 
		  FKAdjalloc(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) $ sum((i_Island,i_MNodeConstraint) $ (TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island)
																									   and i_TradePeriodMNodeEnergyOfferConstraintFactors(i_TradePeriod,i_MNodeConstraint,i_Offer) 
																						               ), FKOfferMWToMove(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint)) 
		    = FKAdjprop(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) * sum((i_Island,i_MNodeConstraint) $ (TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island)
																									       and i_TradePeriodMNodeEnergyOfferConstraintFactors(i_TradePeriod,i_MNodeConstraint,i_Offer) 
																						                   ), AdjFKOfferMWToMove(i_TradePeriod,i_Trader,i_Island,i_MNodeConstraint));

*display FKAdjprop, IncludeFKOffer, IncludeFK;

*+++++++++This section ensures that the minimum generation of the fkeeper is not adjusted below its minimum requirement
	
*+++++++++++++++++++++++++++++++++++++++++++++++++Update with the FK allocation 
     
*Reassign the offer MW of FKeepers at their limit	  
		  OfferMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) $ (OfferAdjustment(i_Offer) 
		                                                                             and (ord(i_TradeBlock) < 20) 
																					 and MWToMove(i_TradePeriod,i_Trader)
																					 and (IncludeFKOffer(i_TradePeriod,i_Offer) = 0)
																					 ) = FKAdjalloc(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock);
	
	      TotMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader) $ (MWToMove(i_TradePeriod,i_Trader)) = sum((i_Offer,i_Island,i_TradeBlock) $ (OfferAdjustment(i_Offer) 
		                                                                                                                                 and IslandAdjustment(i_Island) 
																																		 and TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island) 
																																		 ), OfferMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock));


          PropOfferAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) $ (OfferAdjustment(i_Offer) and TotMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader) and (ord(i_TradeBlock) < 20)) = OfferMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock)/TotMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader);

*If TotMWAtMinOrMaxPriceBand <= Remaining MW to move, then move the entire quantity to the maximum price band otherwise limit the amount to the remaining MW to move
          TotMWMoved(i_TradePeriod,i_Trader) = min(TotMWAtMinOrMaxPriceBand(i_TradePeriod,i_Trader),RemainMWToMove(i_TradePeriod,i_Trader));

*Pro-rate the MW to move by offer depending on the proportion of its MW at the total minimum offer price
          OfferMWToMove(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) $ OfferAdjustment(i_Offer) =  TotMWMoved(i_TradePeriod,i_Trader) * PropOfferAtMinOrMaxPriceBand(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock);

*+++++++++++++++++++++++++++++++++++++++++++++++++Update with the FK allocation 	

		  i_TradePeriodEnergyOffer_temp(i_TradePeriod,i_Offer,i_TradeBlock,'i_GenerationMWOffer') $ (OfferAdjustment(i_Offer) 
																									 and OfferMWToMove(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock) 
																									 ) = max(0,(i_TradePeriodEnergyOffer_temp(i_TradePeriod,i_Offer,i_TradeBlock,'i_GenerationMWOffer') - OfferMWToMove(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock)));

          i_TradePeriodEnergyOffer_temp(i_TradePeriod,i_Offer,'t20','i_GenerationMWOfferPrice') $ (OfferAdjustment(i_Offer) 
																								   and sum(i_TradeBlock, OfferMWToMove(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock))
																								   ) = TargetPrice;

          i_TradePeriodEnergyOffer_temp(i_TradePeriod,i_Offer,'t20','i_GenerationMWOffer') $ (OfferAdjustment(i_Offer) 
																							  and sum(i_TradeBlock, OfferMWToMove(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock))
																							  ) = i_TradePeriodEnergyOffer_temp(i_TradePeriod,i_Offer,'t20','i_GenerationMWOffer') + sum(i_TradeBlock, OfferMWToMove(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock));

         RemainMWToMove(i_TradePeriod,i_Trader) = RemainMWToMove(i_TradePeriod,i_Trader) - TotMWMoved(i_TradePeriod,i_Trader);
	 
         TraderMWMoved(i_TradePeriod,i_Trader,i_Island) = TraderMWMoved(i_TradePeriod,i_Trader,i_Island) + sum((i_Offer,i_TradeBlock) $ (OfferAdjustment(i_Offer) and TraderOfferIslandMap(i_TradePeriod,i_Trader,i_Offer,i_Island)), OfferMWToMove(i_TradePeriod,i_Trader,i_Offer,i_TradeBlock));

         LoopIteration = LoopIteration + 1;

*display RemainMWToMove, TraderMWMoved;

*End while
     );

*End loop
);

*RDN - Update the energy offer if the TradeOfferAdjustment flag is set
i_TradePeriodEnergyOffer(i_TradePeriod,i_Offer,i_TradeBlock,i_EnergyOfferComponent) $ (OfferAdjustment(i_Offer) and sum(i_TraderName, TraderOfferAdjustment(i_TraderName))) = i_TradePeriodEnergyOffer_temp(i_TradePeriod,i_Offer,i_TradeBlock,i_EnergyOfferComponent);


*Clear the temp energy offer file
option clear = i_TradePeriodEnergyOffer_temp;


);

*execute_unload 'RevisedOffer.gdx', i_TradePeriodEnergyOffer;

