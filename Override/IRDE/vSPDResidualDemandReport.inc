*RDN -Residual supply analysis - Calculate benefit by node depending on generator, load or negative load node

       o_NodeGenCost_TP(dt,n) $ (Node(currTP,n) and (o_NodeGeneration_TP(dt,n) > 0)) =
           (i_TradingPeriodLength/60)*sum((o,trdBlk) $ (OfferNode(currTP,o,n) and ValidGenerationOfferBlock(currTP,o,trdBlk)), GENERATIONBLOCK.l(currTP,o,trdBlk) * GenerationOfferPrice(currTP,o,trdBlk));



*RDN - Residual demand analysis
      o_IldPosLoad_TP(dt,ild) = sum(b $ (BusIld(currTP,b,ild) and (BusLoad(currTP,b)>0)), BusLoad(currTP,b));
      o_IldNegLoad_TP(dt,ild) = sum(b $ (BusIld(currTP,b,ild) and (BusLoad(currTP,b)<0)), -BusLoad(currTP,b));

      o_trdrName(dt,trdrName) = yes;
      o_trdrIldSuppliedMW(dt,trdrName,ild) = sum((o,trdr) $ (trdrNameMap(trdr,trdrName) and trdrOfferIldMap(currTP,trdr,o,ild)), GENERATION.l(currTP,o));
      o_trdrIldGenRevenenue(dt,trdrName,ild) = sum((trdr,n,o) $ (trdrNameMap(trdr,trdrName) and trdrOfferIldMap(currTP,trdr,o,ild) and OfferNode(currTP,o,n)), o_NodeRevenue_TP(dt,n));
      o_trdrIldGenCost(dt,trdrName,ild) = sum((trdr,n,o) $ (trdrNameMap(trdr,trdrName) and trdrOfferIldMap(currTP,trdr,o,ild) and OfferNode(currTP,o,n)), o_NodeGenCost_TP(dt,n));
      o_trdrIldOfferedMW(dt,trdrName,ild) = sum(trdr $ trdrNameMap(trdr,trdrName), trdrIldOfferedMW(currTP,ild,trdr));
      o_trdrIldOfferedPQ(dt,trdrName,ild) = sum((trdr,o,trdBlk) $ (trdrNameMap(trdr,trdrName) and trdrOfferIldMap(currTP,trdr,o,ild)), i_TradePeriodEnergyOffer(currTP,o,trdBlk,'i_GenerationMWOffer')*i_TradePeriodEnergyOffer(currTP,o,trdBlk,'i_GenerationMWOfferPrice'));
      o_trdrQMoved(dt,trdrName,ild) = sum(trdr $ trdrNameMap(trdr,trdrName), trdrMWMoved(currTP,trdr,ild));
      o_EnergyOffer_diff(dt,o,trdBlk,NRGofrCmpnt) = i_TradePeriodEnergyOffer(currTP,o,trdBlk,NRGofrCmpnt) - i_TradePeriodEnergyOffer_orig(currTP,o,trdBlk,NRGofrCmpnt);

*RDN - Output of offers and price

