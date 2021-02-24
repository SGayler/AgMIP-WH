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


# 0 INITIALISE ----
query          <- list()

query$plot2file <- TRUE
# NULL: queries are used 
# FALSE: none are updated
query$base_only <- FALSE# TRUE: only baseline plotted/considered, else, all rcpgcms will be plotted
# load and source the setup | later, delete this, as it should be done automatically after the simulations
source("./01_COMPUTATION_R/source_initialisation.R")
#

#if(isTRUE(query$base_only )){
#   rfp.dt <- rfp.dt[climate=="0-"] %>% droplevels 
#   
#}

#
#  SET THIS MANUALLY
#
kmodelktrait <- "NG_N"
#
dir.create(file.path(path$PLOT, kmodelktrait), showWarnings = FALSE, recursive = TRUE)

# SET THE BBCH STAGES YOU WISH TO COMPARE TO
data$BBCH$maturity <- 92
data$BBCH$anthesis <- 65

# the skip element in k is used to specify the skipped lines to read the .rfp
k$skip$rfp         <- 32


# initialise data table
rfp.dt             <- data.table()

# get the file names
rfp.dt$paths_rfp   <- file.path(path$PROJ_ROOT, kmodelktrait) %>%  list.files(., recursive = TRUE, full.names = TRUE, pattern = ".rfp")
rfp.dt$names_rfp   <- file.path(path$PROJ_ROOT, kmodelktrait, "result") %>%  list.files(., recursive = TRUE, full.names = FALSE, pattern = ".rfp") %>%  
   str_remove(., ".rfp") %>% str_sub(., 2, str_length(.))

# get sites and climates from output file names
rfp.dt$site    <- rfp.dt$names_rfp %>% str_sub(., 1, 2) %>% as.integer
rfp.dt$climate <- rfp.dt$names_rfp %>% str_sub(., 3, 4) %>% as.factor


if(isTRUE(query$base_only )){
   rfp.dt <- rfp.dt[climate=="0-"] %>% droplevels 
   
}



# prepare an  index for sorting
mod        <- rfp.dt$names_rfp  %>% stri_length
rfp.dt$int <- str_sub(rfp.dt$names_rfp, -(mod%%min(mod) + 1), -1)  %>% as.integer 
rfp.dt$int <- ifelse(rfp.dt$int < 80, rfp.dt$int + 2000, rfp.dt$int + 1900)
# sort
setorderv(rfp.dt, c("site","climate", "int"))

# add data to the data table
rfp.dt <- merge(x = rfp.dt
                , y = data$manag[, c("number_site", "date_sowing", "date_anthesis","date_maturity")]
                , by.x = "site"
                , by.y = "number_site")


# SIM
pb <- txtProgressBar(min = 0, max = nrow(rfp.dt), style = 3)

rfp.dt$SIM_61_DATE <- rep("NAN", nrow(rfp.dt))
rfp.dt$SIM_92_DATE <- rep("NAN", nrow(rfp.dt))
rfp.dt$YIELD       <- rep("NAN", nrow(rfp.dt))
rfp.dt$ABVBM       <- rep("NAN", nrow(rfp.dt))
rfp.dt$NCONC_G     <- rep("NAN", nrow(rfp.dt))
rfp.dt$PROTEIN     <- rep("NAN", nrow(rfp.dt))

for(i in 1:nrow(rfp.dt)){
   
   # read
   int.dt <- fread(rfp.dt[i]$paths_rfp, skip = k$skip$rfp)
   
   # assign
   rfp.dt$SIM_61_DATE[i] <- int.dt$V1[which.min(abs(int.dt$V2 - data$BBCH$anthesis))] 
   rfp.dt$SIM_92_DATE[i] <- int.dt$V1[which.min(abs(int.dt$V2 - data$BBCH$maturity))] 
   rfp.dt$YIELD[i]       <- max(int.dt$V8)     # v8 is colum 8 in rfp file
   #rfp.dt$ABVBM[i]       <- max(int.dt$V7)     # v8 is colum 7 in rfp file 
   rfp.dt$ABVBM[i]       <- int.dt$V7[which.min(abs(int.dt$V2 - data$BBCH$maturity))]     # v8 is colum 7 in rfp file 
   rfp.dt$NCONC_G[i]       <- int.dt$V16[which.min(abs(int.dt$V2 - data$BBCH$maturity))]  
   
   setTxtProgressBar(pb, i)
}; close(pb)

