TEMPLATE = subdirs

SUBDIRS += \
    Core\
<<<<<<< HEAD
#    RpiClient \  #Client for PIs : compile this separately on embedded device
    Server\
    Client \

#RpiClient.depends = Core
Server.depends = Core
Client.depends = Core
=======
    RpiClient \  #Client for PIs : compile this separately on embedded device
#    Server\
  #  Client \

RpiClient.depends = Core
#Server.depends = Core
#Client.depends = Core
>>>>>>> 262c05b460ecbc18544ed67e53566e195f106572
