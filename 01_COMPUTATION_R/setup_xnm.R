# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#
#    SETUP AGMIP WHEAT PHASE 4 Global step Input files
#    XNM
#    author: Tobias Weber <tobias.weber@uni-hohenheim.de>
#
#    Make sure you use R-32bit. This has been tested for R4.0.2
# 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# 0 INITIALISE ----
# source("./01_COMPUTATION_R/source_initialisation.R")

# xnm template files
tpl <- list.files("./XNM/", full.names = TRUE, pattern = ".tpl") %>%  
   lapply(., readLines) %>% 
   setNames(., "xnm")

# 1 RUN ----
for(ksite in k$ksite.v[k$ksite.v%in%c(1:30)]){ 
   
   tpl_xnm   <- tpl$xnm
   tpl_input <- list()
   
   # 10001 -----
   tpl_input$"$10001" <- if(data$mana$water_regime[ksite] =="Irrigated"){ "1" 
   }else if(data$mana$water_regime[ksite] =="Rainfed"){ "0" }
   
   # OLD VERSION FOR NON UPDATED SITES IN UPDATE 2
   if(ksite%in%k$ksite.update2.excl){
      # connect to database
      ksite_file<- path$files.v[ksite]
      con       <- odbcConnectAccess(ksite_file)
      
   # 10011 -----
      interim <- sqlFetch(con, "BewegungStartWerte") %>% data.table %>% unique(., by = "Schichtnummer" ) %>% 
         .[, c("Schichtnummer", "Schichtdicke", "WassergehaltBoden", "Bodenemperatur"
               , "AmmoniumgehaltBoden", "NitratgehaltBoden")]
      
      interim %<>% add_column(., "MatrixPotential" = -99, .after = "WassergehaltBoden")
      interim %<>% add_column(., "Wurzeldichtenverteilung" = 0, .after = "NitratgehaltBoden")
      interim$Schichtdicke %<>% "/"(5)
      
      tpl_input$"$10011" <- apply(interim, 1, formatC) %>% t %>% apply(., 1, function(x) paste(x, collapse = "\t")) %>% paste(., collapse="\n")
      
      # 10020 -----
      tpl_input$"$10020" <- sqlFetch(con, "StammBodenprofilSchichten") %>%
         data.table %>% 
         .[, c("Schichtnummer", "Bodenart", "PermanenterWelkepunkt", "Feldkapazitaet"
               , "Gesamtporenvolumen", "GesaettigteLeitfaehigkeit")]
      
      # 10021 -----
      tpl_input$"$10021" <- sqlFetch(con, "StammBodenprofilSchichten") %>%
         data.table %>% 
         .[, c("Schichtnummer",  "ModellgroesseWasserRes", "Gesamtporenvolumen",
               "ModellgroesseAev", "ModellgroesseCampellB", "ModellgroesseVanGenuchtenAlpha",
               "ModellgroesseVanGenuchtenN")] %>% 
         .[, ModellgroesseVanGenuchtenM := 1-1/ModellgroesseVanGenuchtenN] %>% 
         .[, ModellgroesseCampellB := 7] %>% 
         data.table(.,  "tau" =.5, "q" = 1, "r" = 2)
      
      odbcCloseAll()
   }
   
   if(!ksite%in%k$ksite.update2.excl){
      
      no.layer <- 1:nrow(data$shp[site==ksite])
      # 10011 -----
      tpl_input$"$10011" <-  data$shp[site==ksite][ , .(no.layer, ((hz_bt-hz_tp)/conv$sim_lyr_thickness), (theta_pwp+(theta_fc-theta_pwp)*conv$ICfrac), -99, 10, ICNH4M, ICNO3M, -99 )  ] %>% 
         apply(., 1, formatC) %>% 
         t %>% 
         apply(., 1, function(x) paste(x, collapse = "\t")) %>%
         paste(., collapse = "\n")
     
      # 10020 -----   
      tpl_input$"$10020" <-  data$shp[site==ksite][ , .(no.layer, text = "Sl3", theta_fc, theta_pwp, porosity = THS*1.05, K0)]

      # 10021 -----
      tpl_input$"$10021" <- data$shp[site==ksite][  , .(no.layer, THS, THR,ModellgroesseAev =-99, ModellgroesseCampellB =7, ALF, N, M=1-1/N, L, q = 1, r= 2  )]
   }
   
   ######    REFORMAT   ######
   
   tpl_input$"$10020" %<>% 
      apply(., 1, formatC) %>% t %>% 
      apply(., 1, function(x) paste(x, collapse = "\t")) %>% 
      paste(., collapse = "\n")
   
   tpl_input$"$10021" %<>% 
      apply(., 1, formatC) %>% t %>% 
      apply(., 1, function(x) paste(x, collapse = "\t")) %>% 
      paste(., collapse = "\n")
   
   ######    OUTPUT    #######
   
   # PREPARE OUTPUT, i.e. the replacement
   mapply(function(x, y) {
      tpl_xnm <<- str_replace(tpl_xnm, pattern = paste0("\\", y), replacement = x) 
      
   }, x = tpl_input,
   y = names(tpl_input)) %>% invisible
   
   # OUTPUT, i.e. the the files
   file.path(path$PROJ_ROOT, k$kmodelktrait.v, "param", data$fnames$name_xnm[ksite])  %>% lapply(., writeLines, text = tpl_xnm) %>%  invisible
}

