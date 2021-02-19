# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#
#    SETUP AGMIP WHEAT PHASE 4 Global step Input files
#    XNP
#    author: Tobias Weber <tobias.weber@uni-hohenheim.de>
#
#    Make sure you use R-32bit. This has been tested for R4.0.2
# 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


# 0 INITIALISE ----
# source("./01_COMPUTATION_R/source_initialisation.R")


# 1 LOAD ------ 
# xnd template files
tpl <- lapply(list.files("./XNP/", full.names = TRUE, pattern="xnp.tpl"), readLines) %>%  setNames(., "xnp")


# 1 RUN ----
for(kmodel in k$kmodel.v){
   for(ksite in k$ksite.v){ 
      for(krcpgcm in k$krcpgcm.v){ 
         for(ktrait in k$ktrait.v){ 
            tpl_xnp   <- tpl$xnp
            tpl_input <- list()
            # 10111 -----
            tpl_input$"$10111" <-  paste0("10111 ", str_remove(kmodel, pattern = "N"), sprintf("%02d", ksite), krcpgcm, ktrait,"_",k$year.v[k$kyear.v], ".xnd") %>% 
               paste(., collapse="\n")
            
            # the replacement
            mapply(function(x, y) {
               tpl_xnp <<- str_replace(tpl_xnp, pattern = paste0("\\", y), replacement = x) 
               
            }, x = tpl_input
            ,  y = names(tpl_input)
            ) %>% invisible
            
            # xnp is written to all 8 subfolders
            writeLines(con = paste0(path$PROJ_ROOT, paste(kmodel, ktrait, sep = "_"),"/", str_remove(kmodel, pattern = "N"), sprintf("%02d", ksite),krcpgcm, ktrait, ".xnp")
                       , text = tpl_xnp)
         }# end ktrait
      }# end krcpgcm
   }# end ksite
}# end kmodel
