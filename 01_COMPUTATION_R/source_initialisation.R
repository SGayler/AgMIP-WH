# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#
#    AGMIP WHEAT PHASE 4 Global step Input files
#    SOURCE INITIALISATION 
#    author: Tobias Weber <tobias.weber@uni-hohenheim.de>
#
#    Make sure you use R-32bit. This has been tested for R4.0.2
# 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


rm(list = ls()); gc(); graphics.off()

# 0 LOAD ------ 
# install packages not installed, else just load
if(!require("RODBC")){install.packages("RODBC"); library(RODBC)}
if(!require("magrittr")){install.packages("magrittr"); library(magrittr)}
if(!require("stringr")){install.packages("stringr"); library(stringr)}
if(!require("lubridate")){install.packages("lubridate"); library(lubridate)}
if(!require("data.table")){install.packages("data.table"); library(data.table)}
if(!require("tibble")){install.packages("tibble"); library(tibble)}

# initialise lists
data <- path <- tpl <- k <- list()

path$PROJ_ROOT   <- "./02_COMPUTATION_XN/"
path$SOURCE_ROOT <- "./03_SOURCES/"
path$R_ROOT      <- "./01_COMPUTATION_R/"
path$GTP         <- "./GTP/"
path$XNI         <- "./XNI/"
path$modlib      <- "./modlib/"
if(!dir.exists(path$PROJ_ROOT )){dir.create(path$PROJ_ROOT)}
# read data
data$manag <- fread("./auxFiles/wheat_regions.csv")
data$treat <- fread("./auxFiles/treatment_layout.csv")
data$fnames<- fread("./auxFiles/filenames.csv")

# get full paths of all data bases
path$files.v    <- list.files(path      = "./"
                              , full.names= TRUE
                              , recursive = TRUE
                              , pattern   = ".mdb"
)
# xnd template files
tpl <- lapply(list.files("./XND/", full.names = TRUE), readLines) %>%  setNames(., c("xnd", as.character(31:34)))

# initialise the loops
k$kmodel.v   <- "NC" #c("NC", "NG", "NP", "NS")             # the four models
k$kyear.v    <- 1:30               # 1:30             # the thirty years 1:30
k$ksite.v    <- 1# 1:nrow(data$fnames)                   # the number of sites 1:34
k$krcpbase.v <- unique(data$treat$code_baseline_rcp)[1]  # the rcp and baseline scenarios
k$kgcm.v     <- unique(data$treat$code_gcm)[1]           # the gcm scenarios
k$ktrait.v   <- unique(data$treat$code_trait)[1]         # the simulated traits 
k$year.v     <- k$kyear.v + 1980                      # the harvest years
row.names(data$fnames) <- 1:nrow(data$fnames)

# 1 CREATE SUBFOLDERS 
# silent create subpaths for kmodels x ktraits 
# silent create subpaths for kmodels x ktraits 
k$kmodelktrait.v <- expand.grid(k$kmodel.v, k$ktrait.v) %>%  apply(., 1, paste, collapse="_")
file.path(path$PROJ_ROOT, k$kmodelktrait.v) %>% lapply(., dir.create, showWarnings = FALSE) %>%  invisible

# silent create param folders
file.path(path$PROJ_ROOT, k$kmodelktrait.v, "param")  %>% lapply(., dir.create, showWarnings = FALSE) %>%  invisible

# copy xnm files of sites 31-34 to the  respective param of the respective project folders
lapply(file.path(path$PROJ_ROOT, k$kmodelktrait.v, "param"), function(x){
   file.copy(from = list.files("./XNM/", full.names = TRUE, pattern = ".xnm"), to = x, overwrite = TRUE)})


# # 2 COPY XNC FILES
source(file.path(path$R_ROOT, "copy_xnc.R"))
