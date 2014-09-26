
# define MPI_ON
# 1
# endef

all:
	$(MAKE) MPI_ON=$(MPI_ON) -f makefile.at
	$(MAKE) MPI_ON=$(MPI_ON) -f makefile.rt

tidy:
	$(MAKE) -f makefile.rt tidy

sweep:
	$(MAKE) -f makefile.rt sweep

clean:
	$(MAKE) -f makefile.rt clean
