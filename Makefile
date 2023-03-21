.SUFFIXES: .cpp .o .hpp .h .tpp
.PHONY: all clean fclean cleanobj cleanbin re deps depsclean depsre allre bonus

JOBS =

ifeq ($(shell uname), Linux)
	JOBS += --jobs $(shell nproc)
else
	JOBS += --jobs $(shell sysctl -n hw.ncpu)
endif

# Configuration
CXX = c++
RM = rm -f
MKDIR = mkdir -p

CXXFLAGS += -MMD -MF $(@:.o=.d) -MT $@ -MP
CXXFLAGS += --std=c++98 -Wall -Wextra -Werror -pedantic
CXXFLAGS += -Iincludes

LIBSERV := includes/libserv/libserv.a
CXXFLAGS += -I$(dir $(LIBSERV))includes
LDFLAGS += -L$(dir $(LIBSERV)) -lserv -pthread

# Target
TARGET = ircserv.out
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

# Bonus target
BONUS_TARGET = bot.out

BONUS_SOURCES += \
	bot.cpp \
	message.cpp \
	reply.cpp \

BONUS_OBJECTS = $(addprefix $(OBJECTS_DIR), $(BONUS_SOURCES:.cpp=.o))

# Options
ifdef ASAN
	CXXFLAGS += -fsanitize=address
	LDFLAGS += -fsanitize=address
endif

ifdef MSAN
	CXXFLAGS += -fsanitize=memory
	LDFLAGS += -fsanitize=memory
endif

ifdef TSAN
	CXXFLAGS += -fsanitize=thread
	LDFLAGS += -fsanitize=thread
endif

ifdef UBSAN
	CXXFLAGS += -fsanitize=undefined
	LDFLAGS += -fsanitize=undefined
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

# Phonies
all: | $(LIBSERV)
	@$(MAKE) $(JOBS) $(TARGET)

bonus: $(BONUS_TARGET)

clean: cleanobj
	$(MAKE) -C $(dir $(LIBSERV)) $@

fclean: cleanobj cleanbin
	$(MAKE) -C $(dir $(LIBSERV)) $@

cleanobj:
	$(RM) -r $(OBJECTS_DIR)

cleanbin:
	$(RM) $(TARGET) $(BONUS_TARGET)

re: 
	$(MAKE) fclean
	$(MAKE)

# Recipes
$(OBJECTS_DIR):
	$(MKDIR) $(OBJECTS_DIR)

$(OBJECTS_DIR)%.o: %.cpp | $(OBJECTS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(BONUS_TARGET): $(BONUS_OBJECTS) | $(LIBSERV)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(LIBSERV):
	$(MAKE) -C $(dir $(LIBSERV))


# Headers
-include $(OBJECTS:.o=.d)
-include $(BONUS_OBJECTS:.o=.d)
