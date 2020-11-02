# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#
#    SETUP AGMIP WHEAT PHASE 4 Global step Input files
#
#    author: Tobias Weber <tobias.weber@uni-hohenheim.de>
#
#    Make sure you use R-32bit. This has been tested for R4.0.2
# 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


# 0 INITIALISE ----
# source("./01_COMPUTATION_R/source_initialisation.R")

# 1 LOAD ------ 
# xnd template files
tpl <- lapply(list.files("./XND/", full.names = TRUE), readLines) %>%  setNames(., c("xnd", as.character(31:34)))

# 1 RUN ----
pb  <- txtProgressBar(min = 0, max = k$pb_length, style = 3)
k$k <-0

for(kmodel in k$kmodel.v){ 
   for(ksite in k$ksite.v){ 
      
      if(ksite%in%1:30){
         ksite_file<- path$files.v[ksite]
         con       <- odbcConnectAccess(ksite_file)
      }
      
      for(kyear in k$kyear.v){ 
         for(krcpgcm in k$krcpgcm.v){ 
            
            for(ktrait in k$ktrait.v){ 
               
               # 1a CONNECT ------
               # to i'th database
               if(ksite%in%1:30){
                  source("./01_COMPUTATION_R/source_sites_1-30.R")
                  
               } else if(ksite%in%31:34){
                  source("./01_COMPUTATION_R/source_sites_31-34.R")
               } else{
                  stop("ksite out of bounds (1-34")
               }
               
               # 1b.1 Reformatting  Time ----
               # reformat times to XN3 format | HIGHLY VERBOSE
               tpl_input$Saattermin        %<>% ymd %>% format(., "%d%m%y")
               tpl_input$TerminAuflaufen   %<>% ymd %>% format(., "%d%m%y")
               tpl_input$TerminErnteNutzung%<>% ymd %>% format(., "%d%m%y")
               tpl_input$DatumStart        %<>% ymd %>% format(., "%d%m%y")
               tpl_input$DatumEnde         %<>% ymd %>% format(., "%d%m%y")
               
               # 1b.2 Reformatting  list names ----
               tpl_input        <- lapply(tpl_input, format)
               names(tpl_input) <- paste0("$", names(tpl_input))
               # the replacement
               mapply(function(x, y) {
                  tpl_xnd <<- str_replace(tpl_xnd, pattern = paste0("\\", y), replacement = x) 
                  
               }, x = tpl_input,
               y = names(tpl_input)) %>% invisible
               
               rm(tpl_input)
               
               
               writeLines(con = paste0(path$PROJ_ROOT,paste(kmodel,ktrait, sep = "_"),"/", str_remove(kmodel, pattern = "N"), sprintf("%02d", ksite),krcpgcm, ktrait,"_", k$year.v[kyear], ".xnd")
                          , text = tpl_xnd)
               
               k$k <- k$k + 1
               setTxtProgressBar(pb, k$k)      
            }# end ktrait  
         }# end krcpgcm
      }# end kyear  
      odbcCloseAll()
   }# end ksite
}# end kmodel  

message(kmodel, sprintf("%02d",ksite), krcpgcm, ktrait)
