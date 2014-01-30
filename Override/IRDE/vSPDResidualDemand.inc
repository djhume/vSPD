*=====================================================================================
* Name:                 vSPDResidualDemand.inc
* Function:             Residual demand analysis - Offer adjustment
* Developed by:         Electricity Authority, New Zealand
* Source:               https://github.com/djhume/vSPD.git
* Contact:              humed@ea.govt.nz
* Last modified on:     31 January 2014 D J Hume (code developed by Dr Ramu Naidoo)
*=====================================================================================

$ontext
NOTES:

Page 92 onwards in the Wolak report provides a description of the "inverse elasticity
residual demand" curve.

Wolak's example involves Meridian, who, on a 15% reduction of their total MW offer band
are able to increase price a whopping 108%.

This is termed the inverse elasticity and is a key determinant of how far above its
marginal cost (in percentage terms) a profit-maximizing supplier would like its offer
curve to intersect with its residual demand curve. Specifically, the larger the value
of the inverse  of the elasticity of the residual demand curve, the greater is the
percentage a profit-maximizing supplier would like the market price to be above its
marginal cost.

For the example above the inverse elasticity is 108/15 = 7.2.  

Wolak notes that investigating this gives set quantity/price pairs that could (in
imperfect -or illiquid markets) be used to "price to take what competition gives them"
or "pricing at what the market will bear"... obviously to be able to do this and want
to do this are dependent on a generators net pivotal status.

This work, created by Ramu is to be used (by me) to model this index for every trading
period and for each participant.  This will give a time series of lerner index for each
participant.  

This code adjusts the offer stack for the selected scenario, basically a specified
quantity of the offer stack is moved to a tranch with a specified price.

It allows for moving (at island or national level):
    a) Percentage of island cleared generation to specified offer price band
    b) MW of island cleared generation to specified offer price band

This turned out more complicated than first anticipated.... and required the following:

    a) Determine the MW amount to be moved (convert the percentage to a MW amount)
    b) loop over all offers for trader
    c) Determine the highest priced tranch of cleared energy
    d) Move the select the MW pf this tranch and move to a higher priced tranch
    e) If more than unit needs to be moved pro-rate the MW amount between the different
       offers with the same price
    f) If the movement violates the FK min constraint then readjust the FK allocation
       and recalculate MW to move
    g) If the quantity moved reaches the specified thershold, then exit else continue
       to move MW

Phew!

To acheive this reqiures a dual solve; 
        1) the first to report cleared gneration per tranch (could get this from the DW); 
        2) the second to then manipulate the offer curve by setting the last bit of the
           offer curve to a high price.

So the following is current done (pretty adhoc) 
    1. Base case run:
        (a) edit vSPDpaths.inc to setglobal runname and paths as required
        (b) edit vSPDsettings.inc to set global: i_SequentialSolve (Vectorisation)
                 and PivotAnalysis flags
        (c) edit IRDE/vSPDResidualDemand.inc (this file) to set: BaseSolve to 1 

    2. Pivot IRDE analysis:
        (a) edit vSPDpaths.inc to change the global runname
        (b) edit IRDE/vSPDResidualDemand.inc to set: BaseSolve/Percentageadj/etc see above
            flags, also to set the active pivot scenario (see above). 

Once this is achieved look at results: in particular we're interested in the csv file:
       *IldResDemandResults_TP.csv
Output of the base solve is placed in the /Programs/IRDE/basePQ/ dir where it is read for
the non-base, or IRDE solve.

In terms of multiple solves - need to edit vSPDSolve.gms to solve all of a day, over
multiple files, then there are the 5 options above - also shouldn't we be doing this
nationally too?...

In vSPDSolve.gms: overrides begin ~line 500

$offtext
*=====================================================================================

*Allow empty data set declaration
$onempty

*Define the trader names - these are mapped to the number names later on
Set
trdrName /CTCT, GENE, MERI, MRPL, TRUS, NZAS, Other/

*Defines the set of scenarios for the residual demand analysis
RDAScnr             /Base, MRPL_NI, CTCT_NI, GENE_NI, GENE_SI, MERI_SI, CTCT_SI, TRUS_SI, TRUS_NI/
;

*=====================================================================================
Sets
trdrNameMap(trdr,trdrName)
trdrOfferIldMap(tp,trdr,o,ild)
o_trdrName(dt,trdrName)
ActRDAScnr(RDAScnr)
FKtrdrIldMinCstr(tp,trdr,ild,MnodeCstr)
;

