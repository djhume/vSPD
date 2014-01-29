*Set AC branch constraint factors to zero if i_UseACBranchLimits = 0 when in Residual demand mode

BranchConstraintFactors(CurrentTradePeriod,i_BranchConstraint,i_Branch) $ (ACBranch(CurrentTradePeriod,i_Branch) and (i_ExcludeACBranchLimits)) = 0;