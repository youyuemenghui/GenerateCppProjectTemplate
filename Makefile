# 编译器
CC := g++
# 编译选项
CFLAGS := -std=c++20 -Wall -Wextra -pedantic-errors -Wunused-parameter -Wuninitialized  -Wfloat-equal -Wconversion -g

# 项目名称
PROJECT_NAME :=getopt

# 目录
SRC_DIR := src
INC_DIR := include
BUILD_DIR := build
BIN_DIR := bin

# 源文件和目标文件
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# 目标可执行文件
TARGET := $(BIN_DIR)/$(PROJECT_NAME)

# 默认目标
.PHONY: all
all: $(TARGET)

# 编译目标
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

# 依赖关系
-include $(DEPS)

# 编译规则
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(INC_DIR) -MMD -MP -c $< -o $@

# 清理
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# 运行
.PHONY: run
run: $(TARGET)
	$(TARGET)

