#
# CONFIGURATION GENERALE
#

EXEC = voiture controleur 
OBJETS = 
NOM_PROJET = projet2

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
#CLE_MSG = 1056
CC = gcc
CCFLAGS = -Wall -O3 -Werror -ansi -pedantic -g -D_POSIX_C_SOURCE#-D CLE=$(CLE_MSG)
CCLIBS = 

#
# REGLES
#

all: $(OBJETS) $(EXEC_O)
	@echo "Création des exécutables..."
	@for i in $(EXEC); do \
	$(CC) -o $$i $$i.o $(OBJETS) $(CCLIBS); \
	done
	@echo "Terminé."

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
	@echo "Terminé."

cleanIPC:
	@echo "Suppression de la file de messages..."
	@ipcrm -Q $(CLE_MSG)
	@echo "File supprimée."

depend:
	@echo "Création des dépendances..."
	@sed -e "/^# DEPENDANCES/,$$ d" makefile > dependances
	@echo "# DEPENDANCES" >> dependances
	@for i in $(OBJETS_O); do \
	$(CC) -MM -MT $$i $(CCFLAGS) `echo $$i | sed "s/\(.*\)\\.o$$/\1.c/"` >> dependances; \
	done
	@cat dependances > makefile
	@rm dependances
	@echo "Terminé."

#
# CREATION ARCHIVE
#

ARCHIVE_FILES = *

archive: clean
	@echo "Création de l'archive $(NOM_PROJET)$(shell date '+%y%m%d.tar.gz')..."
	@REP=`basename $$PWD`; cd .. && tar zcf $(NOM_PROJET)$(shell date '+%y%m%d.tar.gz') $(addprefix $$REP/,$(ARCHIVE_FILES))
	@echo "Termine."

# DEPENDANCES
ncurses.o:ncurses.c ncurses.h
fonctions.o : struct.h ncurses.h fonctions.h fonctions.c
voiture.o : struct.h ncurses.h fonctions.h voiture.c
controleur.o : struct.h ncurses.h fonctions.h controleur.c

