# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#
#    SETUP AGMIP WHEAT PHASE 4 Global step Input files
#     Copy XNCs
#    author: Tobias Weber <tobias.weber@uni-hohenheim.de>
#
#    Make sure you use R-32bit. This has been tested for R4.0.2
# 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# 0 LOAD
# silent create subpaths for kmodels x ktraits 

lapply(file.path(path$PROJ_ROOT, k$kmodelktrait.v), function(x){
   list.files("./XNC/", full.names = TRUE, pattern = ".xnc") %>% file.copy(. 
             , to = x, overwrite = TRUE)})
message("copied xnc files to subfolders")
