*RDN - This creates the pivot results output template

   put IslandPivotResults_TP;
   put 'DateTime', 'Island', 'Scenario', 'GenMW', 'LoadMW', 'IslandACLossMW', 'HVDCFlowMW', 'HVDCLossMW', 'ReferencePrice'
       'FIRMW', 'SIRMW', 'FIRPrice', 'SIRPrice', 'EnergyOfferedMW', 'FIROfferedMW', 'SIROfferedMW', 'PivotEnergyOfferMW', 'PivotFIROfferMW', 'PivotSIROfferMW'
        'PivotScheduleMW', 'PivotFKminMW', 'PivotMW', 'PivotFIRClearedMW', 'PivotSIRClearedMW'
        'PosLoadMW', 'GWAP', 'LWAP', 'LoadAtPivotPriceMW', 'PivotTraderTotGenMW', 'PivotGenMaxNodePrice';