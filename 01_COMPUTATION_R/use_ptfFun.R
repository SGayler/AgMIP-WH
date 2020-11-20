# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#
#    Call PTFFun with BUEK attributes
#    predict SHPs
#    author: Tobias Weber <tobias.weber@uni-hohenheim.de>
#
#    This script was adapted from Tobias Template, works only, with Tobias PC :)
#    I have used a mixture of euptf2 and and Rosetta RM9, whenever a model predicts better sldul and sllul,
#    it is correspondonly used. If undecided, then RM9 was used.
# 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# 0 Initialise ----------------------------------------------------------
# detach("package:euptf2lite", unload  = TRUE)
if(!require("magrittr"))   {install.packages("magrittr");library(magrittr)}
if(!require("data.table"))   {install.packages("data.table");library(data.table)}
if(!require("spsh"))   {install.packages("spsh");library(spsh)}
if(!require("soiltexture")){install.packages("soiltexture");library(soiltexture)}
if(!require("qdapTools"))  {install.packages("qdapTools");library(qdapTools)}
if(!require("euptf2lite")) {install.packages("C:/Users/tkdweber/PowerFolders/Coding/R/myprojects/PTF/euptf2.0/", repos = NULL, type="source")}


source("C:/Users/tkdweber/PowerFolders/Coding/R/myprojects/PTF/stochastic_ptf/PTFfun.R")

{
   settings <- list()
   
   # For ROSETTA only
   settings$myROSETTApath  <- "C:/Users/tkdweber/PowerFolders/Coding/R/myprojects/PTF/Rosetta"
   settings$INPUTFILENAME  <- "input_template"
   settings$OUTPUTFILENAME <- "input_template_output"
   
   # Adjustments to PTFs: 0 is off
   settings$capKS        <- 10           # cap KS value to a minimum of     !!!!!!!!! [cm d-1] !!!!!!!!!!!
   settings$capN         <- 1.1          # cap van Genuchten N
   settings$adjSC        <- FALSE        # TRUE/FALSE adjust THETA_S and KS linearly with Stone Content.
   settings$OM2OC        <- 1            # conversion factor from soil organic matter to soil organic carbon
   settings$sum.corr     <- TRUE         # TRUE/FALSE correct texture analysis which does not sum to 100%
   settings$classPTFonly <- FALSE        # TRUE/FALSE if only class is given, certain functions aren't possibly in PTFfun
}

# 1 Read ------------------------------------------------------------------
# _1a AgMiP Wheat Phase 4 global step 1 -----------------------------------

# _1b AMMERTAL attributes -----------------------------------------------------------
DF <- fread( "./00_DATA/soil_properties_noNA.csv")

# DF[is.na(DF$caco3), "caco3"] <- mean(DF$caco3, na.rm = TRUE)

X <- cbind.data.frame(    "sam_id" = 1:nrow(DF)
                          , "ped_id" = 1:nrow(DF)
                          , "hz_tp"  = DF$hz_tp
                          , "hz_bt"  = DF$hz_bt
                          , "SAND"   = DF$USSAND                        # Vol-%
                          , "SILT"   = DF$USSILT                        # Vol-%
                          , "CLAY"   = DF$USCLAY                        # Vol-%
                          , "BD"     = DF$BD                            # g cm-3
                          , "OC"     = DF$OC * settings$OM2OC           # MASS-%
                          , "FK_25"  = DF$sldul                         # [-]
                          , "WP_42"  = DF$slll                          # [-]
                          , "TEXT"   = -99
                          , "USDA"   = -99
                          , "CACO3"  = -99 
)


X.RM9      <- PTFfun(PTF.model = "RM9",
                     X,
                     settings = settings)

dsl1 <- apply(X.RM9[,c(1,2,3,4,5,7)],1, function(x) spsh::shypFun.01110(x, h=c(330,15000))$theta )  %>% 
   "-"(rbind(DF$sldul,DF$slll)) %>% 
   t 

library(euptf2)
which_PTF(predictor = DF, target = "MVG")
DF <- as.data.frame(DF)

X.euptf <- euptf2::euptfFun(ptf = "PTF02", predictor = DF,  target = "MVG", query = "predictions")

dsl2 <- apply(X.euptf[,c(3,2,4,5,6,7)],1, function(x) spsh::shypFun.01110(x, h=c(330,15000))$theta )  %>% 
   "-"(rbind(DF$sldul,DF$slll)) %>% 
   t 

# TRUE means RM9 is better, in doubt RM9, ot euptf2.02
query <- apply(abs(dsl1)<abs(dsl2), 1, any) %>% t %>% t

X.euptf[query, c(3,2,4,5,6,7)] <- X.RM9[query,c(1,2,3,4,5,7)] 

fwrite(X.euptf, "./00_DATA/SHP_VGM_noNA.csv")

