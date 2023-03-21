.SUFFIXES: .cpp .o .hpp .h .tpp
.PHONY: all clean cleanobj cleanbin re

CXX = c++
RM = rm -f
AR = ar
ARFLAGS = crs
MKDIR = mkdir -p

CXXFLAGS += -MMD -MF $(@:.o=.d) -MT $@ -MP
CXXFLAGS += --std=c++98 -Wall -Wextra -Werror
CXXFLAGS += -Iincludes

TARGET = libserv.a
OBJECTS_DIR = objs/

SOURCES += \
	bootstrap.cpp \
	bytes_decoder.cpp \
	bytes_encoder.cpp \
	event_channel_base.cpp \
	event_handler_base.cpp \
	event_layer.cpp \
	event_worker.cpp \
	event_worker_group.cpp \
	file_channel.cpp \
	logic_adapter.cpp \
	object_decoder.cpp \
	object_encoder.cpp \
	server_channel.cpp \
	stream_channel.cpp \
	task_base.cpp \

ifeq ($(shell uname), Linux)
	SOURCES += \
		event_worker.epoll.cpp \

else
	SOURCES += \
		event_worker.kqueue.cpp \

endif

OBJECTS = $(addprefix $(OBJECTS_DIR), $(SOURCES:.cpp=.o))

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

all: $(TARGET)
clean: cleanobj cleanbin
cleanobj:	;	$(RM) -r $(OBJECTS_DIR)
cleanbin:	;	$(RM) $(TARGET)

$(OBJECTS_DIR):
	$(MKDIR) $(OBJECTS_DIR)

$(OBJECTS_DIR)%.o: %.cpp | $(OBJECTS_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(AR) $(ARFLAGS) $@ $?

-include $(OBJECTS:.o=.d)
