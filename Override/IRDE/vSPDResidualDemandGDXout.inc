*Report residual demand results

execute_unload '%OutputPath%%runName%\RunNum%VSPDRunNum%_IslandOutputRDA_TP.gdx', o_DateTime, o_Island, RDAScenario, ActiveRDAScenario, o_TraderName,                                                                                   o_TraderIslandSuppliedMW, o_TraderIslandGenRevenenue, o_TraderIslandGenCost, o_TraderIslandOfferedMW, o_TraderIslandOfferedPQ, o_TraderQMoved, o_IslandScenarioMW, o_IslandScenarioPrice
      o_IslandBaseMW, o_IslandBasePrice, o_IslandBaseGWAP, o_IslandScenarioGWAP, o_IRDE_refprice, o_IRDE_gwap, o_QWOP, o_QWOPcleared, o_maxPcleared;


display o_QWOP;

*Write the cleared generation by trade block
if(BaseSolve = 1,
  execute_unload '%OutputPath%\basePQ\%VSPDInputData%_basePQ_TP.gdx', i_TradeBlock, o_OfferTradeBlockEnergy_TP, o_BaseNodePrice_TP, o_TradePeriodBaseEnergyOffer;
);





            
      
