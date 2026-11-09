package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

// ===== HashSet из Задания 2 =====
type HashNode struct {
	key  string
	next *HashNode
}

type HashSet struct {
	table []*HashNode
	size  int
	count int
}

func hashFunc(key string, size int) int {
	h := 0
	for _, c := range key { h = (h*31 + int(c)) % size }
	return h
}

func newHashSet(size int) *HashSet {
	return &HashSet{table: make([]*HashNode, size), size: size}
}

func (s *HashSet) add(key string) {
	if key == "" { return }
	idx := hashFunc(key, s.size)
	curr := s.table[idx]
	for curr != nil {
		if curr.key == key { return }
		curr = curr.next
	}
	s.table[idx] = &HashNode{key: key, next: s.table[idx]}
	s.count++
}

func (s *HashSet) contains(key string) bool {
	idx := hashFunc(key, s.size)
	curr := s.table[idx]
	for curr != nil {
		if curr.key == key { return true }
		curr = curr.next
	}
	return false
}

func (s *HashSet) forEach(fn func(string)) {
	for i := 0; i < s.size; i++ {
		for curr := s.table[i]; curr != nil; curr = curr.next { fn(curr.key) }
	}
}

func intersectionSize(a, b *HashSet) int {
	count := 0
	a.forEach(func(k string) { if b.contains(k) { count++ } })
	return count
}

// ===== Парсинг и решение =====
func parseSets(input string) ([]*HashSet, bool) {
	if len(input) < 4 || input[0] != '{' || input[1] != '{' ||
		input[len(input)-1] != '}' || input[len(input)-2] != '}' {
		fmt.Println("Error: invalid format. Expected {{...}, {...}}")
		return nil, false
	}

	n := 0
	for _, c := range input { if c == '{' { n++ } }
	n--
	if n < 2 {
		fmt.Println("Error: need at least 2 sets")
		return nil, false
	}

	sets := make([]*HashSet, n)
	for i := 0; i < n; i++ { sets[i] = newHashSet(1000) }

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
			if currentVal != "" { sets[setIndex].add(currentVal); currentVal = "" }
			insideSet = false
		} else if c == ',' || c == ' ' || c == '\t' {
			if insideSet && currentVal != "" { sets[setIndex].add(currentVal); currentVal = "" }
		} else {
			if !insideSet {
				fmt.Println("Error: invalid format (element outside of set)")
				return nil, false
			}
			if c < '0' || c > '9' {
				fmt.Printf("Error: invalid character '%c'\n", c)
				return nil, false
			}
			currentVal += string(c)
		}
	}
	return sets, true
}

func findMaxIntersection(sets []*HashSet) {
	maxSize := -1
	bestI, bestJ := -1, -1
	for i := 0; i < len(sets); i++ {
		for j := i + 1; j < len(sets); j++ {
			cur := intersectionSize(sets[i], sets[j])
			if cur > maxSize { maxSize = cur; bestI = i; bestJ = j }
		}
	}
	if maxSize <= 0 { fmt.Println("Пересечений не найдено."); return }
	fmt.Printf("Максимальное пересечение: %d\n", maxSize)
	fmt.Printf("Множества: %d и %d\n", bestI+1, bestJ+1)
}

func main() {
	reader := bufio.NewReader(os.Stdin)
	fmt.Print("Введите множества (например, {{1, 2, 3}, {2, 3, 4}}): ")
	input, _ := reader.ReadString('\n')
	input = strings.TrimSpace(input)
	if input == "" { fmt.Println("Error: empty input"); return }

	sets, ok := parseSets(input)
	if !ok { return }
	findMaxIntersection(sets)
}