#
# CONFIGURATION GENERALE
#

EXEC = serveur joueur poiss
OBJETS = fonctions_sys.o ncurses.o
NOM_PROJET = projet3

#
# SUFFIXES
#

.SUFFIXES: .c .o

#
# OBJETS
#

EXEC_O = $(EXEC:=.o)
OBJETS_O = $(OBJETS) $(EXEC_O)

#
# ARGUMENTS ET COMPILATEUR
#
CLE = 1056
CC = gcc -g -pthread
CCFLAGS = -Wall -O3 -ansi -pedantic -Werror -D CLE_SEM=$(CLE)
CCLIBS = -lncurses

#
# REGLES
#

all: $(OBJETS) $(EXEC_O)
	@echo "Creation des executables..."
	@for i in $(EXEC); do \
	$(CC) -o $$i $$i.o $(OBJETS) $(CCLIBS); \
	done
	@echo "Termine."

#
# REGLES PAR DEFAUT
#

.c.o: .h
	@cd $(dir $<) && ${CC} ${CCFLAGS} -c $(notdir $<) -o $(notdir $@)

#
# REGLES GENERALES
#

clean:
	@echo "Suppresion des objets, des fichiers temporaires..."
	@rm -f $(OBJETS) $(EXEC_O)
	@rm -f *~ *#
	@rm -f $(EXEC)
	@rm -f dependances
	@echo "Termine."

cleanIPC:
	@echo "Suppression du tableau de sémaphores..."
	@ipcrm -S $(CLE)
	@echo "Sémaphores supprimés."

depend:
	@echo "Creation des dependances..."
	@sed -e "/^# DEPENDANCES/,$$ d" makefile > dependances
	@echo "# DEPENDANCES" >> dependances
	@for i in $(OBJETS_O); do \
	$(CC) -MM -MT $$i $(CCFLAGS) `echo $$i | sed "s/\(.*\)\\.o$$/\1.c/"` >> dependances; \
	done
	@cat dependances > makefile
	@rm dependances
	@echo "Termine."

#
# CREATION ARCHIVE
#

ARCHIVE_FILES = *

archive: clean
	@echo "Creation de l'archive $(NOM_PROJET)$(shell date '+%y%m%d.tar.gz')..."
	@REP=`basename $$PWD`; cd .. && tar zcf $(NOM_PROJET)$(shell date '+%y%m%d.tar.gz') $(addprefix $$REP/,$(ARCHIVE_FILES))
	@echo "Termine."

# DEPENDANCES
ncurses.o:ncurses.c ncurses.h
fonctions_sys.o:fonctions_sys.c fonctions_sys.h struct_message.h
serveur.o: serveur.c struct_message.h fonctions_sys.h
joueur.o: joueur.c struct_message.h fonctions_sys.h ncurses.h
poiss.o: poiss.c struct_message.h fonctions_sys.h ncurses.h
