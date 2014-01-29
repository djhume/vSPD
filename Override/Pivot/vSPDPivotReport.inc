*This section of code records the pivotal results

       o_PivotOffer(i_Offer,i_Island) $ (PivotOffer(i_Offer) and PivotIsland(i_Island)) = yes;
       o_PivotScheduledMW_TP(i_DateTime,i_Island) = sum(i_Offer $ o_PivotOffer(i_Offer,i_Island), o_OfferEnergy_TP(i_DateTime,i_Offer));

*Map of mnode min constraints that contain pivot offers
      o_pivotMnodeMinmap(i_DateTime,i_Island,i_MNodeConstraint) $ sum(i_Offer $ [o_PivotOffer(i_Offer,i_Island)
                                                                                 and
                                                                                 (MNodeEnergyOfferConstraintFactors(CurrentTradePeriod,i_MNodeConstraint,i_Offer) > 0)
                                                                                 and
                                                                                 (MNodeConstraintSense(CurrentTradePeriod,i_MNodeConstraint) = 1)], 1)
                                                                                 = yes;


       o_PivotFKMin_TP(i_DateTime,i_Island) $ PivotIsland(i_Island) = sum(i_MnodeConstraint $ o_pivotMnodeMinmap(i_DateTime,i_Island,i_MNodeConstraint), o_MNodeConstraintRHS_TP(i_DateTime,i_MNodeConstraint));



       o_PivotMW_TP(i_DateTime,i_Island) = o_PivotScheduledMW_TP(i_DateTime,i_Island) - o_PivotFKMin_TP(i_DateTime,i_Island);
       o_EnergyOffered(i_DateTime,i_Island) = sum((i_Offer,i_TradeBlock) $ o_OfferIsland_TP(i_DateTime,i_Offer,i_Island),
                                                  GenerationOfferMW(CurrentTradePeriod,i_Offer,i_TradeBlock));
       o_FIROffered(i_DateTime,i_Island)  = sum((i_Offer,i_TradeBlock,i_ReserveType) $ o_OfferIsland_TP(i_DateTime,i_Offer,i_Island),
                                               ReserveOfferMaximum(CurrentTradePeriod,i_Offer,i_TradeBlock,'FIR',i_ReserveType));

       o_SIROffered(i_DateTime,i_Island)  = sum((i_Offer,i_TradeBlock,i_ReserveType) $ o_OfferIsland_TP(i_DateTime,i_Offer,i_Island),
                                               ReserveOfferMaximum(CurrentTradePeriod,i_Offer,i_TradeBlock,'SIR',i_ReserveType));


       o_PivotEnergyOffered(i_DateTime,i_Island) = sum((PivotOffer,i_TradeBlock) $ o_OfferIsland_TP(i_DateTime,PivotOffer,i_Island),
                                                  GenerationOfferMW(CurrentTradePeriod,PivotOffer,i_TradeBlock));
       o_PivotFIROffered(i_DateTime,i_Island)  = sum((PivotOffer,i_TradeBlock,i_ReserveType) $ o_OfferIsland_TP(i_DateTime,PivotOffer,i_Island),
                                               ReserveOfferMaximum(CurrentTradePeriod,PivotOffer,i_TradeBlock,'FIR',i_ReserveType));
       o_PivotSIROffered(i_DateTime,i_Island)  = sum((PivotOffer,i_TradeBlock,i_ReserveType) $ o_OfferIsland_TP(i_DateTime,PivotOffer,i_Island),
                                               ReserveOfferMaximum(CurrentTradePeriod,PivotOffer,i_TradeBlock,'SIR',i_ReserveType));

       o_PivotOfferFIR_TP(i_DateTime,i_Island) = sum(i_Offer $ o_PivotOffer(i_Offer,i_Island), o_OfferFIR_TP(i_DateTime,i_Offer));
       o_PivotOfferSIR_TP(i_DateTime,i_Island) = sum(i_Offer $ o_PivotOffer(i_Offer,i_Island), o_OfferSIR_TP(i_DateTime,i_Offer));

*Maximum nodal price at a pivot offer in an island
       o_maxPivotNodePrice(i_DateTime,i_Island) = 0;
       o_maxPivotNodePrice(i_DateTime,i_Island) = smax((i_Offer,i_Node) $ [o_PivotOffer(i_Offer,i_Island) and OfferNode(CurrentTradePeriod,i_Offer,i_Node)], o_NodePrice_TP(i_DateTime,i_Node));

       o_maxPivotNodePrice(i_DateTime,i_Island) $ (not(sum(i_Offer $ o_PivotOffer(i_Offer,i_Island), 1))) = 0;

*Report gwap and lwap
      o_IslandAllGen_TP(i_DateTime,i_Island) = sum(i_Bus $ BusIsland(CurrentTradePeriod,i_Bus,i_Island), BusGeneration(CurrentTradePeriod,i_Bus))
                                             + sum(i_Bus $ [BusIsland(CurrentTradePeriod,i_Bus,i_Island) and (BusLoad(CurrentTradePeriod,i_Bus)<0)], abs(BusLoad(CurrentTradePeriod,i_Bus)));

      o_IslandPosLoad_TP(i_DateTime,i_Island) = sum(i_Bus $ [BusIsland(CurrentTradePeriod,i_Bus,i_Island) and (BusLoad(CurrentTradePeriod,i_Bus)>0)], BusLoad(CurrentTradePeriod,i_Bus));

      o_IslandGWAP_TP(i_DateTime,i_Island) $ (o_IslandAllGen_TP(i_DateTime,i_Island) and i_TradingPeriodLength) = [(60/i_TradingPeriodLength)*(o_IslandLoadRevenue_TP(i_DateTime,i_Island)+o_IslandEnergyRevenue_TP(i_DateTime,i_Island))]/o_IslandAllGen_TP(i_DateTime,i_Island);
      o_IslandLWAP_TP(i_DateTime,i_Island) $ (o_IslandPosLoad_TP(i_DateTime,i_Island) and i_TradingPeriodLength) = [(60/i_TradingPeriodLength)*o_IslandLoadCost_TP(i_DateTime,i_Island)]/o_IslandPosLoad_TP(i_DateTime,i_Island);

*Report load at pivot price and pivot trader total scheduled generation
      o_IslandLoadAtPivotPrice(i_DateTime,i_Island) = sum(i_Bus $ [BusIsland(CurrentTradePeriod,i_Bus,i_Island) and (BusLoad(CurrentTradePeriod,i_Bus)>0) and (BusPrice(CurrentTradePeriod,i_Bus) >= i_PivotPriceThreshold*i_PivotPrice)], BusLoad(CurrentTradePeriod,i_Bus));

*Report total scheduled generation
      o_IslandPivotTraderTotGen(i_DateTime,i_Island) = sum((i_Trader,i_Offer) $ [PivotTrader(i_Trader) and i_TradePeriodOfferTrader(CurrentTradePeriod,i_Offer,i_Trader)], o_OfferEnergy_TP(i_DateTime,i_Offer));
