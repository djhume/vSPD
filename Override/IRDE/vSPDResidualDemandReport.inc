*RDN -Residual supply analysis - Calculate benefit by node depending on generator, load or negative load node

       o_NodeGenCost_TP(i_DateTime,i_Node) $ (Node(CurrentTradePeriod,i_Node) and (o_NodeGeneration_TP(i_DateTime,i_Node) > 0)) = (i_TradingPeriodLength/60)*sum((i_Offer,i_TradeBlock) $ (OfferNode(CurrentTradePeriod,i_Offer,i_Node) and ValidGenerationOfferBlock(CurrentTradePeriod,i_Offer,i_TradeBlock)), GENERATIONBLOCK.l(CurrentTradePeriod,i_Offer,i_TradeBlock) * GenerationOfferPrice(CurrentTradePeriod,i_Offer,i_TradeBlock));



*RDN - Residual demand analysis
      o_IslandPosLoad_TP(i_DateTime,i_Island) = sum(i_Bus $ (BusIsland(CurrentTradePeriod,i_Bus,i_Island) and (BusLoad(CurrentTradePeriod,i_Bus)>0)), BusLoad(CurrentTradePeriod,i_Bus));
      o_IslandNegLoad_TP(i_DateTime,i_Island) = sum(i_Bus $ (BusIsland(CurrentTradePeriod,i_Bus,i_Island) and (BusLoad(CurrentTradePeriod,i_Bus)<0)), -BusLoad(CurrentTradePeriod,i_Bus));

      o_TraderName(i_DateTime,i_TraderName) = yes;

      o_TraderIslandSuppliedMW(i_DateTime,i_TraderName,i_Island) = sum((i_Offer,i_Trader) $ (TraderNameMap(i_Trader,i_TraderName) and TraderOfferIslandMap(CurrentTradePeriod,i_Trader,i_Offer,i_Island)), GENERATION.l(CurrentTradePeriod,i_Offer));

      o_TraderIslandGenRevenenue(i_DateTime,i_TraderName,i_Island) = sum((i_Trader,i_Node,i_Offer) $ (TraderNameMap(i_Trader,i_TraderName) and TraderOfferIslandMap(CurrentTradePeriod,i_Trader,i_Offer,i_Island) and OfferNode(CurrentTradePeriod,i_Offer,i_Node)), o_NodeRevenue_TP(i_DateTime,i_Node));

      o_TraderIslandGenCost(i_DateTime,i_TraderName,i_Island) = sum((i_Trader,i_Node,i_Offer) $ (TraderNameMap(i_Trader,i_TraderName) and TraderOfferIslandMap(CurrentTradePeriod,i_Trader,i_Offer,i_Island) and OfferNode(CurrentTradePeriod,i_Offer,i_Node)), o_NodeGenCost_TP(i_DateTime,i_Node));

      o_TraderIslandOfferedMW(i_DateTime,i_TraderName,i_Island) = sum(i_Trader $ TraderNameMap(i_Trader,i_TraderName), TraderIslandOfferedMW(CurrentTradePeriod,i_Island,i_Trader));

      o_TraderIslandOfferedPQ(i_DateTime,i_TraderName,i_Island) = sum((i_Trader,i_Offer,i_TradeBlock) $ (TraderNameMap(i_Trader,i_TraderName) and TraderOfferIslandMap(CurrentTradePeriod,i_Trader,i_Offer,i_Island)), i_TradePeriodEnergyOffer(CurrentTradePeriod,i_Offer,i_TradeBlock,'i_GenerationMWOffer')*i_TradePeriodEnergyOffer(CurrentTradePeriod,i_Offer,i_TradeBlock,'i_GenerationMWOfferPrice'));

      o_TraderQMoved(i_DateTime,i_TraderName,i_Island) = sum(i_Trader $ TraderNameMap(i_Trader,i_TraderName), TraderMWMoved(CurrentTradePeriod,i_Trader,i_Island));

      o_EnergyOffer_diff(i_DateTime,i_Offer,i_TradeBlock,i_EnergyOfferComponent) = i_TradePeriodEnergyOffer(CurrentTradePeriod,i_Offer,i_TradeBlock,i_EnergyOfferComponent) - i_TradePeriodEnergyOffer_orig(CurrentTradePeriod,i_Offer,i_TradeBlock,i_EnergyOfferComponent);

*RDN - Output of offers and price


