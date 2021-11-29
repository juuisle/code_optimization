# For Java
JFLAGS = -g
JC = javac

# For C
CC          = gcc
CLINKER     = $(CC)
OPTFLAGS    = -O3 ###-std=gnu11    ####-std=c99
LFLAGS	    = -lm -lopenblas
CFLAGS  =   $(OPTFLAGS) $(LFLAGS) 

.SUFFIXES: .java .class
.java.class:
	$(JC) $(JFLAGS) $*.java

CLASSES = mm.java

default: classes mm mm_blas

classes: $(CLASSES:.java=.class)

.c.o:
	$(CC) $(CFLAGS) -c $*.c
.f.o:
	$(F77) $(FFLAGS) -c $*.f

### run: 
###	srun -n 1 java mm 2000 &
###	srun -n 1 mm 2000 &

clean:
	$(RM) *.class *.o mm
