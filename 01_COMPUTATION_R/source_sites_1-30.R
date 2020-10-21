

tpl_xnd   <- tpl$xnd
tpl_input <- list()

# 10000 ----
# selects information of kth year
tpl_input <-  sqlFetch(con, "BewegungPflanzendaten")[Saattermin = tpl_input$DatumStart][kyear, c("Sorte", "Saattiefe", "Reihenabstand", "Saatstaerke")]
   # 10001 and 10002----
# *Farm Data
# hard coded to "1", since identical
tpl_input <- sqlFetch(con, "StammBetrieb")[c("Laengengrad", "Breitengrad")][1, ] %>% c(., tpl_input)
tpl_input <- sqlFetch(con, "StammTeilschlag")[c("Hoehelage", "NameBodenprofil", "Hangneigung", "Exposition")][1,] %>% 
   c(., tpl_input)
# 10003 ----
# $Name    string of 6:  ModelCode (w/o N) Site RCP GCM Trait
tpl_input$'Name' <- paste0(str_remove(kmodel,pattern = "N"), sprintf("%02d",ksite),krcpbase, kgcm, ktrait)
tpl_input$'xnw'  <- data$fnames[ksite][[paste0(krcpbase,kgcm)]]
tpl_input$'xnm'  <- data$fnames[ksite,name_xnm]
tpl_input$'xnc'  <- paste0(kmodel,".xnc")

# 10004 ----
# ignored 
# 10005 ----
tpl_input <-  sqlFetch(con, "BewegungPflanzendaten") %>% data.table %>% 
   .[ Saattermin %in% tpl_input$DatumStart, c("Sorte", "Saattiefe", "Reihenabstand", "Saatstaerke"
                                              ,"Saattermin", "TerminAuflaufen", "TerminErnteNutzung"
                                              , "MaxDurchwurzelungstiefe")] %>% 
   .[1] %>% 
   c(., tpl_input)

# correct 
tpl_input$Saattiefe     %<>% "/"(100)
tpl_input$Reihenabstand %<>% "/"(100)
tpl_input$MaxDurchwurzelungstiefe %<>% "/"(100)
# 10006 ---- 
##### OLD 
# interim        <- data.table(sqlFetch(con, "BewegungMineraldüngung"))[Ausbringungstermin %between% c(tpl_input$Saattermin, tpl_input$TerminErnteNutzung)] %>%
#    unique(., by = "Ausbringungstermin")
# interim$Ausbringungstermin   %<>% ymd %>% format(., "%d%m%y")
# query.fert_min <- sqlFetch(con, "TabelleDüngerMineralisch")[c("NameDuenger", "Code")]
# interim        %<>%  merge(., query.fert_min, by.x = "Düngerart", by.y =  "NameDuenger" )
# interim        <- interim[, c("Ausbringungstermin", "Düngerart", "Code", "Ausbringungsmenge", "NitratNGehaltDuenger", "AmmoniumNGehaltDuenger", "AmidNGehalDuenger")]

##### NEW
interim <- data.table("Ausbringungstermin" = c(ymd(data$manag$date_fert1[ksite]) - years(year(data$manag$date_sowing[ksite]) - year(tpl_input$Saattermin)),
                                               ymd(data$manag$date_fert2[ksite]) - years(year(data$manag$date_sowing[ksite]) - year(tpl_input$Saattermin)))
                      , "Düngerart"             = "Ammonnitrat"
                      , "Code"                  = "FE001"
                      , "Ausbringungsmenge"     = 150
                      , "NitratNGehaltDuenger"  = 75
                      , "AmmoniumNGehaltDuenger"= 75
                      , "AmidNGehalDuenger"     = 0
)
tpl_input$no_min_fert <- nrow(interim)
tpl_input$min_fert_table <-  paste(apply(interim, 1, function(x) paste(x, collapse = " ")), collapse="\n")
rm(interim)

# 10010 ----
interim        <- data.table(sqlFetch(con, "StammBodenprofilSchichten"))
interim        <- interim[, c("Schichtnummer", "AnzSimSchichten", "Tongehalt", "Schluffgehalt","Sandgehalt"
                              , "GehaltOrganischerKohlenstoff","Lagerungsdichte", "Steingehalt","Ph"
)]
tpl_input$no_soil_lyr   <- sum(interim$AnzSimSchichten )
tpl_input$soil_lyr_table<-  paste(apply(interim, 1, function(x) paste(x, collapse = " ")), collapse="\n")

# 10011 ----
interim        <- data.table(sqlFetch(con, "BewegungStartwerte"))[Termin %between% c(tpl_input$Saattermin, tpl_input$TerminErnteNutzung)] %>% 
   unique(., by = c("Termin", "Schichtnummer"))
interim$Termin   %<>% ymd %>% format(., "%d%m%y")
interim        <- interim[, c("Schichtnummer", "Schichtdicke", "WassergehaltBoden", "Bodenemperatur"
                              , "AmmoniumgehaltBoden","NitratgehaltBoden"
)]
interim$Matrixpotential <- "-99"
interim$RootDensity     <- "-99"
interim$Schichtdicke    %<>% "/"(5)

interim <- interim[,c("Schichtnummer", "Schichtdicke", "WassergehaltBoden","Matrixpotential",
                      "Bodenemperatur", "AmmoniumgehaltBoden", "NitratgehaltBoden", "RootDensity")]
tpl_input$soil_ic_table <-  paste(apply(interim, 1, function(x) paste(x, collapse = " ")), collapse="\n")
# END