*=====================================================================================
Parameters
trdrIldOfferedMW(tp,ild,trdr)
MWToMove(tp,trdr)
trdrOfferadj(trdrName)
Ildadj(ild)
MaxPB(tp,ild,trdr)
trdrMaxPB(tp,trdr)
zeroMaxPrice(tp,ild,trdr) 
TotMWMinMaxPB(tp,trdr)
o_MWMinMaxPB(tp,trdr,o,trdBlk)
PropOfferMinMaxPB(tp,trdr,o,trdBlk)
TotMWMoved(tp,trdr)
o_MWToMove(tp,trdr,o,trdBlk)
RemainMWToMove(tp,trdr)
trdrMWMoved(tp,trdr,ild) 'MW moved for each trader in each island as part of the RDA'
tpEnergyOffer_temp(tp,o,trdBlk,NRGofrCmpnt)
tpEnergyOffer_orig(tp,o,trdBlk,NRGofrCmpnt)
o_NodeGenCost_TP(dt,n)                                 'Generator cost - $'
o_IldPosLoad_TP(dt,ild)                             'Positive only load in each island - MW'
o_IldNegLoad_TP(dt,ild)                             'Negative only load in each island - MW'
o_trdrIldSuppliedMW(dt,trdrName,ild)                'MW supplied by each trader in each island - MW'
o_trdrIldGenRevenenue(dt,trdrName,ild)              'Generation revenue of each trader in each island - $'
o_trdrIldGenCost(dt,trdrName,ild)                   'Generation cost of each trader in each island - $'
o_trdrIldOfferedMW(dt,trdrName,ild)                 'MW offereed by each trader in each island - MW'
o_trdrIldOfferedPQ(dt,trdrName,ild)                 'PricexQuantity offereed by each trader in each island for each trading period'
o_trdrQMoved(dt,trdrName,ild)                          'Quantity of MW moved of trader offered MW in each island'
o_EnergyOffer_diff(dt,o,trdBlk,NRGofrCmpnt) 'Difference in the final and original offers'
o_IldScnrMW(dt,ild)                             'Scheduled MW from adjusted offers'
o_IldScnrPrice(dt,ild)                          'Reference price from adjusted offers'
o_IldBaseMW(dt,ild)                                 'Base MW'
o_IldBasePrice(dt,ild)                              'Base price'
o_IldAllGen(dt,ild)                                 'Island generation for the GWAP calculation'
o_IldPosLoad(dt,ild)                                'Island load for the GWAP calculation'
o_IldScnrGWAP(dt,ild)                           'Island scenario GWAP'
o_IldBaseGWAP(dt,ild)                               'Island base GWAP'
prop_Qchange(dt,ild)                                   'Proportion Q change'
o_IRDE_refprice(dt,ild)                                'Residual demand elasticity using reference price'
o_IRDE_gwap(dt,ild)                                    'Residual demand elasticity using gwap price'
o_OfferTradeBlockEnergy_TP(dt,o,trdBlk)                'Cleared gen by trade block'
o_ClearedBlockGen(*,dt,o,trdBlk)                       'Cleared block generation'
Offeradj(o)                                            'Offer to adjust'
o_BaseNodePrice_TP(dt,n)                               'Nodal price from the base solve'
o_IldScnrPQ(dt,ild)                             'Scenario price*quantity'
o_IldBasePQ(dt,ild)                                 'Base price*quantity'
o_TradePeriodBaseEnergyOffer(dt,o,trdBlk,NRGofrCmpnt)
o_Qoffer(dt,ild)
o_PQoffer(dt,ild)
o_QWOP(dt,ild)
o_QWOPcleared(dt,ild)
o_PQoffercleared(dt,ild)
o_maxPcleared(dt,ild)
IsFK(tp,o)                                             'Flag if offer is the frequency keeper in trading period'  
IncludeFKOffer(tp,o)                                   'Flag to include frequency keeper in offer stack is included in the calc'
*IncludeFK(tp,trdr,ild)                                'Flag to include frequency keeper in offer stack is included in the calc'
*FKgen(tp,trdr,ild)                                    'Total unadjusted cleared offers from FKeeper'
*FKMWAtMinorMaxPB(tp,trdr,ild)                  'FK MW at maximum price band'
*FKminGen(tp,trdr,ild)
*FKo_MWToMove(tp,trdr,ild)
*AdjFKo_MWToMove(tp,trdr,ild)
FKAdjalloc(tp,trdr,o,trdBlk)
FKAdjprop(tp,trdr,o,trdBlk)
*FKAvailMWToMove(tp,trdr,ild)
FKgen(tp,trdr,ild,MnodeCstr)
FKminGen(tp,trdr,ild,MnodeCstr)
FKMWAtMinorMaxPB(tp,trdr,ild,MnodeCstr)
FKAvailMWToMove(tp,trdr,ild,MnodeCstr)
FKo_MWToMove(tp,trdr,ild,MnodeCstr)
AdjFKo_MWToMove(tp,trdr,ild,MnodeCstr)
IncludeFK(tp,trdr,ild,MnodeCstr)
;

