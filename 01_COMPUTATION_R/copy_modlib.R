# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#
#    SETUP AGMIP WHEAT PHASE 4 Global step Input files
#     Copy modlib
#    author: Tobias Weber <tobias.weber@uni-hohenheim.de>
#
#    Make sure you use R-32bit. This has been tested for R4.0.2
# 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# 0 LOAD
# silent create subpaths for kmodels x ktraits 

# create subfolders
lapply(file.path(path$SOURCE_ROOT, k$kmodelktrait.v), dir.create, showWarnings = FALSE) %>%  invisible

# copy modlib. dll.
lapply(file.path(path$SOURCE_ROOT, k$kmodelktrait.v), function(x){
   list.files(path$MODLIB, full.names = TRUE, pattern = ".dll") %>% 
      file.copy(., to = x, overwrite = TRUE)}) %>%  invisible

# copy .dll
lapply(file.path(path$SOURCE_ROOT, k$kmodelktrait.v), function(x){
   list.files(file.path(path$SOURCE_ROOT,"_TEMPLATE"), full.names = TRUE, pattern = ".dll") %>% 
      file.copy(., to = x, overwrite = TRUE)}) %>%  invisible

# copy .exe if it does not exist
lapply(file.path(path$SOURCE_ROOT, k$kmodelktrait.v)[!file.exists(file.path(path$SOURCE_ROOT,  k$kmodelktrait.v, "expertn.exe"))], function(x){
   list.files(file.path(path$SOURCE_ROOT,"_TEMPLATE"), full.names = TRUE, pattern = ".exe") %>% 
      file.copy(., to = x, overwrite = TRUE)}) %>%  invisible


# create the LIB folder
lapply(file.path(path$SOURCE_ROOT, k$kmodelktrait.v, "LIB"), dir.create, showWarnings = FALSE) %>%  invisible
# copy the lib folder and contents
lapply(file.path(path$SOURCE_ROOT, k$kmodelktrait.v, "LIB"), function(x) file.copy(from = file.path(path$SOURCE_ROOT,"_TEMPLATE", "LIB"), to = x, recursive = TRUE, overwrite = TRUE ))  %>%  invisible

message("copied modlib.dll, and other .dll, LIB, .exe to source subfolders")
message("remember: expertn.exe is only copied, if it is not in dedicated folder")


          