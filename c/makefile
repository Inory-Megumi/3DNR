CXX = g++
CXX_FLAGS = -Wall -g
CXX_SRC = $(wildcard src/*.cpp)
HEADER_SRC = $(wildcard src/*.h)
BIN = NR
YUV_SRC = ../data/akiyo_qcif.yuv
YUV_DST = ../data/akiyo_qcif_out.yuv
$(BIN): $(CXX_SRC) $(HEADER_SRC)
	@$(CXX) $(CXX_FLAGS) $(CXX_SRC) -o $(BIN)
run : $(BIN)
	@./$(BIN) $(YUV_SRC) $(YUV_DST)
clean :
	rm -rf $(BIN)
.PHONY:
	clean