*=====================================================================================
alias(*,dim1);

*=====================================================================================
Scalars
BaseSolve          'Base solve, or offer adjustment solve flag, 0 = base, 1 = adj'                 /0/
Percentageadj      'Percentage adjustment if using'                                                /0/
MWadj              'MW adjustment if using'                                                        /50/
LoopIteration      'Loop iteration - loops required to work out adjustment - I think - DJH'        /1/
MaxLoopIteration   'Map loop iteration - ie, dont take too long about this - I think - DJH'        /50/
Upwardadj          'Flag to indicate if adjusted prices moved to higher price band'                /1/
UseTargetPrice     'Flag to use the target price rather than the offer price for the adjusted MW'  /1/
TargetPrice        'Target price to use rather than the offer price'                               /20000/
ManualOfferSelect  'Flag to manually select the offers'                
;

*=====================================================================================
*Set the active pivot scenario
*ActRDAScnr('GENE_NI') = yes;
*ActRDAScnr('GENE_SI') = yes;
*ActRDAScnr('MRPL_NI') = yes;
ActRDAScnr('CTCT_NI') = yes;
*ActRDAScnr('MERI_SI') = yes;
*ActRDAScnr('CTCT_SI') = yes;
*ActRDAScnr('TRUS_SI') = yes;
*ActRDAScnr('TRUS_NI') = yes;

ActRDAScnr('Base') $ BaseSolve = yes;
ActRDAScnr('GENE_NI') $ BaseSolve = no;
ActRDAScnr('GENE_SI') $ BaseSolve = no;
ActRDAScnr('MRPL_NI') $ BaseSolve = no;
ActRDAScnr('CTCT_NI') $ BaseSolve = no;
ActRDAScnr('MERI_SI') $ BaseSolve = no;
ActRDAScnr('CTCT_SI') $ BaseSolve = no;
ActRDAScnr('TRUS_SI') $ BaseSolve = no;
ActRDAScnr('TRUS_NI') $ BaseSolve = no;

*=====================================================================================
*Set up mapping to trader names from the trader id
trdrNameMap(trdr,trdrName) = no;
trdrNameMap(trdr,'CTCT') $ sum(sameas(trdr,'38433'),1) = yes;
trdrNameMap(trdr,'GENE') $ sum(sameas(trdr,'38459'),1) = yes;
trdrNameMap(trdr,'MERI') $ sum(sameas(trdr,'38481'),1) = yes;
trdrNameMap(trdr,'MRPL') $ sum(sameas(trdr,'38486'),1) = yes;
trdrNameMap(trdr,'TRUS') $ sum(sameas(trdr,'38546'),1) = yes;

*Use this to map NZAS IR offers to MERI during IL contract
trdrNameMap(trdr,'NZAS') $ sum(sameas(trdr,'38430'),1) = yes;

trdrNameMap(trdr,'Other') $ (not (sum(trdrName $ trdrNameMap(trdr,trdrName), 1))) = yes;

*Initialise some parameters
MWToMove(tp,trdr) = 0;
Ildadj(ild) = 0;
trdrOfferadj(trdrName) = 0;
Ildadj(ild) = 0;
Offeradj(o) = 0;

