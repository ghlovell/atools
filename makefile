
# define MPI_ON
# 1
# endef

all: | checkrootsys
	$(MAKE) MPI_ON=$(MPI_ON) -f makefile.at
	$(MAKE) MPI_ON=$(MPI_ON) -f makefile.rt

tidy:
	$(MAKE) -f makefile.at tidy

sweep:
	$(MAKE) -f makefile.at sweep

clean:
	$(MAKE) -f makefile.at clean

checkrootsys:
ifndef ROOTSYS
	$(error ROOTSYS variable is not set. Configure a valid root environment)
endif
