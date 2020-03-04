library(tidyverse)
library(sbmR)


edges <- clements_pollinators

nodes <- bind_rows(tibble(id = edges$pollinator,
                          type = "pollinator"),
                   tibble(id = edges$flower,
                          type = "flower")) %>%
  arrange(type)

types <- count(nodes, type, name = "count")


