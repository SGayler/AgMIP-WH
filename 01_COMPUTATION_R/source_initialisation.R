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

if(all(!is.logical(unlist(query)))){
   stop("setup.query must either be FALSE or TRUE")
}

# 0 LOAD ------ 
# install packages not installed, else just load
if(!require("RODBC")){install.packages("RODBC"); library(RODBC)}
if(!require("magrittr")){install.packages("magrittr"); library(magrittr)}
if(!require("stringr")){install.packages("stringr"); library(stringr)}
if(!require("lubridate")){install.packages("lubridate"); library(lubridate)}
if(!require("data.table")){install.packages("data.table"); library(data.table)}
if(!require("tibble")){install.packages("tibble"); library(tibble)}
if(!require("stringi")){install.packages("stringi"); library(stringi)}
if(!require("viridis")){install.packages("viridis"); library(viridis)}

rm(list = ls() %>% grep(., pattern = "query|run_base_only", value = TRUE, invert = TRUE)); gc(); graphics.off()

# initialise lists
data <- path <- tpl <- k <- list()

path$DATA        <- "./00_DATA"
path$R_ROOT      <- "./01_COMPUTATION_R/"
path$PROJ_ROOT   <- "./02_COMPUTATION_XN/"
path$SOURCE_ROOT <- "./03_SOURCES/"
path$GTP         <- "./GTP/"
path$XNI         <- "./XNI/"
path$XNW         <- "./Wetterdateien/"
path$MODLIB      <- "./MODLIB/"
path$XNC         <- "./XNC/"
path$XND         <- "./XND/"
path$XNP         <- "./XNP/"
path$XNM         <- "./XNM/"

if(!dir.exists(path$PROJ_ROOT )){dir.create(path$PROJ_ROOT)}
# read data
data$manag <- fread(file.path(path$DATA, "wheat_regions.csv"   ))
data$treat <- fread(file.path(path$DATA, "treatment_layout.csv"))
data$fnames<- fread(file.path(path$DATA, "filenames.csv"       ))

# get full paths of all data bases
path$files.v    <- list.files(path      = "./"
                              , full.names= TRUE
                              , recursive = TRUE
                              , pattern   = ".mdb"
)
# xnd template files
tpl <- lapply(list.files("./XND/", full.names = TRUE), readLines) %>%  setNames(., c("xnd", as.character(31:34)))

# initialise the loops
k$kmodel.v   <- "NP" # c("NC", "NG", "NP", "NS")         # the four models
k$kyear.v    <- 1:30#15:25                               # 1:30                # the thirty years 1:30
k$ksite.v    <- 1:34                                     # the number of sites 1:34, k$ksite.v    <- 1:nrow(data$fnames)
k$krcpgcm.v  <- c("0-","G1","G2","GK","GO","GR","I1","I2","IK","IO","IR")[2:11]
k$ktrait.v   <- unique(data$treat$code_trait)[1]         # the simulated traits 
k$pb_length  <- lapply(k, length) %>% unlist %>% prod    # length of the progressbar
# hard set for AgMiP WHEAT Pahse 4
k$year.v     <- 1981:2010                                # the harvest years
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


# 2 CHECKS
if(!(c("Irrigated", "Rainfed") %in% unique(data$manag$water_regime ) %>% all)){
   stop("Water regime is ill-specified, has to be uniquely Irrgated or Rainged")
}

# 3 setup
#all
if(isTRUE(query$all)){
   query %<>%  lapply(., function(x) return(TRUE))
}
if(isFALSE(query$all)){
   query %<>%  lapply(., function(x) return(FALSE))
}
if(is.null(query$all) | isTRUE(query$all)){
   #xnm
   if(isTRUE(query$xnm)){
      message("Setting up XNM files")
      source(file.path(path$R_ROOT, "setup_xnm.R")) 
   }
   #xnd
   if(isTRUE(query$xnd)){
      message("Setting up XND files")
      source(file.path(path$R_ROOT, "setup_xnd.R"))
   }
   #xnp
   if(isTRUE(query$xnp)){
      message("Setting up XNP files")
      source(file.path(path$R_ROOT, "setup_xnp.R"))
   }
}   

message("copying XNC files")
source(file.path(path$R_ROOT, "copy_xnc.R"))

message(paste("UPDATE modlib.dll in", path$SOURCE_ROOT))
source(file.path(path$R_ROOT, "copy_modlib.R"))


