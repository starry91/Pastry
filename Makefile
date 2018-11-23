#Name: Praveen Balireddy
#Roll: 2018201052

export PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

#TRACKER_OBJS =  main.o trackerHandler.o seeder.o fileAttr.o client.o 

SUBDIRS := common/ pastryClient/
TOPTARGETS := all clean

$(TOPTARGETS): $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

.PHONY: $(TOPTARGETS) $(SUBDIRS)