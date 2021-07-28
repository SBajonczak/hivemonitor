$(eval venvpath     := .venv)
$(eval python       := $(venvpath)/bin/python)
$(eval pip          := $(venvpath)/bin/pip)
$(eval platformio   := $(venvpath)/bin/platformio)


help:
	@echo "Please use \`make <target>' where <target> is one of"
	@echo "  build              to compile the firmare"
	@echo "  upload-firmware    to upload the firmware to the device"
	@echo "  upload-config      to upload the configuration to the device"


build: setup-virtualenv
	$(platformio) run

upload-firmware:
	$(platformio) run --target upload

upload-config:
	$(platformio) run --target uploadfs

setup-virtualenv:
	@test -e $(python) || `command -v virtualenv` --python=python3 $(venvpath)
	$(pip) install platformio
