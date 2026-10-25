package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

type SetNode struct {
	data string
	next *SetNode
}

type MySet struct {
	head *SetNode
}

func (s *MySet) init() {
	s.head = nil
}

func (s *MySet) contains(val string) bool {
	curr := s.head
	for curr != nil {
		if curr.data == val {
			return true
		}
		curr = curr.next
	}
	return false
}

func (s *MySet) add(val string) {
	if val == "" || s.contains(val) {
		return
	}
	newNode := &SetNode{data: val, next: s.head}
	s.head = newNode
}

func (s *MySet) clear() {
	s.head = nil
}

func intersectionSize(a, b *MySet) int {
	count := 0
	curr := a.head
	for curr != nil {
		if b.contains(curr.data) {
			count++
		}
		curr = curr.next
	}
	return count
}

func parseSets(input string) ([]*MySet, bool) {
	if len(input) < 4 || input[0] != '{' || input[1] != '{' ||
		input[len(input)-1] != '}' || input[len(input)-2] != '}' {
		fmt.Println("Error: invalid format. Expected {{...}, {...}}")
		return nil, false
	}

	n := 0
	for _, c := range input {
		if c == '{' {
			n++
		}
	}
	n--

	if n < 2 {
		fmt.Println("Error: need at least 2 sets")
		return nil, false
	}

	sets := make([]*MySet, n)
	for i := 0; i < n; i++ {
		sets[i] = &MySet{}
		sets[i].init()
	}

	setIndex := -1
	currentVal := ""
	insideSet := false

	for i := 1; i < len(input)-1; i++ {
		c := input[i]
		if c == '{' {
			setIndex++
			insideSet = true
			currentVal = ""
		} else if c == '}' {
			if currentVal != "" {
				sets[setIndex].add(currentVal)
				currentVal = ""
			}
			insideSet = false
		} else if c == ',' || c == ' ' {
			if insideSet && currentVal != "" {
				sets[setIndex].add(currentVal)
				currentVal = ""
			}
		} else {
			if !insideSet {
				fmt.Println("Error: invalid format (element outside of set)")
				return nil, false
			}
			if c < '0' || c > '9' {
				fmt.Printf("Error: invalid character '%c' (only digits allowed)\n", c)
				return nil, false
			}
			currentVal += string(c)
		}
	}
	return sets, true
}

func findMaxIntersection(sets []*MySet, n int) {
	maxSize := -1
	bestI, bestJ := -1, -1

	for i := 0; i < n; i++ {
		for j := i + 1; j < n; j++ {
			currentSize := intersectionSize(sets[i], sets[j])
			if currentSize > maxSize {
				maxSize = currentSize
				bestI = i
				bestJ = j
			}
		}
	}

	if maxSize <= 0 {
		fmt.Println("Пересечений не найдено.")
	} else {
		fmt.Printf("Максимальное пересечение: %d\n", maxSize)
		fmt.Printf("Множества: %d и %d\n", bestI+1, bestJ+1)
	}
}

func main() {
	reader := bufio.NewReader(os.Stdin)
	fmt.Print("Введите множества (например, {{1, 2, 3}, {2, 3, 4}}): ")
	input, _ := reader.ReadString('\n')
	input = strings.TrimSpace(input)

	cleanInput := strings.ReplaceAll(input, " ", "")
	cleanInput = strings.ReplaceAll(cleanInput, "\t", "")

	if cleanInput == "" {
		fmt.Println("Error: empty input")
		return
	}

	sets, ok := parseSets(input)
	if !ok {
		return
	}

	findMaxIntersection(sets, len(sets))
}
