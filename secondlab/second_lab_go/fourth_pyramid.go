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

type BlockArray struct {
	data []Block
	size int
	cap  int
}

func newBlockArray(initialCap int) *BlockArray {
	if initialCap <= 0 {
		initialCap = 1
	}
	return &BlockArray{
		data: make([]Block, initialCap),
		size: 0,
		cap:  initialCap,
	}
}

func (a *BlockArray) grow() {
	newCap := a.cap * 2
	newData := make([]Block, newCap)
	for i := 0; i < a.size; i++ {
		newData[i] = a.data[i]
	}
	a.data = newData
	a.cap = newCap
}

func (a *BlockArray) push(b Block) {
	if a.size >= a.cap {
		a.grow()
	}
	a.data[a.size] = b
	a.size++
}

func (a *BlockArray) get(i int) Block      { return a.data[i] }
func (a *BlockArray) set(i int, b Block)   { a.data[i] = b }
func (a *BlockArray) len() int             { return a.size }
func (a *BlockArray) swap(i, j int) {
	a.data[i], a.data[j] = a.data[j], a.data[i]
}

func less(a, b Block) bool {
	if a.w != b.w {
		return a.w > b.w
	}
	return a.h > b.h
}

func quickSort(arr *BlockArray, lo, hi int) {
	if lo >= hi {
		return
	}
	pivot := arr.get((lo + hi) / 2)
	i, j := lo, hi
	for i <= j {
		for less(arr.get(i), pivot) {
			i++
		}
		for less(pivot, arr.get(j)) {
			j--
		}
		if i <= j {
			arr.swap(i, j)
			i++
			j--
		}
	}
	quickSort(arr, lo, j)
	quickSort(arr, i, hi)
}

func readBlocks() (*BlockArray, bool) {
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

	blocks := newBlockArray(n)
	fmt.Println("Введите ширину и высоту каждого блока (через пробел):")
	for i := 0; i < n; i++ {
		line, _ := reader.ReadString('\n')
		line = strings.TrimSpace(line)
		if line == "" {
			fmt.Println("Error: empty line")
			return nil, false
		}
		parts := strings.Fields(line)
		if len(parts) != 2 {
			fmt.Println("Error: invalid format")
			return nil, false
		}
		w, e1 := strconv.Atoi(parts[0])
		h, e2 := strconv.Atoi(parts[1])
		if e1 != nil || e2 != nil || w <= 0 || h <= 0 {
			fmt.Println("Error: dimensions must be positive")
			return nil, false
		}
		blocks.push(Block{w: w, h: h})
	}
	return blocks, true
}

func calculateHeight(blocks *BlockArray) uint64 {
	quickSort(blocks, 0, blocks.len()-1)

	var total uint64 = 0
	lastWidth := int(^uint(0) >> 1) // MaxInt
	for i := 0; i < blocks.len(); i++ {
		b := blocks.get(i)
		if b.w < lastWidth {
			total += uint64(b.h)
			lastWidth = b.w
		}
	}
	return total
}

func main() {
	blocks, ok := readBlocks()
	if !ok {
		return
	}
	fmt.Printf("Максимальная высота пирамиды: %d\n", calculateHeight(blocks))
}