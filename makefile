KEXT=DerivedData/Black80211/Build/Products/Debug/Black80211.kext

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
