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
source("./01_COMPUTATION_R/source_initialisation.R")

# xnm template files
tpl <- list.files("./XNM/", full.names = TRUE, pattern = ".tpl") %>%  
   lapply(., readLines) %>% 
   setNames(., "xnm")

# 1 RUN ----
for(ksite in k$ksite.v){ 
   
   tpl_xnm   <- tpl$xnm
   tpl_input <- list()
   
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
            , "Gesamtporenvolumen", "GesaettigteLeitfaehigkeit")] %>% 
      apply(., 1, formatC) %>% t %>% 
      apply(., 1, function(x) paste(x, collapse = "\t")) %>% 
      paste(., collapse="\n")

   # 10021 -----
   tpl_input$"$10021" <- sqlFetch(con, "StammBodenprofilSchichten") %>%
      data.table %>% 
      .[, c("Schichtnummer",  "ModellgroesseWasserRes", "Gesamtporenvolumen",
            "ModellgroesseAev", "ModellgroesseCampellB", "ModellgroesseVanGenuchtenAlpha",
            "ModellgroesseVanGenuchtenN")] %>% 
      data.table(., "tau" =.5, "q" = 1, "r" = 2) %>% 
      apply(., 1, formatC) %>% t %>% 
      apply(., 1, function(x) paste(x, collapse = "\t")) %>% 
      paste(., collapse="\n")
   
   # the replacement
   mapply(function(x, y) {
      tpl_xnm <<- str_replace(tpl_xnm, pattern = paste0("\\", y), replacement = x) 
      
   }, x = tpl_input,
   y = names(tpl_input)) %>% invisible
   
   # xnm is written to all 8 subfolders
   file.path(path$PROJ_ROOT, k$kmodelktrait.v, "param", data$fnames$name_xnm[ksite])  %>% lapply(., writeLines, text = tpl_xnm) %>%  invisible
   # odbcCloseAll()
}# end ksite
