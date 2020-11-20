# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#
#    RUN AGMIP WHEAT PHASE 4 Global step Input files
#
#    author: Tobias Weber <tobias.weber@uni-hohenheim.de>
#
#    Make sure you use R-32bit. This has been tested for R4.0.2
# 
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #

# 0 INITIALISE ----
query         <- list()
   
query$all     <- TRUE   # TRUE: all made new 
                         # NULL: queries are used and selected input files created
                         # FALSE: none are updated
   
query$xnp     <- TRUE    # TRUE: xnp is made new
query$xnd     <- TRUE   # TRUE: xnd is made new
query$xnm     <- TRUE    # TRUE: xnm is made new

run_base_only <- FALSE
# load and source the setup
source("./01_COMPUTATION_R/source_initialisation.R")

# RUN specific kmodel and ktrait combination
kmodeltrait <- "NG_N"

# xni template files
tpl         <- list.files(path$XNI, full.names = TRUE, pattern = "xni.tpl") %>%  lapply(., readLines) %>% setNames(., "xni")
# get the xnp of the kmodeltrait combination

#alle Sites:
k$kxnp.v    <- file.path(path$PROJ_ROOT, kmodeltrait) %>% list.files(., pattern = ".xnp")

#ausgewaehlte Sites:
query$pattern<- NULL#c("G1")#c("02","04", "08", "13", "15", "23", "26", "28") # c(2, 10, 13, 21, 24, 26, 27, 28, 29, 31, 34)    # NULL = no pattern query$pattern <- c(30:34, "01", "G1"), (oder/auch mit sprintf($02d,3:7))
k$kxnp.v    <- file.path(path$PROJ_ROOT, kmodeltrait) %>% list.files(., pattern = paste0(paste0(query$pattern, ".*.xnp"), collapse = "|")) %>%
   grep(k$kxnp.v , pattern = ".xnp", value = TRUE )

if(isTRUE(run_base_only == TRUE)){
   k$kxnp.v <-  k$kxnp.v[ str_detect(k$kxnp.v, pattern = "0-", negate = FALSE) ]
}

# 1 RUN ----
path$ProjectDir <- file.path(getwd(), str_remove(path$PROJ_ROOT, "./"), kmodeltrait)

for(kxnp in k$kxnp.v){
   
   # 1a write xni
   tpl_xni   <- tpl$xni
   tpl_input <- list()
   
    ksite <- substring(kxnp, 2,3) %>% as.integer
   
   tpl_input$"$LastProject" <- kxnp
   tpl_input$"$ProjectDir"  <- path$ProjectDir 
   
   ## the replacement
   mapply(function(x, y) {
      tpl_xni <<- str_replace(tpl_xni, pattern = paste0("\\", y), replacement = x) 
      
   }, x = tpl_input,
   y = names(tpl_input)) %>% invisible
   
   writeLines(con = file.path(path$SOURCE_ROOT, kmodeltrait, "ExpertN.xni")
              , text = tpl_xni)
   
   # 1b copy .gtp
   file.copy(
      from = list.files(file.path(path$GTP, paste0("N", substring(kxnp, 1,1)), substring(kxnp, 6,6)), recursive = TRUE, full.names = TRUE, pattern = paste0("N", substring(kxnp, 1,3), substring(kxnp, 6, 6), ".gtp"))
      , to = file.path(path$PROJ_ROOT, kmodeltrait, "param", "wheat.gtp")
      , overwrite = TRUE
      )   
   
   readLines(file.path(path$PROJ_ROOT, kmodeltrait, "param", "wheat.gtp")) %>% 
      str_replace(., pattern = data$manag$name_variety[ksite], replacement = strtrim(data$manag$name_variety[ksite],10)) %>% writeLines(., file.path(path$PROJ_ROOT, kmodeltrait, "param", "wheat.gtp"))

   # 1c copy .xnw
   list.files(path$XNW, pattern = data$fnames[ksite]$name_short, full.names = TRUE)  %>% 
      file.copy(., to = file.path(path$PROJ_ROOT, kmodeltrait)) %>% invisible 
                                                                                                    
   # 1d delete old results
   file.path(path$PROJ_ROOT, kmodeltrait) %>% 
      list.files(., pattern = paste(c(str_replace(kxnp,".xnp" ,".rf*"), str_remove(kxnp, ".xnp"), "rfg"), collapse ="|"), full.names = TRUE, recursive = TRUE) %>% 
      grep(., pattern = paste(c(".xnd",".xnp"), collapse="|"),  invert=TRUE, value = TRUE) %>% 
      unlink
   
   # 1e Run Expert-N
   system.time(paste(file.path(path$SOURCE_ROOT, kmodeltrait, "expertn.exe"), "/autostart") %>% system(., timeout = 0))
      
   # 1f remove .xnw (save disk space)
   # list.files(file.path(path$PROJ_ROOT, kmodeltrait), pattern = data$fnames[ksite]$name_short, full.names = TRUE)  %>% 
   #    lapply(., file.remove) %>% invisible

}

# add email sending error messages, here.
source("./01_COMPUTATION_R/plot.R")

