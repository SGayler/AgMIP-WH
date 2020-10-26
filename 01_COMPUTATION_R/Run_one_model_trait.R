# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#
#    RUN AGMIP WHEAT PHASE 4 Global step Input files
#
#    author: Tobias Weber <tobias.weber@uni-hohenheim.de>
#
#    Make sure you use R-32bit. This has been tested for R4.0.2
# 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# 0 INITIALISE ----
source("./01_COMPUTATION_R/source_initialisation.R")

# RUN MODEL and Trait combination
kmodeltrait<- k$kmodelktrait.v[1]

# xni template files
tpl      <- list.files("./XNI/", full.names = TRUE, pattern = "xni.tpl") %>%  lapply(., readLines) %>% setNames(., "xni")
k$kxnp.v <- file.path(path$PROJ_ROOT, kmodeltrait) %>% list.files(., pattern = ".xnp")

# 1 RUN ----
path$ProjectDir <- file.path(getwd(), str_remove(path$PROJ_ROOT, "./"), kmodeltrait)



for(kxnp in k$kxnp.v){
   
   # 1a write xni
   tpl_xni   <- tpl$xni
   tpl_input <- list()
   
   tpl_input$"$LastProject" <- kxnp
   tpl_input$"$ProjectDir"  <- path$ProjectDir 
   
   # the replacement
   mapply(function(x, y) {
      tpl_xni <<- str_replace(tpl_xni, pattern = paste0("\\", y), replacement = x) 
      
   }, x = tpl_input,
   y = names(tpl_input)) %>% invisible
   
   writeLines(con = file.path(path$SOURCE_ROOT, kmodeltrait, "ExpertN.xni")
              , text = tpl_xni)
   
   # 1b copy .gtp
   file.copy(
      from = list.files(file.path(path$GTP, paste0("N", substring(kxnp, 1,1)), substring(kxnp, 6,6)), recursive = TRUE, full.names = TRUE, pattern = paste0("N",substring(kxnp, 1,3), substring(kxnp, 6, 6), ".gtp"))
      , to = file.path(path$PROJ_ROOT, kmodeltrait, "param", "wheat.gtp")
      , overwrite = TRUE
   )   
   # 1c copy modlib
   if(data$manag$water_regime[as.numeric(substring(kxnp, 2,3))] == "Irrigated"){
      with_UW <- "_UW"
   }else if(data$manag$water_regime[as.numeric(substring(kxnp, 2,3))] == "Rainfed"){
      with_UW <- ""
   }
   
   file.copy(
      from = file.path(path$modlib, paste0("modlib_", substring(kxnp, 6,6), "", ".dll"))
      , to = file.path(path$SOURCE_ROOT, kmodeltrait, "modlib.dll")
      , overwrite = TRUE
   )
   
   # 1d Run ExperN
   
   
}