*=====================================================================================
*Load the base cleared offers and base nodal prices if it exists
$if exist "%system.fp%..\..\BASE\IRDE\basePQ\%VSPDInputData%_basePQ_TP.gdx" $GDXIN "%system.fp%..\..\BASE\IRDE\basePQ\%VSPDInputData%_basePQ_TP.gdx"
$If exist "%system.fp%..\..\BASE\IRDE\basePQ\%VSPDInputData%_basePQ_TP.gdx" $LOAD o_OfferTradeBlockEnergy_TP o_BaseNodePrice_TP o_TradePeriodBaseEnergyOffer
*Close the gdx
$If exist "%system.fp%..\..\BASE\IRDE\basePQ\%VSPDInputData%_basePQ_TP.gdx" $GDXIN

*Initialise if data exists
o_OfferTradeBlockEnergy_TP(dt,o,trdBlk) $ BaseSolve = 0;
o_BaseNodePrice_TP(dt,n) $ BaseSolve = 0;
o_TradePeriodBaseEnergyOffer(dt,o,trdBlk,NRGofrCmpnt) $ BaseSolve = 0;

o_IldAllGen(dt,ild) $ BaseSolve = 0;
o_IldPosLoad(dt,ild) $ BaseSolve = 0;
o_IldBasePrice(dt,ild) $ BaseSolve = 0;
o_IldBaseGWAP(dt,ild) $ BaseSolve = 0;

*=====================================================================================
*Identify the frequency keeper
IsFK(tp,o) = 0;
IsFK(tp,o) $ sum(MnodeCstr, tpMNodeEnergyOfferCstrFactors(tp,MnodeCstr,o))
 = 1;
IncludeFKOffer(tp,o) = 1;
IncludeFK(tp,trdr,ild,MnodeCstr) = 1;

