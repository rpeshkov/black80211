TARGETOS := $(shell uname -s)

ifeq ($(TARGETOS), Darwin)
	OSXRELEASE := $(shell uname -r | sed 's/\..*//')
	ifeq ($(OSXRELEASE), 17)
		OSXVER = HighSierra
	endif
	ifeq ($(OSXRELEASE), 16)
		OSXVER = Sierra
	endif
endif
KEXT=DerivedData/Black80211/Build/Products/$(OSXVER)/Debug/Black80211.kext

ifeq ($(findstring 32,$(BITS)),32)
    OPTIONS:=$(OPTIONS) -arch i386
endif

ifeq ($(findstring 64,$(BITS)),64)
    OPTIONS:=$(OPTIONS) -arch x86_64
endif

.PHONY: all
all:
	xcodebuild build $(OPTIONS) -scheme Black80211_$(OSXVER) -configuration Debug

.PHONY: deps
deps:
	sudo kextlibs -xml $(KEXT)

.PHONY: load
load:
	sudo chown -R root:wheel $(KEXT)
	sudo kextutil $(KEXT)

.PHONY: unload
unload:
	sudo kextunload $(KEXT)

.PHONY: clean
clean:
	sudo rm -rf $(KEXT)
