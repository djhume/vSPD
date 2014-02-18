*Report residual demand results

execute_unload '%OutputPath%%runName%\RunNum%VSPDRunNum%_IslandOutputRDA_TP.gdx', o_DateTime, o_Island, RDAScnr, ActRDAScnr, o_trdrName,
                                                                                  o_trdrIldSuppliedMW, o_trdrIldGenRevenenue, o_trdrIldGenCost,
                                                                                  o_trdrIldOfferedMW, o_trdrIldOfferedPQ, o_trdrQMoved,
                                                                                  o_IldScnrMW, o_IldScnrPrice, o_IldBaseMW, o_IldBasePrice,
                                                                                  o_IldBaseGWAP, o_IldScnrGWAP, o_IRDE_refprice, o_IRDE_gwap,
                                                                                  o_QWOP, o_QWOPcleared, o_maxPcleared;


*display o_QWOP;

*Write the cleared generation by trade block
if(BaseSolve = 1,
  execute_unload '%OutputPath%basePQ\%VSPDInputData%_basePQ_TP.gdx', 
       trdBlk, o_OfrTrdBlkNRG, o_BaseNodePrice_TP, o_tpbaseNRGOfr;
);





            
      
