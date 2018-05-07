TEMPLATE = subdirs

SUBDIRS += \
    Core\
    RpiClient \  #Client for PIs : compile this separately on embedded device
#    Server\
  #  Client \

RpiClient.depends = Core
#Server.depends = Core
#Client.depends = Core
