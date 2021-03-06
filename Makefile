#############################################################

# Discard this section from all parent makefiles
# Expected variables (with automatic defaults):
#   CSRCS (all "C" files in the dir)
#   SUBDIRS (all subdirs with a Makefile)
#   GEN_LIBS - list of libs to be generated ()
#   GEN_IMAGES - list of object file images to be generated ()
#   GEN_BINS - list of binaries to be generated ()
#   COMPONENTS_xxx - a list of libs/objs in the form
#     subdir/lib to be extracted and rolled up into
#     a generated lib/image xxx.a ()
#
TARGET = eagle
#FLAVOR = release
FLAVOR = debug

#EXTRA_CCFLAGS += -u

ifndef PDIR # {
GEN_IMAGES= eagle.app.v6.out
GEN_BINS= eagle.app.v6.bin
SPECIAL_MKTARGETS=$(APP_MKTARGETS)
SUBDIRS=    \
	user \
	easyq

endif # } PDIR

APPDIR = .
LDDIR = ../ld

CCFLAGS += -Os

TARGET_LDFLAGS =		\
	-nostdlib		\
	-Wl,-EL \
	--longcalls \
	--text-section-literals

ifeq ($(FLAVOR),debug)
    TARGET_LDFLAGS += -g -O2
endif

ifeq ($(FLAVOR),release)
    TARGET_LDFLAGS += -g -O0
endif

COMPONENTS_eagle.app.v6 = \
	user/libuser.a  \
	easyq/libeasyq.a

LINKFLAGS_eagle.app.v6 = \
	-L../lib        \
	-nostdlib	\
    -T$(LD_FILE)   \
	-Wl,--no-check-sections	\
	-Wl,--gc-sections	\
    -u call_user_start	\
	-Wl,-static						\
	-Wl,--start-group					\
	-lc					\
	-lgcc					\
	-lphy	\
	-lpp	\
	-lnet80211	\
	-llwip	\
	-lwpa	\
	-lcrypto	\
	-lmain	\
	-ljson	\
	-lupgrade\
	-lmbedtls	\
	-lpwm	\
	-ldriver \
	-lsmartconfig \
	-lhal					\
	$(DEP_LIBS_eagle.app.v6)					\
	-Wl,--end-group

	
DEPENDS_eagle.app.v6 = \
                $(LD_FILE) \
                $(LDDIR)/eagle.rom.addr.v6.ld

#############################################################
# Configuration i.e. compile options etc.
# Target specific stuff (defines etc.) goes in here!
# Generally values applying to a tree are captured in the
#   makefile at its root level - these are then overridden
#   for a subtree within the makefile rooted therein
#

#UNIVERSAL_TARGET_DEFINES =		\

# Other potential configuration flags include:
#	-DTXRX_TXBUF_DEBUG
#	-DTXRX_RXBUF_DEBUG
#	-DWLAN_CONFIG_CCX
CONFIGURATION_DEFINES =	-DICACHE_FLASH \
                        -DGLOBAL_DEBUG_ON

DEFINES +=				\
	$(UNIVERSAL_TARGET_DEFINES)	\
	$(CONFIGURATION_DEFINES)

DDEFINES +=				\
	$(UNIVERSAL_TARGET_DEFINES)	\
	$(CONFIGURATION_DEFINES)


#############################################################
# Recursion Magic - Don't touch this!!
#
# Each subtree potentially has an include directory
#   corresponding to the common APIs applicable to modules
#   rooted at that subtree. Accordingly, the INCLUDE PATH
#   of a module can only contain the include directories up
#   its parent path, and not its siblings
#
# Required for each makefile to inherit from the parent
#

INCLUDES := $(INCLUDES) \
	-I $(PDIR)include \
	-I $(PDIR)easyq/include \

PDIR := ../$(PDIR)
sinclude $(PDIR)Makefile

.PHONY: flash flash_user2 flash_user1 flash_erase fota

ESPTOOL = esptool.py --baud 576000 write_flash -u --flash_size 1MB --flash_mode qio --flash_freq 40m
FOTATOOL = ~/.virtualenvs/easyq/bin/python ../fota/fota.py
flash:
	 $(ESPTOOL) \
		0x0 	../bin/boot_v1.7.bin \
		0x1000  ../bin/upgrade/user1.1024.new.2.bin \
		0xfc000 ../bin/esp_init_data_default_v08_vdd33.bin \
		0xfb000 ../bin/blank.bin \
		0xfe000 ../bin/blank.bin

flash_user2:
	$(ESPTOOL) 0x81000 ../bin/upgrade/user2.1024.new.2.bin

flash_user1:
	$(ESPTOOL) 0x01000 ../bin/upgrade/user1.1024.new.2.bin

flash_erase:
	$(ESPTOOL) 0x0 ../bin/blank-1mb.bin

fota: 
	$(FOTATOOL) \
		../bin/upgrade/user2.1024.new.2.bin st ha:1085 \
		-b 192.168.8.214:6666