if (BaseSolve = 0,
      o_IldScnrMW(dt,ild) = 0;
      o_IldScnrPrice(dt,ild) = 0;
      o_IldScnrPQ(dt,ild) = 0;

*Base
      o_IldBaseMW(dt,ild) = sum((o,trdr,trdBlk) $ (OfferAdjustment(o) and trdrOfferIldMap(currTP,trdr,o,ild)), o_OfrTrdBlkNRG(dt,o,trdBlk));
      o_IldBasePQ(dt,ild) = sum((trdr,o,n,trdBlk) $ (OfferAdjustment(o) and OfferNode(currTP,o,n) and trdrOfferIldMap(currTP,trdr,o,ild)), o_OfrTrdBlkNRG(dt,o,trdBlk) * o_BaseNodePrice_TP(dt,n));
      o_IldBasePrice(dt,ild) = sum(n $ (ReferenceNode(currTP,n) and NodeIld(currTP,n,ild)), o_BaseNodePrice_TP(dt,n));

*Scnr
      o_IldScnrMW(dt,ild) = sum((o,trdr) $ (OfferAdjustment(o) and trdrOfferIldMap(currTP,trdr,o,ild)), GENERATION.l(currTP,o));
      o_IldScnrPrice(dt,ild) = o_IldRefPrice_TP(dt,ild);
      o_IldScnrPQ(dt,ild) = sum((o,b,n) $ (OfferAdjustment(o) and OfferNode(currTP,o,n) and NodeBus(currTP,n,b) and BusIld(currTP,b,ild)), NodeBusAllocationFactor(currTP,n,b) * GENERATION.l(currTP,o) * BusPrice(currTP,b));

*Report gwap and lwap
      o_IldAllGen(dt,ild) = sum(b $ BusIld(currTP,b,ild), BusGeneration(currTP,b))
                                             + sum(b $ [BusIld(currTP,b,ild) and (BusLoad(currTP,b)<0)], abs(BusLoad(currTP,b)));

      o_IldPosLoad(dt,ild) = sum(b $ [BusIld(currTP,b,ild) and (BusLoad(currTP,b)>0)], BusLoad(currTP,b));
      o_IldScnrGWAP(dt,ild) $ (o_IldScnrMW(dt,ild) and i_TradingPeriodLength) = o_IldScnrPQ(dt,ild)/o_IldScnrMW(dt,ild);
      o_IldBaseGWAP(dt,ild) $ (o_IldBaseMW(dt,ild) and i_TradingPeriodLength) = o_IldBasePQ(dt,ild)/o_IldBaseMW(dt,ild);

*Calculate the IRDE

      prop_Qchange(dt,ild) $ o_IldBaseMW(dt,ild) = (o_IldScnrMW(dt,ild) - o_IldBaseMW(dt,ild)) / o_IldBaseMW(dt,ild);
      o_IRDE_refprice(dt,ild) $ prop_Qchange(dt,ild) = -[(o_IldScnrPrice(dt,ild) - o_IldBasePrice(dt,ild))/o_IldBasePrice(dt,ild)] / prop_Qchange(dt,ild);
      o_IRDE_gwap(dt,ild) $ prop_Qchange(dt,ild) = -[(o_IldScnrGWAP(dt,ild) - o_IldBaseGWAP(dt,ild))/o_IldBaseGWAP(dt,ild)] / prop_Qchange(dt,ild);
      o_Qoffer(dt,ild) = sum((o,trdr,trdBlk) $ (OfferAdjustment(o) and trdrOfferIldMap(currTP,trdr,o,ild)), o_tpbaseNRGOfr(dt,o,trdBlk,'i_GenerationMWOffer'));
      o_PQoffer(dt,ild) = sum((o,trdr,trdBlk) $ (OfferAdjustment(o) and trdrOfferIldMap(currTP,trdr,o,ild)), o_tpbaseNRGOfr(dt,o,trdBlk,'i_GenerationMWOfferPrice') * o_tpbaseNRGOfr(dt,o,trdBlk,'i_GenerationMWOffer'));
      o_QWOP(dt,ild) $ o_Qoffer(dt,ild)  = o_PQoffer(dt,ild) / o_Qoffer(dt,ild);
      o_PQoffercleared(dt,ild) = sum((o,trdr,trdBlk) $ (OfferAdjustment(o) and trdrOfferIldMap(currTP,trdr,o,ild)), o_tpbaseNRGOfr(dt,o,trdBlk,'i_GenerationMWOfferPrice') * o_OfrTrdBlkNRG(dt,o,trdBlk));
      o_QWOPcleared(dt,ild) $ o_IldBaseMW(dt,ild) = o_PQoffercleared(dt,ild) / o_IldBaseMW(dt,ild);
      o_maxPcleared(dt,ild) $ sum((o,trdr) $ (OfferAdjustment(o) and trdrOfferIldMap(currTP,trdr,o,ild)),1) 
 = smax((o,trdr,trdBlk) $ (OfferAdjustment(o) and trdrOfferIldMap (currTP,trdr,o,ild) and o_OfrTrdBlkNRG(dt,o,trdBlk)), o_tpbaseNRGOfr (dt,o,trdBlk,'i_GenerationMWOfferPrice'));

else

*RDN - Write out the cleared offers and price and offers
      o_OfrTrdBlkNRG(dt,o,trdBlk) = GENERATIONBLOCK.l(currTP,o,trdBlk);
      o_BaseNodePrice_TP(dt,n) = o_NodePrice_TP(dt,n);
      o_tpbaseNRGOfr(dt,o,trdBlk,NRGofrCmpnt) = i_TradePeriodEnergyOffer(currTP,o,trdBlk,NRGofrCmpnt);
      o_IldBaseMW(dt,ild) = 0;
      o_IldBasePrice(dt,ild) = 0;
      o_IldBasePQ(dt,ild) = 0;
      o_QWOP(dt,ild) = 0;
      o_Qoffer(dt,ild) = 0;
      o_PQoffer(dt,ild) = 0;
      o_PQoffercleared(dt,ild) = 0;
      o_QWOPcleared(dt,ild) = 0;
      o_maxPcleared(dt,ild) = 0;

);


