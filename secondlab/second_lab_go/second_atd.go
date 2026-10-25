package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

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
	for _, c := range key {
		h = (h*31 + int(c)) % size
	}
	return h
}

func newHashSet(size int) *HashSet {
	return &HashSet{
		table: make([]*HashNode, size),
		size:  size,
		count: 0,
	}
}

func (s *HashSet) add(key string) {
	if key == "" {
		return
	}
	idx := hashFunc(key, s.size)
	curr := s.table[idx]
	for curr != nil {
		if curr.key == key {
			return
		}
		curr = curr.next
	}
	newNode := &HashNode{key: key, next: s.table[idx]}
	s.table[idx] = newNode
	s.count++
}

func (s *HashSet) del(key string) bool {
	idx := hashFunc(key, s.size)
	curr := s.table[idx]
	var prev *HashNode
	for curr != nil {
		if curr.key == key {
			if prev != nil {
				prev.next = curr.next
			} else {
				s.table[idx] = curr.next
			}
			s.count--
			return true
		}
		prev = curr
		curr = curr.next
	}
	return false
}

func (s *HashSet) contains(key string) bool {
	idx := hashFunc(key, s.size)
	curr := s.table[idx]
	for curr != nil {
		if curr.key == key {
			return true
		}
		curr = curr.next
	}
	return false
}

func (s *HashSet) loadFromFile(filename string) {
	file, err := os.Open(filename)
	if err != nil {
		return
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	for scanner.Scan() {
		s.add(scanner.Text())
	}
}

func (s *HashSet) saveToFile(filename string) {
	file, err := os.Create(filename)
	if err != nil {
		return
	}
	defer file.Close()

	for i := 0; i < s.size; i++ {
		curr := s.table[i]
		for curr != nil {
			fmt.Fprintln(file, curr.key)
			curr = curr.next
		}
	}
}

func main() {
	var filename, query string
	args := os.Args[1:]
	for i := 0; i < len(args); i++ {
		if args[i] == "--file" && i+1 < len(args) {
			filename = args[i+1]
			i++
		} else if args[i] == "--query" && i+1 < len(args) {
			query = args[i+1]
			i++
		}
	}

	if filename == "" || query == "" {
		fmt.Println("Usage: ./program --file <file> --query \"<command> <value>\"")
		return
	}

	mySet := newHashSet(1000)
	mySet.loadFromFile(filename)

	parts := strings.Fields(query)
	if len(parts) < 1 {
		fmt.Println("Error: empty query")
		return
	}
	cmd := parts[0]
	val := ""
	if len(parts) > 1 {
		val = parts[1]
	}

	switch cmd {
	case "SETADD":
		if val == "" {
			fmt.Println("Error: empty value")
			return
		}
		mySet.add(val)
		mySet.saveToFile(filename)
		fmt.Printf("Added: %s\n", val)
	case "SETDEL":
		if val == "" {
			fmt.Println("Error: empty value")
			return
		}
		if mySet.del(val) {
			mySet.saveToFile(filename)
			fmt.Printf("Deleted: %s\n", val)
		} else {
			fmt.Printf("Not found: %s\n", val)
		}
	case "SET_AT":
		if val == "" {
			fmt.Println("Error: empty value")
			return
		}
		if mySet.contains(val) {
			fmt.Println("True")
		} else {
			fmt.Println("False")
		}
	default:
		fmt.Println("Error: unknown command")
	}
}