*=====================================================================================
*Map the scenarios to the traders and islands
if(BaseSolve = 0,
*MERI - SI
*=====================================================================================
trdrOfferadj('MERI') $ ActRDAScnr('MERI_SI') = yes;

*From 09-Mar-13 to 11-June-13 TWI IL is under MERI control
if (((InputGDXGDate >= 40976) and (InputGDXGDate <= 41070)),
    trdrOfferadj('NZAS') $ ActRDAScnr('MERI_SI') = yes;
);
Ildadj('SI') $ ActRDAScnr('MERI_SI') = 1;
*=====================================================================================
*GENE - NI
trdrOfferadj('GENE') $ ActRDAScnr('GENE_NI') = yes;
Ildadj('NI') $ ActRDAScnr('GENE_NI') = 1;
*=====================================================================================
*GENE - SI
trdrOfferadj('GENE') $ ActRDAScnr('GENE_SI') = yes;
Ildadj('SI') $ ActRDAScnr('GENE_NI') = 1;
*=====================================================================================
*MRPL - NI
trdrOfferadj('MRPL') $ ActRDAScnr('MRPL_NI') = yes;
Ildadj('NI') $ ActRDAScnr('MRPL_NI') = 1;
*=====================================================================================
*CTCT - NI
trdrOfferadj('CTCT') $ ActRDAScnr('CTCT_NI') = yes;
Ildadj('NI') $ ActRDAScnr('CTCT_NI') = 1;
*=====================================================================================
*CTCT - SI
trdrOfferadj('CTCT') $ ActRDAScnr('CTCT_SI') = yes;
Ildadj('SI') $ ActRDAScnr('CTCT_SI') = 1;
*=====================================================================================
*TRUS - SI
trdrOfferadj('TRUS') $ ActRDAScnr('TRUS_SI') = yes;
Ildadj('SI') $ ActRDAScnr('TRUS_SI') = 1;
*=====================================================================================
*TRUS - NI
trdrOfferadj('TRUS') $ ActRDAScnr('TRUS_NI') = yes;
Ildadj('NI') $ ActRDAScnr('TRUS_NI') = 1;
*=====================================================================================
*Set up the offers to adjust
if (ManualOfferSelect,
    Offeradj('HLY2201 HLY1') = 1;
    Offeradj('HLY2201 HLY2') = 1;
    Offeradj('HLY2201 HLY3') = 1;
    Offeradj('HLY2201 HLY4') = 1;
    Offeradj('HLY2201 HLY5') = 1;
    Offeradj('HLY2201 HLY6') = 1;
else
    Offeradj(o) $ sum((tp,trdr,trdrName,ild,n,i_Bus) $ (tpOffertrdr(tp,o,trdr)
                       and trdrNameMap(trdr,trdrName) and trdrOfferadj(trdrName) and Ildadj(ild)
                       and tpOfferNode(tp,o,n)
                       and tpNodeBus(tp,n,i_Bus)
                       and tpBusIld(tp,i_Bus,ild)), 1) = yes;
);

*=====================================================================================
*Determine mapping of offer to trader and island
trdrOfferIldMap(tp,trdr,o,ild) = no;
trdrOfferIldMap(tp,trdr,o,ild) $ sum((n,i_Bus) $ (tpOffertrdr(tp,o,trdr) 
                                         and tpOfferNode(tp,o,n) 
                                         and tpNodeBus(tp,n,i_Bus) 
                                         and tpBusIld(tp,i_Bus,ild) 
                                         and tpNodeBusAllocationFactor(tp,n,i_Bus)),1) = yes;

*=====================================================================================
*Initialise some parameters
MaxPB(tp,ild,trdr) = -99999;
trdrMaxPB(tp,trdr) = 0;
TotMWMinMaxPB(tp,trdr) = 0;
o_MWMinMaxPB(tp,trdr,o,trdBlk) = 0;
PropOfferMinMaxPB(tp,trdr,o,trdBlk) = 0;
RemainMWToMove(tp,trdr) = 0;
TotMWMoved(tp,trdr) = 0;
o_MWToMove(tp,trdr,o,trdBlk) = 0;
trdrMWMoved(tp,trdr,ild) = 0;

*=====================================================================================
*Update the energy offer to the cleared offer from the base solve for the relevant trader
tpEnergyOffer(tp,o,trdBlk,'i_GenerationMWOffer')
   $ ((sum((ild,trdr,trdrName) $ (trdrOfferadj(trdrName)
       and Ildadj(ild)
       and trdrOfferIldMap(tp,trdr,o,ild)
       and trdrNameMap(trdr,trdrName)
       and tpOffertrdr(tp,o,trdr)), 1))) = sum(dt $ dtTradePeriodMap(dt,tp), o_OfferTradeBlockEnergy_TP(dt,o,trdBlk));

tpEnergyOffer_temp(tp,o,trdBlk,NRGofrCmpnt) = tpEnergyOffer(tp,o,trdBlk,NRGofrCmpnt);
tpEnergyOffer_orig(tp,o,trdBlk,NRGofrCmpnt) = tpEnergyOffer(tp,o,trdBlk,NRGofrCmpnt);

*=====================================================================================
*Calculate the total MW offered by each trader in each island during each trading period
trdrIldOfferedMW(tp,ild,trdr) = sum((o,trdBlk) $ (Offeradj(o) and trdrOfferIldMap(tp,trdr,o,ild)), tpEnergyOffer(tp,o,trdBlk,'i_GenerationMWOffer'));
MWToMove(tp,trdr) $ (sum(trdrName $ trdrNameMap(trdr,trdrName), trdrOfferadj(trdrName)) and Percentageadj) = (Percentageadj/100) * sum(ild $ Ildadj(ild), trdrIldOfferedMW(tp,ild,trdr));
MWToMove(tp,trdr) $ (sum(trdrName $ trdrNameMap(trdr,trdrName), trdrOfferadj(trdrName)) and MWadj) = min(MWadj, sum(ild $ Ildadj(ild), trdrIldOfferedMW(tp,ild,trdr)));

*=====================================================================================
*Loop until the required MW are moved to the higher price tranch
loop((tp,trdr) $ [sum(trdrName $ trdrNameMap(trdr,trdrName), trdrOfferadj(trdrName))],
    LoopIteration = 1;
    RemainMWToMove(tp,trdr) = MWToMove(tp,trdr);
    while((RemainMWToMove(tp,trdr)>0) and (LoopIteration <= MaxLoopIteration),
        MaxPB(tp,ild,trdr) = 0;
        trdrMaxPB(tp,trdr) = 0;
        zeroMaxPrice(tp,ild,trdr) = 0;
        TotMWMinMaxPB(tp,trdr) = 0;
        o_MWMinMaxPB(tp,trdr,o,trdBlk) = 0;
        PropOfferMinMaxPB(tp,trdr,o,trdBlk) = 0;
        o_MWToMove(tp,trdr,o,trdBlk) = 0;
        TotMWMoved(tp,trdr) = 0;
        MaxPB(tp,ild,trdr) $ trdrIldOfferedMW(tp,ild,trdr) = smax((o,trdBlk) $ (Offeradj(o) 
		  								                                     and trdrOfferIldMap(tp,trdr,o,ild) 
		  								                                     and tpEnergyOffer_temp(tp,o,trdBlk,'i_GenerationMWOffer') 
		  								                                     and (ord(trdBlk)<20)
		  								                                     and IncludeFKOffer(tp,o)), tpEnergyOffer_temp(tp,o,trdBlk,'i_GenerationMWOfferPrice'));

        zeroMaxPrice(tp,ild,trdr) $ [trdrIldOfferedMW(tp,ild,trdr) and (MaxPB(tp,ild,trdr) = 0)] = 1; 

*       Do this to set the max price when zero since 0 does not seem to work in the ffg smax
        MaxPB(tp,ild,trdr) $ [trdrIldOfferedMW(tp,ild,trdr) and (zeroMaxPrice(tp,ild,trdr) = 1)] = -1;
        trdrMaxPB(tp,trdr) $ MWToMove(tp,trdr) = smax(ild $ [Ildadj(ild)], MaxPB(tp,ild,trdr));          
		o_MWMinMaxPB(tp,trdr,o,trdBlk) $ (Offeradj(o) and (ord(trdBlk) < 20) and MWToMove(tp,trdr)) = sum(ild $ (Ildadj(ild) 
																								                   		and trdrOfferIldMap(tp,trdr,o,ild)
																								                   		and (tpEnergyOffer_temp(tp,o,trdBlk,'i_GenerationMWOfferPrice') = trdrMaxPB(tp,trdr)) 
																								                   		and IncludeFKOffer(tp,o)), tpEnergyOffer_temp(tp,o,trdBlk,'i_GenerationMWOffer'));


        o_MWMinMaxPB(tp,trdr,o,trdBlk) $ (Offeradj(o)	and (ord(trdBlk) < 20) and MWToMove(tp,trdr) and (trdrMaxPB(tp,trdr) = -1)) = sum(ild $ (Ildadj(ild) 
                                                                                                                                                             and trdrOfferIldMap(tp,trdr,o,ild)
                                                                                                                                                             and IncludeFKOffer(tp,o)), tpEnergyOffer_temp(tp,o,trdBlk,'i_GenerationMWOffer'));

*       display MaxPB, trdrMaxPB, ZeroMaxPrice, o_MWMinMaxPB;

        TotMWMinMaxPB(tp,trdr) $ (MWToMove(tp,trdr)) = sum((o,ild,trdBlk) $ (Offeradj(o) 
                                                                       and Ildadj(ild) 
                                                                       and trdrOfferIldMap(tp,trdr,o,ild) 
                                                                       and IncludeFKOffer(tp,o)), o_MWMinMaxPB(tp,trdr,o,trdBlk));

        PropOfferMinMaxPB(tp,trdr,o,trdBlk) $ (Offeradj(o) and TotMWMinMaxPB(tp,trdr) and (ord(trdBlk) < 20)) = o_MWMinMaxPB(tp,trdr,o,trdBlk)/TotMWMinMaxPB(tp,trdr);

*       If TotMWMinMaxPB <= Remaining MW to move, then move the entire quantity to the maximum price band otherwise limit the amount to the remaining MW to move
        
        TotMWMoved(tp,trdr) = min(TotMWMinMaxPB(tp,trdr),RemainMWToMove(tp,trdr));

*       Pro-rate the MW to move by offer depending on the proportion of its MW at the total minimum offer price
        
        o_MWToMove(tp,trdr,o,trdBlk) $ Offeradj(o) =  TotMWMoved(tp,trdr) * PropOfferMinMaxPB(tp,trdr,o,trdBlk);

*       If no offers to move then exit		  
        
        LoopIteration $ (TotMWMoved(tp,trdr) = 0) = MaxLoopIteration; 
		  
*       display o_MWToMove;		  
		  
*       This section ensures that the minimum generation of the fkeeper is not adjusted below its minimum requirement

		FKgen(tp,trdr,ild,MnodeCstr) = sum((o,trdBlk) $ [trdrOfferIldMap(tp,trdr,o,ild) 
                                                    and tpMNodeEnergyOfferCstrFactors(tp,MnodeCstr,o)
                                                    and (ord(trdBlk) < 20)], tpEnergyOffer_temp(tp,o,trdBlk,'i_GenerationMWOffer'));
																				 
        FKtrdrIldMinCstr(tp,trdr,ild,MnodeCstr) $ [(tpMNodeCstrRHS(tp,MnodeCstr,'i_CstrSense') = 1) and sum(o $ trdrOfferIldMap(tp,trdr,o,ild), tpMNodeEnergyOfferCstrFactors(tp,MnodeCstr,o))] = yes;
		   
        FKminGen(tp,trdr,ild,MnodeCstr) $ FKtrdrIldMinCstr(tp,trdr,ild,MnodeCstr) = tpMNodeCstrRHS(tp,MnodeCstr,'i_CstrLimit');
		   
        FKMWAtMinorMaxPB(tp,trdr,ild,MnodeCstr) = sum((o,trdBlk) $ [trdrOfferIldMap(tp,trdr,o,ild) and tpMNodeEnergyOfferCstrFactors(tp,MnodeCstr,o)], o_MWMinMaxPB(tp,trdr,o,trdBlk));
																			   																		   
*       This is the available MW that can be moved          

        FKAvailMWToMove(FKtrdrIldMinCstr) = FKgen(FKtrdrIldMinCstr) - FKminGen(FKtrdrIldMinCstr);
		  
*       This is the offered MW that should be moved		  
        
        FKo_MWToMove(tp,trdr,ild,MnodeCstr) $ FKtrdrIldMinCstr(tp,trdr,ild,MnodeCstr) = sum((o,trdBlk) $ [trdrOfferIldMap(tp,trdr,o,ild) 
											   and tpMNodeEnergyOfferCstrFactors(tp,MnodeCstr,o)
												], o_MWToMove(tp,trdr,o,trdBlk));

																											
*If the offered MW that should be moved exceeds the available MW that can be moved then exclude the FK from future calcs - at the min gen
		  IncludeFK(FKtrdrIldMinCstr) $ [FKo_MWToMove(FKtrdrIldMinCstr) >= FKAvailMWToMove(FKtrdrIldMinCstr)] = 0;

		  
*Exclude the offers mapped as the Fkeeper if the IncludeFK is set to zero
          IncludeFKOffer(tp,o) $ sum((ild,MnodeCstr) $ (trdrOfferIldMap(tp,trdr,o,ild)
																					  and FKtrdrIldMinCstr(tp,trdr,ild,MnodeCstr)
																					  and tpMNodeEnergyOfferCstrFactors(tp,MnodeCstr,o)
																					  ), 1-IncludeFK(tp,trdr,ild,MnodeCstr)) = 0;	  

*display FKAvailMWToMove, FKo_MWToMove, IncludeFK, IncludeFKOffer;
																					  
*Adjust the offered MW that can be moved - limited to the available MW that can be moved
		  AdjFKo_MWToMove(tp,trdr,ild,MnodeCstr)
            = max(0,min(FKAvailMWToMove(tp,trdr,ild,MnodeCstr), FKo_MWToMove(tp,trdr,ild,MnodeCstr)));

			
*Now if there are multiple offers that need adjustment
*Determine the proportion of the adjusted FK offer movement if need to allocate to several FK offers
		  FKAdjprop(tp,trdr,o,trdBlk) = 0;		 
		  FKAdjprop(tp,trdr,o,trdBlk) $ sum((ild,MnodeCstr) $ (trdrOfferIldMap(tp,trdr,o,ild)
																									   and tpMNodeEnergyOfferCstrFactors(tp,MnodeCstr,o) 
																						               ), FKo_MWToMove(tp,trdr,ild,MnodeCstr)) 
		    = o_MWToMove(tp,trdr,o,trdBlk) / sum((ild,MnodeCstr) $ (trdrOfferIldMap(tp,trdr,o,ild)
																											   and tpMNodeEnergyOfferCstrFactors(tp,MnodeCstr,o) 
																											   ), FKo_MWToMove(tp,trdr,ild,MnodeCstr));
			
*Determine the adjusted allocation
 		  FKAdjalloc(tp,trdr,o,trdBlk) = 0;		 
		  FKAdjalloc(tp,trdr,o,trdBlk) $ sum((ild,MnodeCstr) $ (trdrOfferIldMap(tp,trdr,o,ild)
																									   and tpMNodeEnergyOfferCstrFactors(tp,MnodeCstr,o) 
																						               ), FKo_MWToMove(tp,trdr,ild,MnodeCstr)) 
		    = FKAdjprop(tp,trdr,o,trdBlk) * sum((ild,MnodeCstr) $ (trdrOfferIldMap(tp,trdr,o,ild)
																									       and tpMNodeEnergyOfferCstrFactors(tp,MnodeCstr,o) 
																						                   ), AdjFKo_MWToMove(tp,trdr,ild,MnodeCstr));

*display FKAdjprop, IncludeFKOffer, IncludeFK;

*+++++++++This section ensures that the minimum generation of the fkeeper is not adjusted below its minimum requirement
	
*+++++++++++++++++++++++++++++++++++++++++++++++++Update with the FK allocation 
     
*Reassign the offer MW of FKeepers at their limit	  
		  o_MWMinMaxPB(tp,trdr,o,trdBlk) $ (Offeradj(o) 
		                                                                             and (ord(trdBlk) < 20) 
																					 and MWToMove(tp,trdr)
																					 and (IncludeFKOffer(tp,o) = 0)
																					 ) = FKAdjalloc(tp,trdr,o,trdBlk);
	
	      TotMWMinMaxPB(tp,trdr) $ (MWToMove(tp,trdr)) = sum((o,ild,trdBlk) $ (Offeradj(o) 
		                                                                                                                                 and Ildadj(ild) 
																																		 and trdrOfferIldMap(tp,trdr,o,ild) 
																																		 ), o_MWMinMaxPB(tp,trdr,o,trdBlk));


          PropOfferMinMaxPB(tp,trdr,o,trdBlk) $ (Offeradj(o) and TotMWMinMaxPB(tp,trdr) and (ord(trdBlk) < 20)) = o_MWMinMaxPB(tp,trdr,o,trdBlk)/TotMWMinMaxPB(tp,trdr);

*If TotMWMinMaxPB <= Remaining MW to move, then move the entire quantity to the maximum price band otherwise limit the amount to the remaining MW to move
          TotMWMoved(tp,trdr) = min(TotMWMinMaxPB(tp,trdr),RemainMWToMove(tp,trdr));

*Pro-rate the MW to move by offer depending on the proportion of its MW at the total minimum offer price
          o_MWToMove(tp,trdr,o,trdBlk) $ Offeradj(o) =  TotMWMoved(tp,trdr) * PropOfferMinMaxPB(tp,trdr,o,trdBlk);

*+++++++++++++++++++++++++++++++++++++++++++++++++Update with the FK allocation 	

		  tpEnergyOffer_temp(tp,o,trdBlk,'i_GenerationMWOffer') $ (Offeradj(o) 
																									 and o_MWToMove(tp,trdr,o,trdBlk) 
																									 ) = max(0,(tpEnergyOffer_temp(tp,o,trdBlk,'i_GenerationMWOffer') - o_MWToMove(tp,trdr,o,trdBlk)));

          tpEnergyOffer_temp(tp,o,'t20','i_GenerationMWOfferPrice') $ (Offeradj(o) 
																								   and sum(trdBlk, o_MWToMove(tp,trdr,o,trdBlk))
																								   ) = TargetPrice;

          tpEnergyOffer_temp(tp,o,'t20','i_GenerationMWOffer') $ (Offeradj(o) 
																							  and sum(trdBlk, o_MWToMove(tp,trdr,o,trdBlk))
																							  ) = tpEnergyOffer_temp(tp,o,'t20','i_GenerationMWOffer') + sum(trdBlk, o_MWToMove(tp,trdr,o,trdBlk));

         RemainMWToMove(tp,trdr) = RemainMWToMove(tp,trdr) - TotMWMoved(tp,trdr);
	 
         trdrMWMoved(tp,trdr,ild) = trdrMWMoved(tp,trdr,ild) + sum((o,trdBlk) $ (Offeradj(o) and trdrOfferIldMap(tp,trdr,o,ild)), o_MWToMove(tp,trdr,o,trdBlk));

         LoopIteration = LoopIteration + 1;

*display RemainMWToMove, trdrMWMoved;

*End while
     );

*End loop
);

*RDN - Update the energy offer if the TradeOfferadj flag is set
tpEnergyOffer(tp,o,trdBlk,NRGofrCmpnt) $ (Offeradj(o) and sum(trdrName, trdrOfferadj(trdrName))) = tpEnergyOffer_temp(tp,o,trdBlk,NRGofrCmpnt);


*Clear the temp energy offer file
option clear = tpEnergyOffer_temp;


);

*execute_unload 'RevisedOffer.gdx', tpEnergyOffer;

