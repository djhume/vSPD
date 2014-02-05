*This code creates the output report for residual demand analysis

Sets
RDAScnr(*)
ActRDAScnr(*)
;

Parameters
o_IldScnrMW(*,*)
o_IldScnrPrice(*,*)
o_IldBaseMW(*,*)
o_IldBasePrice(*,*)
o_IldBaseGWAP(*,*)
o_IldScnrGWAP(*,*)
o_IRDE_refprice(*,*)
o_IRDE_gwap(*,*)
o_QWOP(*,*)
o_QWOPcleared(*,*)
o_maxPcleared(*,*)
;


*Pivot offer data
$if %PivotAnalysis%==-1 $GDXIN "%OutputPath%%runName%\RunNum%VSPDRunNum%_IslandOutputRDA_TP.gdx"

$if %PivotAnalysis%==-1 $LOAD RDAScnr ActRDAScnr

$if %PivotAnalysis%==-1 $LOAD o_IldScnrMW o_IldScnrPrice o_IldBaseMW o_IldBasePrice 

$if %PivotAnalysis%==-1 $LOAD o_IldBaseGWAP o_IldScnrGWAP o_IRDE_refprice o_IRDE_gwap o_QWOP o_QWOPcleared o_maxPcleared

*Close the gdx
$if %PivotAnalysis%==-1 $GDXIN

FILES
IldRDAResults_TP        / "%OutputPath%%runName%\%runName%_IldResDemandResults_TP.csv" / 
;


display o_QWOP;


IldRDAResults_TP.pc = 5;        IldRDAResults_TP.lw = 0;       IldRDAResults_TP.pw = 9999;            IldRDAResults_TP.ap = 1;       
IldRDAResults_TP.nd = 5;

put IldRDAResults_TP;
loop((dim1,dim2,dim3) $ (o_DateTime(dim1) and o_Island(dim1,dim2) and ActRDAScnr(dim3)),
*loop((dim1,dim2) $ (o_DateTime(dim1) and o_Ild(dim1,dim2)),
  put dim1.tl, dim2.tl, dim3.tl, o_IldBasePrice(dim1,dim2), o_IldBaseGWAP(dim1,dim2), o_IldBaseMW(dim1,dim2), o_IldScnrPrice(dim1,dim2), o_IldScnrGWAP(dim1,dim2), o_IldScnrMW(dim1,dim2), o_IRDE_refprice(dim1,dim2), o_IRDE_gwap(dim1,dim2), o_QWOP(dim1,dim2), o_QWOPcleared(dim1,dim2) 
o_maxPcleared(dim1,dim2) /;
);
