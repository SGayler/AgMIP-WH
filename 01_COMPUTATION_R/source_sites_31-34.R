# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#
#    SETUP sites 31-30
#
#    author: Tobias Weber <tobias.weber@uni-hohenheim.de>
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

tpl_xnd   <- tpl[[as.character(ksite)]]
tpl_input <- list()

# 10000 ----
# selects information of kth year
# ATTENTION, the correction of days(10) result from an AgMiP Wheat Phase 4 specific requirement in UPDATE2

# correction factor
corfac <- days(10)

tpl_input$DatumStart <- ymd(data$manag[ksite]$date_sowing) + years(kyear-1) - data$SimStart_before_Sowing
tpl_input$DatumEnde  <- ymd(data$manag[ksite]$date_maturity) + years(kyear-1) + months(1)

# 10001 and 10002----
# in .tpl

# 10003 ----
# $Name    string of 6:  ModelCode (w/o N) Site RCP GCM Trait
tpl_input$'Name' <- paste0(str_remove(kmodel,pattern = "N"), sprintf("%02d",ksite),krcpgcm, ktrait)
tpl_input$'xnw'  <- data$fnames[ksite][[krcpgcm]]
tpl_input$'xnm'  <- data$fnames[ksite, name_xnm]
tpl_input$'xnc'  <- paste0(kmodel,".xnc")

# 10004 ----
# ignored
# 10005 ----
tpl_input$Saattermin         <- tpl_input$DatumStart + data$SimStart_before_Sowing
tpl_input$TerminAuflaufen    <- tpl_input$DatumStart + data$SimStart_before_Sowing + days(14)
tpl_input$TerminErnteNutzung <- tpl_input$DatumEnde

# 10006 ---- 

interim <- data.table("Ausbringungstermin" = c(ymd(data$manag$date_fert1[ksite]) - years(year(data$manag$date_sowing[ksite]) - year(tpl_input$Saattermin)),
                                               ymd(data$manag$date_fert2[ksite]) - years(year(data$manag$date_sowing[ksite]) - year(tpl_input$Saattermin)))
                      , "D?ngerart"             = "Ammonnitrat"
                      , "Code"                  = "FE001"
                      , "Ausbringungsmenge"     = 150
                      , "NitratNGehaltDuenger"  = 75
                      , "AmmoniumNGehaltDuenger"= 75
                      , "AmidNGehalDuenger"     = 0
)
tpl_input$no_min_fert        <- nrow(interim)
interim$Ausbringungstermin   %<>% ymd %>% format(., "%d%m%y")
tpl_input$min_fert_table     <- paste(apply(interim, 1, function(x) paste(x, collapse = "\t")), collapse="\n")
# rm(interim)
# END