# convert to time
rfp.dt$SIM_61_DATE <- sprintf("%06d", as.numeric(rfp.dt$SIM_61_DATE)) %>%  dmy
rfp.dt$SIM_92_DATE <- sprintf("%06d", as.numeric(rfp.dt$SIM_92_DATE)) %>%  dmy

# convert to time
rfp.dt$date_sowing  <- ymd(rfp.dt$date_sowing) + years(rfp.dt$int - 1981)

rfp.dt[, SIM_61_dt := difftime(SIM_61_DATE, date_sowing, units = "day") %>% as.integer]
rfp.dt[, SIM_92_dt := difftime(SIM_92_DATE, date_sowing, units = "day") %>% as.integer]

#convert to numbers
rfp.dt$YIELD<- as.numeric(rfp.dt$YIELD)/1000
rfp.dt$ABVBM<- as.numeric(rfp.dt$ABVBM)/1000
rfp.dt$HI   <- rfp.dt$YIELD/(rfp.dt$YIELD+rfp.dt$ABVBM)
rfp.dt$PROTEIN <-as.numeric(rfp.dt$NCONC_G)*6.25

# PLOT
{
   graphics.off()
   len       <- length(unique(rfp.dt$climate))/2
   OBS_ant   <- (ymd(data$manag$date_anthesis) - ymd(data$manag$date_sowing))[k$ksite.v]
   OBS_mat   <- (ymd(data$manag$date_maturity) - ymd(data$manag$date_sowing))[k$ksite.v]
   xplot     <- seq(0, length(unique(rfp.dt$climate)) * length(unique(rfp.dt$site))-length(unique(rfp.dt$climate)), length(unique(rfp.dt$climate)))
   xplot     <- (length(unique(rfp.dt$climate))*xplot+1)/length(unique(rfp.dt$climate))
   
   # PLOT ANTHESIS
   if(isTRUE(query$plot2file)){
      png(file.path(path$PLOT,kmodelktrait, paste0(kmodelktrait, "_ANT", ".png"))
          , height= 20
          , width = 50
          , units = "cm"
          , res   = 600)
   }else{
      windows(height = 10, width = 20, xpos = 400)
      par(oma = c(3,3,1,1))
   }

   with(rfp.dt, {
      # SIM   
      bp <<- boxplot(abs(SIM_61_dt) ~ climate +site, col = viridis::viridis(length(unique(climate))),
                     axes = F, ylab = "", xlab = "", ylim = c(0,360)
                     , main = paste("ANTHESIS",kmodelktrait))
   })
   # OBS
   points( OBS_ant~ xplot, col = "black", cex = 1.2, pch = 16)
   segments(x0 = xplot-len, y0 = OBS_ant, x1 = xplot + len, col = "red", lwd = 2)
   
   axis(1, at = xplot, labels = unique(rfp.dt$site),las = 2, cex.axis = 1.5)
   axis(2, at = seq(0,300,30), cex.axis = 1.5)
   mtext("site"   , 1, line = 4, cex = 2)
   mtext("DAS [d]", 2, line = 4, cex = 2)
   legend("topleft", legend = unique(rfp.dt$climate), fill = viridis::viridis(length(unique(rfp.dt$climate))), cex = 2)
   box()
   if(isTRUE(query$plot2file)){dev.off()}
   
   # PLOT MATURITY
   if(isTRUE(query$plot2file)){
      png(file.path(path$PLOT, kmodelktrait, paste0(kmodelktrait, "_MAT", ".png"))
          , height= 20
          , width = 50
          , units = "cm"
          , res   = 600)
   }else{
      windows(height = 10, width = 20, xpos = 400)
      par(oma = c(3,3,1,1))
   }
   with(rfp.dt, {
      # SIM
      bp <<- boxplot(abs(SIM_92_dt) ~ climate +site, col = viridis::viridis(length(unique(climate))),
                     axes = F, ylab = "", xlab = "", ylim = c(0,360)
                     , main = paste("MATURITY",kmodelktrait))
   })
   # OBS
   points( OBS_mat ~ xplot, col = "black", cex = 1.2, pch = 16)
   segments(x0 = xplot-len, y0 = OBS_mat, x1 = xplot + len, col = "red", lwd = 2)
   
   axis(1, at = xplot, labels = unique(rfp.dt$site),las = 2, cex.axis = 1.5)
   axis(2, at = seq(0,360,30), cex.axis = 1.5)
   mtext("site"   , 1, line = 4, cex = 2)
   mtext("DAS [d]", 2, line = 4, cex = 2)
   legend("topleft", legend = unique(rfp.dt$climate), fill = viridis::viridis(length(unique(rfp.dt$climate))), cex = 2)
   
   box()
   if(isTRUE(query$plot2file)){dev.off()}
   
   # PLOT YIELD
   if(isTRUE(query$plot2file)){
      png(file.path(path$PLOT,kmodelktrait, paste0(kmodelktrait, "_YIELD", ".png"))
          , height= 20
          , width = 50
          , units = "cm"
          , res   = 600)
   }else{
      windows(height = 10, width = 20, xpos = 400)
      par(oma = c(3,3,1,1))
   }
   par(oma = c(3, 3, 1, 1))
   with(rfp.dt, {
      # SIM
      bp <<- boxplot(YIELD ~ climate +site, col = viridis::viridis(length(unique(climate))),
                     axes = F, ylab = "", xlab = "", ylim = c(0,25)
                     , main = paste("YIELD",kmodelktrait))
   })
   
   axis(1, at = xplot, labels = unique(rfp.dt$site),las = 2, cex.axis = 1.5)
   axis(2, at = seq(0,20,2), cex.axis = 1.5)
   mtext("site"   , 1, line = 4, cex = 2)
   mtext("Yield [t/ha]", 2, line = 4, cex = 2)
   legend("topleft", legend = unique(rfp.dt$climate), fill = viridis::viridis(length(unique(rfp.dt$climate))), cex = 2)
   
   box()
   if(isTRUE(query$plot2file)){dev.off()}
   
   
   
   # PLOT HI
   if(isTRUE(query$plot2file)){
      png(file.path(path$PLOT,kmodelktrait, paste0(kmodelktrait, "_HI", ".png"))
          , height= 20
          , width = 50
          , units = "cm"
          , res   = 600)
   }else{
      windows(height = 10, width = 20, xpos = 400)
      par(oma = c(3,3,1,1))
   }
   par(oma = c(3, 3, 1, 1))
   with(rfp.dt, {
      # SIM
      bp <<- boxplot(HI ~ climate + site, col = viridis::viridis(length(unique(climate))),
                     axes = F, ylab = "", xlab = "", ylim = c(0.2,.8)
                     , main = paste("Harvest Index", kmodelktrait))
   })
   abline(h = .63, col = "red")
   axis(1, at = xplot, labels = unique(rfp.dt$site),las = 2, cex.axis = 1.5)
   axis(2, at = seq(.2,.8,.1), cex.axis = 1.5)
   mtext("site"   , 1, line = 4, cex = 2)
   mtext("Harvest Index [-]", 2, line = 4, cex = 2)
   legend("topleft", legend = unique(rfp.dt$climate), fill = viridis::magma(length(unique(rfp.dt$climate))), cex = 2)
   
   box()
   if(isTRUE(query$plot2file)){dev.off()}
   
   # PLOT PROTEIN
   if(isTRUE(query$plot2file)){
      png(file.path(path$PLOT,kmodelktrait, paste0(kmodelktrait, "_PROTEIN", ".png"))
          , height= 20
          , width = 50
          , units = "cm"
          , res   = 600)
   }else{
      windows(height = 10, width = 20, xpos = 400)
      par(oma = c(3,3,1,1))
   }
   par(oma = c(3, 3, 1, 1))
   with(rfp.dt, {
      # SIM
      bp <<- boxplot(PROTEIN ~ climate + site, col = viridis::viridis(length(unique(climate))),
                     axes = F, ylab = "", xlab = "", ylim = c(0,20)
                     , main = paste("Protein concentration", kmodelktrait))
   })
   axis(1, at = xplot, labels = unique(rfp.dt$site),las = 2, cex.axis = 1.5)
   axis(2, at = seq(0,20,2), cex.axis = 1.5)
   mtext("site"   , 1, line = 4, cex = 2)
   mtext("concentration [%]", 2, line = 4, cex = 2)
   legend("topleft", legend = unique(rfp.dt$climate), fill = viridis::magma(length(unique(rfp.dt$climate))), cex = 2)
   
   box()
   if(isTRUE(query$plot2file)){dev.off()}
}


