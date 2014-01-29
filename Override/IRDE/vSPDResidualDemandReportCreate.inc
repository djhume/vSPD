*This code creates the output report for residual demand analysis

Sets
RDAScenario(*)
ActiveRDAScenario(*)
;

Parameters
o_IslandScenarioMW(*,*)
o_IslandScenarioPrice(*,*)
o_IslandBaseMW(*,*)
o_IslandBasePrice(*,*)
o_IslandBaseGWAP(*,*)
o_IslandScenarioGWAP(*,*)
o_IRDE_refprice(*,*)
o_IRDE_gwap(*,*)
o_QWOP(*,*)
o_QWOPcleared(*,*)
o_maxPcleared(*,*)
;


*Pivot offer data
$if %PivotAnalysis%==-1 $GDXIN "%OutputPath%%runName%\RunNum%VSPDRunNum%_IslandOutputRDA_TP.gdx"

$if %PivotAnalysis%==-1 $LOAD RDAScenario ActiveRDAScenario

$if %PivotAnalysis%==-1 $LOAD o_IslandScenarioMW o_IslandScenarioPrice o_IslandBaseMW o_IslandBasePrice 

$if %PivotAnalysis%==-1 $LOAD o_IslandBaseGWAP o_IslandScenarioGWAP o_IRDE_refprice o_IRDE_gwap o_QWOP o_QWOPcleared o_maxPcleared

*Close the gdx
$if %PivotAnalysis%==-1 $GDXIN

FILES
IslandRDAResults_TP        / "%OutputPath%%runName%\%runName%_IslandResDemandResults_TP.csv" / 
;


display o_QWOP;


IslandRDAResults_TP.pc = 5;        IslandRDAResults_TP.lw = 0;       IslandRDAResults_TP.pw = 9999;            IslandRDAResults_TP.ap = 1;       
IslandRDAResults_TP.nd = 5;

put IslandRDAResults_TP;
loop((dim1,dim2,dim3) $ (o_DateTime(dim1) and o_Island(dim1,dim2) and ActiveRDAScenario(dim3)),
*loop((dim1,dim2) $ (o_DateTime(dim1) and o_Island(dim1,dim2)),
  put dim1.tl, dim2.tl, dim3.tl, o_IslandBasePrice(dim1,dim2), o_IslandBaseGWAP(dim1,dim2), o_IslandBaseMW(dim1,dim2), o_IslandScenarioPrice(dim1,dim2), o_IslandScenarioGWAP(dim1,dim2), o_IslandScenarioMW(dim1,dim2), o_IRDE_refprice(dim1,dim2), o_IRDE_gwap(dim1,dim2), o_QWOP(dim1,dim2), o_QWOPcleared(dim1,dim2) 
o_maxPcleared(dim1,dim2) /;
);