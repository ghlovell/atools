
# define MPI_ON
# 1
# endef

all: | checkrootsys
	$(MAKE) MPI_ON=$(MPI_ON) -f makefile.at
	$(MAKE) MPI_ON=$(MPI_ON) -f makefile.rt

.PHONY: install tidy sweep clean

install:
	$(MAKE) -f makefile.at install
	$(MAKE) -f makefile.rt install

tidy:
	$(MAKE) -f makefile.rt tidy

sweep:
	$(MAKE) -f makefile.rt sweep

clean:
	$(MAKE) -f makefile.rt clean

checkrootsys:
ifndef ROOTSYS
	$(error ROOTSYS variable is not set. Configure a valid root environment)
endif
