#
#  There exist several targets which are by default empty and which can be 
#  used for execution of your targets. These targets are usually executed 
#  before and after some main targets. They are: 
#
#     .build-pre:              called before 'build' target
#     .build-post:             called after 'build' target
#     .clean-pre:              called before 'clean' target
#     .clean-post:             called after 'clean' target
#     .clobber-pre:            called before 'clobber' target
#     .clobber-post:           called after 'clobber' target
#     .all-pre:                called before 'all' target
#     .all-post:               called after 'all' target
#     .help-pre:               called before 'help' target
#     .help-post:              called after 'help' target
#
#  Targets beginning with '.' are not intended to be called on their own.
#
#  Main targets can be executed directly, and they are:
#  
#     build                    build a specific configuration
#     clean                    remove built files from a configuration
#     clobber                  remove all built files
#     all                      build all configurations
#     help                     print help mesage
#  
#  Targets .build-impl, .clean-impl, .clobber-impl, .all-impl, and
#  .help-impl are implemented in nbproject/makefile-impl.mk.
#
#  Available make variables:
#
#     CND_BASEDIR                base directory for relative paths
#     CND_DISTDIR                default top distribution directory (build artifacts)
#     CND_BUILDDIR               default top build directory (object files, ...)
#     CONF                       name of current configuration
#     CND_PLATFORM_${CONF}       platform name (current configuration)
#     CND_ARTIFACT_DIR_${CONF}   directory of build artifact (current configuration)
#     CND_ARTIFACT_NAME_${CONF}  name of build artifact (current configuration)
#     CND_ARTIFACT_PATH_${CONF}  path to build artifact (current configuration)
#     CND_PACKAGE_DIR_${CONF}    directory of package (current configuration)
#     CND_PACKAGE_NAME_${CONF}   name of package (current configuration)
#     CND_PACKAGE_PATH_${CONF}   path to package (current configuration)
#
# NOCDDL


# Environment 
MKDIR=mkdir
CP=cp
CCADMIN=CCadmin

# Extract version string once for all targets
#VERSION_STRING = $(shell grep 'VERSION_STRING' variables.h | sed -E 's/.*"([^"]+)".*/\1/')
VERSION_STRING := $(shell grep -E '^#define[[:space:]]+VERSION_STRING[[:space:]]+"[^"]+"' variables.h | sed -E 's/.*"([^"]+)".*/\1/')

# build
build: .build-post

.build-pre:
# Add your pre 'build' code here...

.build-pre:
	@echo "Generujem build_time.h s aktuálnym dátumom..."
	@echo "#ifndef BUILD_TIME_H" > build_time.h
	@echo "#define BUILD_TIME_H" >> build_time.h
	@echo "#define COMPILE_YEAR $$(date +%y)" >> build_time.h
	@echo "#define COMPILE_MONTH $$(date +%-m)" >> build_time.h
	@echo "#define COMPILE_DAY $$(date +%-d)" >> build_time.h
	@echo "#define COMPILE_HOUR $$(date +%-H)" >> build_time.h
	@echo "#define COMPILE_MIN $$(date +%-M)" >> build_time.h
	@echo "#define COMPILE_SEC $$(date +%-S)" >> build_time.h
	@echo "#endif" >> build_time.h

.build-post: .build-impl
# Add your post 'build' code here...
	arm-none-eabi-objcopy -O binary dist/Debug/GNU_ARM-Linux/fnirsi_1013d_scope.elf dist/Debug/GNU_ARM-Linux/fnirsi_1013d_scope.bin
	
#	chmod a+rwx dist/Debug/GNU_ARM-Linux/fnirsi_1013d_scope.bin
 
 
# Fix the BROM header
	./mksunxi dist/Debug/GNU_ARM-Linux/fnirsi_1013d_scope.bin