if (BaseSolve = 0,

      o_IslandScenarioMW(i_DateTime,i_Island) = 0;
      o_IslandScenarioPrice(i_DateTime,i_Island) = 0;
      o_IslandScenarioPQ(i_DateTime,i_Island) = 0;


*Base
      o_IslandBaseMW(i_DateTime,i_Island) = sum((i_Offer,i_Trader,i_TradeBlock) $ (OfferAdjustment(i_Offer) and TraderOfferIslandMap(CurrentTradePeriod,i_Trader,i_Offer,i_Island)), o_OfferTradeBlockEnergy_TP(i_DateTime,i_Offer,i_TradeBlock));

*      o_IslandBasePQ(i_DateTime,i_Island) = sum((i_Offer,i_Bus,i_Node,i_TradeBlock) $ (OfferAdjustment(i_Offer) and OfferNode(CurrentTradePeriod,i_Offer,i_Node) and NodeBus(CurrentTradePeriod,i_Node,i_Bus) and BusIsland*(CurrentTradePeriod,i_Bus,i_Island)), NodeBusAllocationFactor(CurrentTradePeriod,i_Node,i_Bus) * o_OfferTradeBlockEnergy_TP(i_DateTime,i_Offer,i_TradeBlock) * BusPrice(CurrentTradePeriod,i_Bus));


      o_IslandBasePQ(i_DateTime,i_Island) = sum((i_Trader,i_Offer,i_Node,i_TradeBlock) $ (OfferAdjustment(i_Offer) and OfferNode(CurrentTradePeriod,i_Offer,i_Node) and TraderOfferIslandMap(CurrentTradePeriod,i_Trader,i_Offer,i_Island)), o_OfferTradeBlockEnergy_TP(i_DateTime,i_Offer,i_TradeBlock) * o_BaseNodePrice_TP(i_DateTime,i_Node));


      o_IslandBasePrice(i_DateTime,i_Island) = sum(i_Node $ (ReferenceNode(CurrentTradePeriod,i_Node) and NodeIsland(CurrentTradePeriod,i_Node,i_Island)), o_BaseNodePrice_TP(i_DateTime,i_Node));


*Scenario
      o_IslandScenarioMW(i_DateTime,i_Island) = sum((i_Offer,i_Trader) $ (OfferAdjustment(i_Offer) and TraderOfferIslandMap(CurrentTradePeriod,i_Trader,i_Offer,i_Island)), GENERATION.l(CurrentTradePeriod,i_Offer));

      o_IslandScenarioPrice(i_DateTime,i_Island) = o_IslandRefPrice_TP(i_DateTime,i_Island);

      o_IslandScenarioPQ(i_DateTime,i_Island) = sum((i_Offer,i_Bus,i_Node) $ (OfferAdjustment(i_Offer) and OfferNode(CurrentTradePeriod,i_Offer,i_Node) and NodeBus(CurrentTradePeriod,i_Node,i_Bus) and BusIsland(CurrentTradePeriod,i_Bus,i_Island)), NodeBusAllocationFactor(CurrentTradePeriod,i_Node,i_Bus) * GENERATION.l(CurrentTradePeriod,i_Offer) * BusPrice(CurrentTradePeriod,i_Bus));



*Report gwap and lwap
      o_IslandAllGen(i_DateTime,i_Island) = sum(i_Bus $ BusIsland(CurrentTradePeriod,i_Bus,i_Island), BusGeneration(CurrentTradePeriod,i_Bus))
                                             + sum(i_Bus $ [BusIsland(CurrentTradePeriod,i_Bus,i_Island) and (BusLoad(CurrentTradePeriod,i_Bus)<0)], abs(BusLoad(CurrentTradePeriod,i_Bus)));


      o_IslandPosLoad(i_DateTime,i_Island) = sum(i_Bus $ [BusIsland(CurrentTradePeriod,i_Bus,i_Island) and (BusLoad(CurrentTradePeriod,i_Bus)>0)], BusLoad(CurrentTradePeriod,i_Bus));


*      o_IslandScenarioGWAP(i_DateTime,i_Island) $ (o_IslandAllGen(i_DateTime,i_Island) and i_TradingPeriodLength) = [(60/i_TradingPeriodLength)*(o_IslandLoadRevenue_TP(i_DateTime,i_Island)+o_IslandEnergyRevenue_TP**(i_DateTime,i_Island))]/o_IslandAllGen(i_DateTime,i_Island);

      o_IslandScenarioGWAP(i_DateTime,i_Island) $ (o_IslandScenarioMW(i_DateTime,i_Island) and i_TradingPeriodLength) = o_IslandScenarioPQ(i_DateTime,i_Island)/o_IslandScenarioMW(i_DateTime,i_Island);


      o_IslandBaseGWAP(i_DateTime,i_Island) $ (o_IslandBaseMW(i_DateTime,i_Island) and i_TradingPeriodLength) = o_IslandBasePQ(i_DateTime,i_Island)/o_IslandBaseMW(i_DateTime,i_Island);


*Calculate the IRDE

      prop_Qchange(i_DateTime,i_Island) $ o_IslandBaseMW(i_DateTime,i_Island) = (o_IslandScenarioMW(i_DateTime,i_Island) - o_IslandBaseMW(i_DateTime,i_Island)) / o_IslandBaseMW(i_DateTime,i_Island);

      o_IRDE_refprice(i_DateTime,i_Island) $ prop_Qchange(i_DateTime,i_Island) = -[(o_IslandScenarioPrice(i_DateTime,i_Island) - o_IslandBasePrice(i_DateTime,i_Island))/o_IslandBasePrice(i_DateTime,i_Island)] / prop_Qchange(i_DateTime,i_Island);

      o_IRDE_gwap(i_DateTime,i_Island) $ prop_Qchange(i_DateTime,i_Island) = -[(o_IslandScenarioGWAP(i_DateTime,i_Island) - o_IslandBaseGWAP(i_DateTime,i_Island))/o_IslandBaseGWAP(i_DateTime,i_Island)] / prop_Qchange(i_DateTime,i_Island);


      o_Qoffer(i_DateTime,i_Island) = sum((i_Offer,i_Trader,i_TradeBlock) $ (OfferAdjustment(i_Offer) and TraderOfferIslandMap(CurrentTradePeriod,i_Trader,i_Offer,i_Island)), o_TradePeriodBaseEnergyOffer(i_DateTime,i_Offer,i_TradeBlock,'i_GenerationMWOffer'));


      o_PQoffer(i_DateTime,i_Island) = sum((i_Offer,i_Trader,i_TradeBlock) $ (OfferAdjustment(i_Offer) and TraderOfferIslandMap(CurrentTradePeriod,i_Trader,i_Offer,i_Island)), o_TradePeriodBaseEnergyOffer(i_DateTime,i_Offer,i_TradeBlock,'i_GenerationMWOfferPrice') * o_TradePeriodBaseEnergyOffer(i_DateTime,i_Offer,i_TradeBlock,'i_GenerationMWOffer'));


      o_QWOP(i_DateTime,i_Island) $ o_Qoffer(i_DateTime,i_Island)  = o_PQoffer(i_DateTime,i_Island) / o_Qoffer(i_DateTime,i_Island);


      o_PQoffercleared(i_DateTime,i_Island) = sum((i_Offer,i_Trader,i_TradeBlock) $ (OfferAdjustment(i_Offer) and TraderOfferIslandMap(CurrentTradePeriod,i_Trader,i_Offer,i_Island)), o_TradePeriodBaseEnergyOffer(i_DateTime,i_Offer,i_TradeBlock,'i_GenerationMWOfferPrice') * o_OfferTradeBlockEnergy_TP(i_DateTime,i_Offer,i_TradeBlock));

      o_QWOPcleared(i_DateTime,i_Island) $ o_IslandBaseMW(i_DateTime,i_Island) = o_PQoffercleared(i_DateTime,i_Island) / o_IslandBaseMW(i_DateTime,i_Island);

      o_maxPcleared(i_DateTime,i_Island) $ sum((i_Offer,i_Trader) $ (OfferAdjustment(i_Offer) and TraderOfferIslandMap(CurrentTradePeriod,i_Trader,i_Offer,i_Island)),1) 
 = smax((i_Offer,i_Trader,i_TradeBlock) $ (OfferAdjustment(i_Offer) and TraderOfferIslandMap (CurrentTradePeriod,i_Trader,i_Offer,i_Island) and o_OfferTradeBlockEnergy_TP(i_DateTime,i_Offer,i_TradeBlock)), o_TradePeriodBaseEnergyOffer (i_DateTime,i_Offer,i_TradeBlock,'i_GenerationMWOfferPrice'));

else


*RDN - Write out the cleared offers and price and offers
      o_OfferTradeBlockEnergy_TP(i_DateTime,i_Offer,i_TradeBlock) = GENERATIONBLOCK.l(CurrentTradePeriod,i_Offer,i_TradeBlock);
      o_BaseNodePrice_TP(i_DateTime,i_Node) = o_NodePrice_TP(i_DateTime,i_Node);

      o_TradePeriodBaseEnergyOffer(i_DateTime,i_Offer,i_TradeBlock,i_EnergyOfferComponent) = i_TradePeriodEnergyOffer(CurrentTradePeriod,i_Offer,i_TradeBlock,i_EnergyOfferComponent);

      
      o_IslandBaseMW(i_DateTime,i_Island) = 0;
      o_IslandBasePrice(i_DateTime,i_Island) = 0;
      o_IslandBasePQ(i_DateTime,i_Island) = 0;
      o_QWOP(i_DateTime,i_Island) = 0;
      o_Qoffer(i_DateTime,i_Island) = 0;
      o_PQoffer(i_DateTime,i_Island) = 0;
      o_PQoffercleared(i_DateTime,i_Island) = 0;
      o_QWOPcleared(i_DateTime,i_Island) = 0;
      o_maxPcleared(i_DateTime,i_Island) = 0;

);


