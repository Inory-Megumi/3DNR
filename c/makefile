CXX = g++
CXX_FLAGS = -Wall -g
CXX_SRC = $(wildcard src/*.cpp src/*.c)
HEADER_SRC = $(wildcard src/*.h)
BIN = NR
YUV_SRC = ../data/akiyo_qcif_noise_out.yuv
YUV_DST = ../data/akiyo_qcif_out.yuv
LOG_FILE = log.txt
$(BIN): $(CXX_SRC) $(HEADER_SRC)
	@$(CXX) $(CXX_FLAGS) $(CXX_SRC) -o $(BIN)
run : $(BIN)
	@./$(BIN) $(YUV_SRC) $(YUV_DST) $(LOG_FILE)
clean :
	rm -rf $(BIN)
.PHONY:
	clean