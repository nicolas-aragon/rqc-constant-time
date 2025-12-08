CC = gcc

CFLAGS = -O3 -march=native -mpclmul -maes -Iinclude -Wno-deprecated-declarations
CFLAGS_POISON = -g -I /usr/include/valgrind #Valgrind path

LDFLAGS = -lssl -lcrypto -lm

BUILDDIR = bin
SOURCEDIR = src

C_SRCS = $(wildcard $(SOURCEDIR)/rqc-128/*.c)
ASM_SRCS = $(wildcard $(SOURCEDIR)/rqc-128/*.s)

C_OBJS = $(C_SRCS:%.c=%.o)
ASM_OBJS = $(ASM_SRCS:%.s=%.o)

SRCS = $(C_SRCS) $(ASM_SRCS)
OBJS = $(C_OBJS) $(ASM_OBJS)

TARGET = main
TARGET_SPEED = main_speed
TARGET_CONSTANT_TIME = main_const_time
TARGET_DISTRIBUTION = main_distribution
TARGET_AG_EG= main_ag_vs_eg
TARGET_POISON = main_poison
TARGET_LDIV= main_ldiv


#.PHONY: all
all: $(TARGET) $(TARGET_SPEED) $(TARGET_CONSTANT_TIME) $(TARGET_DISTRIBUTION) $(TARGET_AG_EG) $(TARGET_LDIV) $(TARGET_POISON)

$(TARGET): $(OBJS) src/$(TARGET).c
	@mkdir -p $(BUILDDIR)
	$(CC) src/$(TARGET).c $(OBJS) $(LDFLAGS) -o $(BUILDDIR)/$(TARGET)

$(TARGET_SPEED) : $(OBJS) src/$(TARGET_SPEED).c
	@mkdir -p $(BUILDDIR)
	$(CC) src/$(TARGET_SPEED).c $(OBJS) $(LDFLAGS) -o $(BUILDDIR)/$(TARGET_SPEED)

$(TARGET_CONSTANT_TIME): $(OBJS) src/$(TARGET_CONSTANT_TIME).c
	@mkdir -p $(BUILDDIR)
	$(CC) src/$(TARGET_CONSTANT_TIME).c $(OBJS) $(LDFLAGS) -o $(BUILDDIR)/$(TARGET_CONSTANT_TIME)

$(TARGET_DISTRIBUTION): $(OBJS) src/$(TARGET_DISTRIBUTION).c
	@mkdir -p $(BUILDDIR)
	$(CC) src/$(TARGET_DISTRIBUTION).c $(OBJS) $(LDFLAGS) -o $(BUILDDIR)/$(TARGET_DISTRIBUTION)

$(TARGET_AG_EG): $(OBJS) src/$(TARGET_AG_EG).c
	@mkdir -p $(BUILDDIR)
	$(CC) src/$(TARGET_AG_EG).c $(OBJS) $(LDFLAGS) -o $(BUILDDIR)/$(TARGET_AG_EG)

$(TARGET_POISON) : $(OBJS) src/$(TARGET_POISON).c
	@mkdir -p $(BUILDDIR)
	$(CC) src/$(TARGET_POISON).c $(OBJS) $(CFLAGS_POISON) $(LDFLAGS) -o $(BUILDDIR)/$(TARGET_POISON)

$(TARGET_LDIV): $(OBJS) src/$(TARGET_LDIV).c
	@mkdir -p $(BUILDDIR)
	$(CC) src/$(TARGET_LDIV).c $(OBJS) $(LDFLAGS) -o $(BUILDDIR)/$(TARGET_LDIV)

clean:
	rm -rf $(BUILDDIR)
	rm $(SOURCEDIR)/rqc-128/*.o
