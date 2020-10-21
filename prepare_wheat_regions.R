# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #
#
#
#    prepare the input file wheat_regions.csv
#       Not needed, unless the file 'wheat_regions" is 
#       newly produced from the instructions.pdf
#
#
# # # # # # # # # # # # # # # # # # # # # # # # # # # # # # #


data$manag <- fread("./00_DATA/wheat_regions.csv")
# insert fertilisation dates, here only 2.
{
   data$manag$date_anthesis <- ifelse(data$manag$date_anthesis>data$manag$date_maturity, ymd(data$manag$date_anthesis)-years(1), data$manag$date_anthesis) %>% 
      as.Date(.,origin = as.Date(origin))
   data$manag$date_sowing <- ifelse(data$manag$date_sowing>data$manag$date_anthesis, ymd(data$manag$date_sowing)-years(1), data$manag$date_sowing) %>% 
      as.Date(.,origin = as.Date(origin))
   
   data$manag$date_fert1 <- data$manag$date_sowing + weeks(4)
   data$manag$date_fert2 <- data$manag$date_sowing + days(difftime(data$manag$date_anthesis, data$manag$date_sowing, units ="days")- 7*4)
   
   stopifnot(all(data$manag$date_anthesis>data$manag$date_sowing))
   stopifnot(all(data$manag$date_maturity>data$manag$date_anthesis))
}