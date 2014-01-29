*This code creates the output report for pivotal analysis

Sets
PivotScenario(*)
ActivePivotScenario(*)
;

Parameters
o_PivotScheduledMW_TP(*,*)
o_PivotFKMin_TP(*,*)
o_PivotMW_TP(*,*)
o_EnergyOffered(*,*)
o_FIROffered(*,*)
o_SIROffered(*,*)
o_PivotEnergyOffered(*,*)
o_PivotFIROffered(*,*)
o_PivotSIROffered(*,*)
o_PivotOfferFIR_TP(*,*)
o_PivotOfferSIR_TP(*,*)

o_IslandPosLoad_TP(*,*)
o_IslandGWAP_TP(*,*)
o_IslandLWAP_TP(*,*)
o_IslandLoadAtPivotPrice(*,*)
o_IslandPivotTraderTotGen(*,*)
o_maxPivotNodePrice(*,*)

;

*Pivot offer data
$if %PivotAnalysis%==1 $GDXIN "%OutputPath%%runName%\RunNum%VSPDRunNum%_PivotOutput_TP.gdx"

$if %PivotAnalysis%==1 $LOAD PivotScenario ActivePivotScenario

$if %PivotAnalysis%==1 $LOAD o_PivotScheduledMW_TP o_PivotFKMin_TP o_PivotMW_TP
$if %PivotAnalysis%==1 $LOAD o_EnergyOffered o_FIROffered o_SIROffered
$if %PivotAnalysis%==1 $LOAD o_PivotEnergyOffered o_PivotFIROffered o_PivotSIROffered
$if %PivotAnalysis%==1 $LOAD o_PLRO_FIR_TP o_PLRO_SIR_TP o_TWRO_FIR_TP o_TWRO_SIR_TP
$if %PivotAnalysis%==1 $LOAD o_ILRO_FIR_TP o_ILRO_SIR_TP o_ILBus_FIR_TP o_ILBus_SIR_TP
$if %PivotAnalysis%==1 $LOAD o_GenerationRiskSetter o_GenHVDCRiskSetter o_HVDCRiskSetter
$if %PivotAnalysis%==1 $LOAD o_MANURiskSetter o_MANUHVDCRiskSetter o_FIRCleared_TP o_SIRCleared_TP
$if %PivotAnalysis%==1 $LOAD o_PivotOfferFIR_TP o_PivotOfferSIR_TP
$if %PivotAnalysis%==1 $LOAD o_IslandPosLoad_TP o_IslandGWAP_TP o_IslandLWAP_TP o_IslandLoadAtPivotPrice o_IslandPivotTraderTotGen o_maxPivotNodePrice
*Close the gdx
$if %PivotAnalysis%==1 $GDXIN

FILES
IslandPivotResults_TP        / "%OutputPath%%runName%\%runName%_IslandPivotResults_TP.csv" /
;

IslandPivotResults_TP.pc = 5;        IslandPivotResults_TP.lw = 0;       IslandPivotResults_TP.pw = 9999;            IslandPivotResults_TP.ap = 1;       IslandPivotResults_TP.nd = 5;

put IslandPivotResults_TP;
loop((dim1,dim2,dim3) $ (o_DateTime(dim1) and o_Island(dim1,dim2) and (ActivePivotScenario(dim3))),
  put dim1.tl, dim2.tl, dim3.tl, o_IslandGen_TP(dim1,dim2), o_IslandLoad_TP(dim1,dim2), o_IslandBranchLoss_TP(dim1,dim2), o_HVDCFlow_TP(dim1,dim2), o_HVDCLoss_TP(dim1,dim2), o_IslandRefPrice_TP(dim1,dim2)
      o_FIRReqd_TP(dim1,dim2), o_SIRReqd_TP(dim1,dim2), o_FIRPrice_TP(dim1,dim2), o_SIRPrice_TP(dim1,dim2)
      o_EnergyOffered(dim1,dim2), o_FIROffered(dim1,dim2), o_SIROffered(dim1,dim2)
      o_PivotEnergyOffered(dim1,dim2), o_PivotFIROffered(dim1,dim2), o_PivotSIROffered(dim1,dim2)
      o_PivotScheduledMW_TP(dim1,dim2), o_PivotFKMin_TP(dim1,dim2), o_PivotMW_TP(dim1,dim2)
      o_PivotOfferFIR_TP(dim1,dim2), o_PivotOfferSIR_TP(dim1,dim2)
      o_IslandPosLoad_TP(dim1,dim2), o_IslandGWAP_TP(dim1,dim2), o_IslandLWAP_TP(dim1,dim2), o_IslandLoadAtPivotPrice(dim1,dim2), o_IslandPivotTraderTotGen(dim1,dim2), o_maxPivotNodePrice(dim1,dim2) /;
);