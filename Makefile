########################################################################
####################### Makefile Template ##############################
########################################################################

# Compiler settings - Can be customized.
CC = g++
WALLFLAGS = -Wall -Wextra -Wpedantic -Wno-unused-parameter
CXXFLAGS = -std=c++14 -O3 $(WALLFLAGS) -fopenmp #-pthread
LDFLAGS = -lglut -lGL #-lGLU

# Makefile settings - Can be customized.
APPNAME = main
EXT = .cpp
SRCDIR = src
OBJDIR = obj

############## Do not change anything from here downwards! #############
SRC = $(wildcard $(SRCDIR)/*$(EXT))
OBJ = $(SRC:$(SRCDIR)/%$(EXT)=$(OBJDIR)/%.o)
DEP = $(OBJ:$(OBJDIR)/%.o=%.d)
# UNIX-based OS variables & settings
RM = rm
RMDIR = rm -r
TEST = test -s
DELOBJ = $(OBJ)

########################################################################
####################### Targets beginning here #########################
########################################################################

all: mkdirs $(APPNAME)
	! ($(TEST) $(DEP)) || $(RM) $(DEP)

# Builds the app
$(APPNAME): $(OBJ)
	$(CC) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Creates the dependecy rules
%.d: $(SRCDIR)/%$(EXT)
	@$(CPP) $(CFLAGS) $< -MM -MT $(@:%.d=$(OBJDIR)/%.o) >$@

# Includes all .h files
-include $(DEP)

# Building rule for .o files and its .c/.cpp in combination with all .h
$(OBJDIR)/%.o: $(SRCDIR)/%$(EXT)
	$(CC) $(CXXFLAGS) -o $@ -c $<

# Building OBJ and SRC dir if they don't exist yet
.PHONY: mkdirs
mkdirs: $(OBJDIR)/

$(OBJDIR)/:
	mkdir -p $@

################### Cleaning rules for Unix-based OS ###################
# Cleans complete project
.PHONY: clean
clean:
	-$(RM) $(DELOBJ) $(DEP) $(APPNAME)
	-$(RMDIR) $(OBJDIR)

# Cleans only all files with the extension .d
.PHONY: cleandep
cleandep:
	$(RM) $(DEP)