#	
#	cp dist/Debug/GNU_ARM-Linux/fnirsi_1013d.bin ../../FNIRSI_1013D_Firmware-main/firmware_Atlan_Peco/scope.bin
# for bootloader files

	@echo ">>> Removing old updater versions..."
	rm -f "../../FNIRSI_1013D_Firmware-main/firmware_Atlan_Peco/scope v"*.bin
	#- rm -f "/media/dk/6741-6DAB/firmware Atlan & PECO/scope v"*.bin
	- rm -f "/media/dk/5229-1580/firmware Atlan & PECO/scope v"*.bin
	- rm -f "/media/dk/524A-7973/firmware Atlan & PECO/scope v"*.bin
	
# Skopírovať výsledný binárny súbor s pridanou verziou v názve
	@echo ">>> VERSION DETECTED: $(VERSION_STRING)"
	cp dist/Debug/GNU_ARM-Linux/fnirsi_1013d_scope.bin \
		"../../FNIRSI_1013D_Firmware-main/firmware_Atlan_Peco/scope $(VERSION_STRING).bin"
		
	#- cp dist/Debug/GNU_ARM-Linux/fnirsi_1013d_scope.bin "/media/dk/6741-6DAB/firmware Atlan & PECO/scope $(VERSION_STRING).bin"
	- cp dist/Debug/GNU_ARM-Linux/fnirsi_1013d_scope.bin "/media/dk/5229-1580/firmware Atlan & PECO/scope $(VERSION_STRING).bin"
	- cp dist/Debug/GNU_ARM-Linux/fnirsi_1013d_scope.bin "/media/dk/524A-7973/firmware Atlan & PECO/scope $(VERSION_STRING).bin"
	
	@echo ">>> Created: scope $(VERSION_STRING).bin"

	#cp dist/Debug/GNU_ARM-Linux/fnirsi_1013d_scope.bin ../../FNIRSI_1013D_Firmware-main/firmware_Atlan_Peco/scope.bin
	#cp dist/Debug/GNU_ARM-Linux/fnirsi_1013d_scope.bin "/media/dk/6741-6DAB/firmware Atlan & PECO/scope.bin"

#	chmod a+rwx dist/Debug/GNU_ARM-Linux/fnirsi_1013d_scope.bin
#	chmod a+rwx dist/Debug/GNU_ARM-Linux/fnirsi_1013d.bin


# SD card package
	./flashfilepacker -o dist/Debug/GNU_ARM-Linux/fnirsi_1013d.bin -i dist/Debug/GNU_ARM-Linux/fnirsi_1013d_scope.bin -l 0x5BC00
# 	./flashfilepacker -o dist/Debug/GNU_ARM-Linux/fnirsi_1013d.bin -i dist/Debug/GNU_ARM-Linux/fnirsi_1013d_scope.bin -l 0x8000 0x5BC00

# Auto load firmware with FEL
	sudo ./dist/Debug/GNU_ARM-Linux/sunxi-fel -p write 0x7FFFFFE0 dist/Debug/GNU_ARM-Linux/fnirsi_1013d_scope.bin exe 0x80000000



# clean
clean: .clean-post

.clean-pre:
# Add your pre 'clean' code here...

.clean-post: .clean-impl
# Add your post 'clean' code here...


# clobber
clobber: .clobber-post

.clobber-pre:
# Add your pre 'clobber' code here...

.clobber-post: .clobber-impl
# Add your post 'clobber' code here...


# all
all: .all-post

.all-pre:
# Add your pre 'all' code here...

.all-post: .all-impl
# Add your post 'all' code here...


# build tests
build-tests: .build-tests-post

.build-tests-pre:
# Add your pre 'build-tests' code here...

.build-tests-post: .build-tests-impl
# Add your post 'build-tests' code here...


# run tests
test: .test-post

.test-pre: build-tests
# Add your pre 'test' code here...

.test-post: .test-impl
# Add your post 'test' code here...


# help
help: .help-post

.help-pre:
# Add your pre 'help' code here...

.help-post: .help-impl
# Add your post 'help' code here...


# include project implementation makefile
include nbproject/Makefile-impl.mk

# include project make variables
include nbproject/Makefile-variables.mk
