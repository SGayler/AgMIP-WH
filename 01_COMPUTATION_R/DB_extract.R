
# make connection
# con <- odbcConnectAccess("myaccessDB.mdb")
# list tables
# sqlTables(con)


rm(list = ls()); gc(); graphics.off()

# 0 LOAD ------ 
# install package RODBC and load if not installed
if(!require("RODBC")){install.packages("RODBC"); library(RODBC)}
if(!require("magrittr")){install.packages("magrittr"); library(magrittr)}
if(!require("stringr")){install.packages("stringr"); library(stringr)}
if(!require("lubridate")){install.packages("lubridate"); library(lubridate)}
if(!require("data.table")){install.packages("data.table"); library(data.table)}

# set working directory
setwd("C:/Projects/AgMiP/WHEAT/Phase_4/global_step_1/")

# initialise lists
data  <- path <-tpl<- k <- list()

path$PROJ_ROOT <- "./02_COMPUTATION_XN/"
if(!dir.exists(path$PROJ_ROOT )){dir.create(path$PROJ_ROOT )}
# read data
data$manag <- fread("./auxFiles/wheat_regions.csv")
data$treat <- fread("./auxFiles/treatment_layout.csv")
data$fnames<- fread("./auxFiles/filenames.csv")

   # get full paths of all data bases
   files.v    <- list.files("./00_DATA"
                            , full.names= TRUE
                            , recursive = TRUE
                            , pattern    = ".mdb")
# xnd template files
tpl <- lapply(list.files("./00_DATA/XND/", full.names = TRUE) ,readLines) %>%  setNames(., c("xnd", as.character(31:34)))

lapply()

# initialise the loops
k$kmodel.v   <- c("NC", "NG", "NP", "NS")
k$kyear.v    <- 1:30
k$ksite.v    <- 1:2 # nrow(data$fnames) 
k$krcpbase.v <- unique(data$treat$code_baseline_rcp)
k$kgcm.v     <- unique(data$treat$code_gcm)
k$ktrait.v   <- unique(data$treat$code_trait)

row.names(data$fnames) <- 1:nrow(data$fnames)
# for testing
kmodel   <- "NC"
kyear    <- 10
ksite    <- 1
krcpbase <- "0"
kgcm     <- "-"
ktrait   <- "N"
# In the order of the submission
for(kmodel in k$kmodel.v){ 
   if(!dir.exists( file.path(path$PROJ_ROOT,kmodel ))){dir.create(file.path(path$PROJ_ROOT,kmodel ) )}
   for(ksite in k$ksite.v){ 
      
      for(kyear in k$kyear.v){ 
         for(krcpbase in k$krcpbase.v){ 
            for(kgcm in k$kgcm.v){ 
               test <- paste0(krcpbase,kgcm) %in% c("0-","G1","G2","GK","GO","GR","I1","I2","IK","IO","IR")
               if(test){
                  for(ktrait in k$ktrait.v){ 
                     # 1 CONNECT ------
                     # to i'th database
                     if(ksite%in%1:30){
                        ksite_file<- list.files("./", pattern = paste0(data$fnames[ksite]$name_database, ".mdb")
                                                , recursive = TRUE
                                                , full.names = TRUE)
                        con       <- odbcConnectAccess(ksite_file)
                        source("./01_COMPUTATION_R/source_sites_1-30.R")
                        odbcCloseAll()
                     }
                     if(ksite%in%31:34){
                        source("./01_COMPUTATION_R/source_sites_31-34.R")
                     }else{
                        stop("ksite out of bounds (1-34")
                     }
                     # REPLACEMENT  ----
                     
                     # preliminaries
                     
                     # reformat times to XN3 format | HIGHLY VERBOSE
                     tpl_input$Saattermin        %<>% ymd %>% format(., "%d%m%y")
                     tpl_input$TerminAuflaufen   %<>% ymd %>% format(., "%d%m%y")
                     tpl_input$TerminErnteNutzung%<>% ymd %>% format(., "%d%m%y")
                     tpl_input$DatumStart        %<>% ymd %>% format(., "%d%m%y")
                     tpl_input$DatumEnde         %<>% ymd %>% format(., "%d%m%y")
                     
                     # reformatting
                     tpl_input        <- lapply(tpl_input, format)
                     names(tpl_input) <- paste0("$", names(tpl_input))
                     # the replacement
                     mapply(function(x, y) {
                        tpl_xnd <<- str_replace(tpl_xnd, pattern = paste0("\\", y), replacement = x) 
                        
                     }, x = tpl_input,
                     y = names(tpl_input)) %>% invisible
                     
                     rm(tpl_input)
                     writeLines(con = paste0(path$PROJ_ROOT,kmodel,"/", str_remove(kmodel, pattern = "N"), sprintf("%02d", ksite),krcpbase, kgcm, ktrait,"_", k$year.v[kyear], ".xnd")
                                , text = tpl_xnd)
                  }# end ktrait  
               }
            }# end kgcm 
         }# end krcpbase
      }# end kyear    
   }# end ksite
}# end kmodel  

message(kmodel, ksite, krcpbase, kgcm, ktrait)

# sqlTables(con)$TABLE_NAME