## OUTPUT TO FILES
if(isTRUE(query$base_only )){

dcast(rfp.dt, int ~ site, value.var = "YIELD") %>% .[,int:=NULL] %>%  rbind(.,apply(., 2, quantile)) %>%  set_rownames(., paste(c(1981:2010,"0%","25%","50%","75%","100%")))%>%
   fwrite(., file.path(path$PLOT,kmodelktrait, paste0(kmodelktrait, "_YIELD", ".csv")), row.names = TRUE)

dcast(rfp.dt, int ~ site, value.var = "ABVBM") %>% .[,int:=NULL] %>%  rbind(.,apply(., 2, quantile)) %>%  set_rownames(., paste(c(1981:2010,"0%","25%","50%","75%","100%")))%>%
   fwrite(., file.path(path$PLOT,kmodelktrait, paste0(kmodelktrait, "_ABVBM", ".csv")), row.names = TRUE)

dcast(rfp.dt, int ~ site, value.var = "HI") %>% .[,int:=NULL] %>%  rbind(.,apply(., 2, quantile)) %>%  set_rownames(., paste(c(1981:2010,"0%","25%","50%","75%","100%")))%>%
   fwrite(., file.path(path$PLOT,kmodelktrait, paste0(kmodelktrait, "_HI", ".csv")), row.names = TRUE)

dcast(rfp.dt, int ~ site, value.var = "PROTEIN") %>% .[,int:=NULL] %>%  rbind(.,apply(., 2, quantile)) %>%  set_rownames(., paste(c(1981:2010,"0%","25%","50%","75%","100%")))%>%
   fwrite(., file.path(path$PLOT,kmodelktrait, paste0(kmodelktrait, "_PROTEIN", ".csv")), row.names = TRUE)


(apply(dcast(rfp.dt, int ~ site, value.var = "SIM_61_dt") %>% .[,int:=NULL], 1, function(x) x- as.integer(OBS_ant)) %>%  t)%>% 
   rbind(.,apply(., 2, quantile)) %>% data.table %>% 
   set_rownames(., paste(c(1981:2010,"0%","25%","50%","75%","100%")))%>%
   fwrite(., file.path(path$PLOT,kmodelktrait, paste0(kmodelktrait, "_ANT", ".csv")), row.names = TRUE)

(apply(dcast(rfp.dt, int ~ site, value.var = "SIM_92_dt") %>% .[,int:=NULL], 1, function(x) x- as.integer(OBS_mat)) %>%  t)%>%  
   rbind(.,apply(., 2, quantile)) %>% data.table %>% 
   set_rownames(., paste(c(1981:2010,"0%","25%","50%","75%","100%")))%>%
   fwrite(., file.path(path$PLOT,kmodelktrait, paste0(kmodelktrait, "_MAT", ".csv")), row.names = TRUE)

}


# rfp.dt[SIM_61_dt<0]


# cbind(
#      "MTDV" = (sapply(1:34, function(x) mean(OBS_ant[x] - rfp.dt[site==x]$SIM_61_dt)) %>%   round(., 1))
#    , "MTDR" = (sapply(1:34, function(x) mean(OBS_mat[x] - rfp.dt[site==x]$SIM_92_dt)) %>%   round(., 1))
#    ) %>% t %>% set_colnames(., c(1:34)) %>% print





