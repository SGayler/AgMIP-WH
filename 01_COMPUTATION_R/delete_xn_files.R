# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#AgMip_WH
#    SETUP AGMIP WHEAT PHASE 4 Global Step 
#    
#    DELETE files   CAREFULL!!!
#
#    author: Tobias Weber <tobias.weber@uni-hohenheim.de>
#
#    Make sure you use R-32bit. This has been tested for R4.0.2
# 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
path <- list()
path$DELETE_ROOT <- "./02_COMPUTATION_XN/NC_N/"
if(!require("magrittr")){install.packages("magrittr"); library(magrittr)}
# 1 XND
list.files(path$DELETE_ROOT, full.names = TRUE, recursive = TRUE, pattern = ".xnd") %>%  unlink

# 2 XNP
list.files(path$DELETE_ROOT, full.names = TRUE, recursive = TRUE, pattern = ".xnp") %>%  unlink

# 3 XNM
list.files(path$DELETE_ROOT, full.names = TRUE, recursive = TRUE, pattern = ".xnm") %>%  unlink

# 4 result files
list.files(path$DELETE_ROOT, full.names = TRUE, recursive = TRUE, pattern = ".rf*|.bal|.mea|.log|.hp") %>%  unlink
