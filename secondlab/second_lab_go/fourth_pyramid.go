package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

type Block struct {
	w, h int
}

func sortBlocks(arr []Block) {
	n := len(arr)
	for i := 0; i < n-1; i++ {
		for j := 0; j < n-i-1; j++ {
			if arr[j].w < arr[j+1].w || (arr[j].w == arr[j+1].w && arr[j].h < arr[j+1].h) {
				arr[j], arr[j+1] = arr[j+1], arr[j]
			}
		}
	}
}

func readBlocks() ([]Block, bool) {
	reader := bufio.NewReader(os.Stdin)
	fmt.Print("Введите количество блоков: ")
	line, _ := reader.ReadString('\n')
	line = strings.TrimSpace(line)

	if line == "" {
		fmt.Println("Error: empty input")
		return nil, false
	}

	n, err := strconv.Atoi(line)
	if err != nil || n <= 0 {
		fmt.Println("Error: invalid input")
		return nil, false
	}

	blocks := make([]Block, n)
	fmt.Println("Введите ширину и высоту каждого блока (через пробел):")

	for i := 0; i < n; i++ {
		line, _ := reader.ReadString('\n')
		line = strings.TrimSpace(line)
		if line == "" {
			fmt.Println("Error: empty line for block data")
			return nil, false
		}

		parts := strings.Fields(line)
		if len(parts) != 2 {
			fmt.Println("Error: invalid block data format")
			return nil, false
		}

		w, err1 := strconv.Atoi(parts[0])
		h, err2 := strconv.Atoi(parts[1])

		if err1 != nil || err2 != nil || w <= 0 || h <= 0 {
			fmt.Println("Error: block dimensions must be positive")
			return nil, false
		}
		blocks[i] = Block{w: w, h: h}
	}
	return blocks, true
}

func calculateHeight(blocks []Block) uint64 {
	sortBlocks(blocks)
	var totalHeight uint64 = 0
	lastWidth := 2147483647

	for _, b := range blocks {
		if b.w < lastWidth {
			totalHeight += uint64(b.h)
			lastWidth = b.w
		}
	}
	return totalHeight
}

func main() {
	blocks, ok := readBlocks()
	if !ok {
		return
	}

	height := calculateHeight(blocks)
	fmt.Printf("Максимальная высота пирамиды: %d\n", height)
}
