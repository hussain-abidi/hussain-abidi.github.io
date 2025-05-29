CXX := g++
CXX_FLAGS := -std=c++20 -g -Wall -Werror
LIB_FLAGS := -lusockets -lz
TARGET := server
SRC_DIR := src

SRC_FILES := $(wildcard $(SRC_DIR)/*.cpp)
OBJ_FILES := $(SRC_FILES:$(SRC_DIR)/%.cpp=$(SRC_DIR)/%.o)

all: ${TARGET}

${TARGET}: ${OBJ_FILES}
	${CXX} ${CXX_FLAGS} ${LIB_FLAGS} ${OBJ_FILES} -o ${TARGET}

%.o: %.cpp
	${CXX} ${CXX_FLAGS} -c $< -o $@

clean:
	rm -rf ${TARGET} ${OBJ_FILES}
