.SUFFIXES: .cpp .o .hpp .h .tpp
.PHONY: all clean cleanobj cleanbin re deps depsclean depsre bot

MAKEFLAGS =

ifeq ($(shell uname),Linux)
	MAKEFLAGS := --jobs $(shell nproc)
else
	MAKEFLAGS := --jobs $(shell sysctl -n hw.ncpu)
endif


CXX = c++
RM = rm -f

CXXFLAGS += -MMD -MF $(@:.o=.d) -MT $@ -MP
CXXFLAGS += --std=c++98 -Wall -Wextra -Werror -pedantic
CXXFLAGS += -Iincludes

LIBSERV := includes/libserv/
CXXFLAGS += -I$(LIBSERV)includes
LDFLAGS += -L$(LIBSERV) -lserv -pthread

TARGET = ircserv.out
BOT_TARGET = bot.out
OBJECTS_DIR = objs/

SOURCES += \
	channel.cpp \
	main.cpp \
	message.cpp \
	processor.cpp \
	reply.cpp \
	server.cpp \
	user.cpp \

OBJECTS = $(addprefix $(OBJECTS_DIR), $(SOURCES:.cpp=.o))

BOT_SOURCES += \
	bot.cpp \
	bot_main.cpp \
	message.cpp \
	reply.cpp \

BOT_OBJECTS = $(addprefix $(OBJECTS_DIR), $(BOT_SOURCES:.cpp=.o))

ifdef ASAN
	CXXFLAGS += -fsanitize=address
	LDFLAGS += -fsanitize=address
	DEBUG = 1
endif

ifdef MSAN
	CXXFLAGS += -fsanitize=memory
	LDFLAGS += -fsanitize=memory
	DEBUG = 1
endif

ifdef TSAN
	CXXFLAGS += -fsanitize=thread
	LDFLAGS += -fsanitize=thread
	DEBUG = 1
endif

ifdef UBSAN
	CXXFLAGS += -fsanitize=undefined
	LDFLAGS += -fsanitize=undefined
	DEBUG = 1
endif

ifdef DEBUG
	CXXFLAGS += -g3 -O0 -DFT_DEBUG
endif

ifdef TRACE
	CXXFLAGS += -DFT_TRACE
endif

ifdef MJ
	CXXFLAGS += -MJ $@.part.json
endif

all: $(TARGET) $(BOT_TARGET)
clean: cleanobj cleanbin
cleanobj:	;	$(RM) -r $(OBJECTS_DIR)
cleanbin:	;	$(RM) $(TARGET) $(BOT_TARGET)
re: clean	;	$(MAKE)

$(OBJECTS_DIR):
	mkdir -p $(OBJECTS_DIR)

$(OBJECTS_DIR)%.o: %.cpp | $(OBJECTS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BOT_TARGET): $(BOT_OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

-include $(OBJECTS:.o=.d)

deps:
	$(MAKE) -C $(LIBSERV)

depsclean:
	$(MAKE) -C $(LIBSERV) clean

depsre:
	$(MAKE) -C $(LIBSERV) re

allre:
	$(MAKE) depsre
	$(MAKE) re
