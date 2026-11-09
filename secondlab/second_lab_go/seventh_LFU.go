package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

type CacheNode struct {
	key   string
	value int
	freq  int
	order int
	next  *CacheNode
}

type LFUCache struct {
	capacity    int
	size        int
	head        *CacheNode
	globalOrder int
}

func newLFUCache(cap int) *LFUCache {
	if cap <= 0 {
		fmt.Println("Error: Capacity must be > 0. Setting to 1.")
		cap = 1
	}
	return &LFUCache{capacity: cap, size: 0, head: nil, globalOrder: 0}
}

func (c *LFUCache) removeNode(prev, curr *CacheNode) {
	if prev != nil {
		prev.next = curr.next
	} else {
		c.head = curr.next
	}
	c.size--
}

func (c *LFUCache) set(key string, value int) {
	
	for curr := c.head; curr != nil; curr = curr.next {
		if curr.key == key {
			curr.value = value
			curr.freq++
			c.globalOrder++
			curr.order = c.globalOrder
			return
		}
	}

	if c.size >= c.capacity {
		minNode := c.head
		var minPrev *CacheNode
		var prev *CacheNode
		for cur := c.head; cur != nil; cur = cur.next {
			if cur.freq < minNode.freq ||
				(cur.freq == minNode.freq && cur.order < minNode.order) {
				minNode = cur
				minPrev = prev
			}
			prev = cur
		}
		if minNode != nil {
			c.removeNode(minPrev, minNode)
		}
	}

	c.globalOrder++
	newNode := &CacheNode{
		key:   key,
		value: value,
		freq:  1,
		order: c.globalOrder,
		next:  c.head,
	}
	c.head = newNode
	c.size++
}

func (c *LFUCache) get(key string) int {
	for curr := c.head; curr != nil; curr = curr.next {
		if curr.key == key {
			curr.freq++
			c.globalOrder++
			curr.order = c.globalOrder
			return curr.value
		}
	}
	return -1
}

func readInt(reader *bufio.Reader) (int, bool) {
	line, _ := reader.ReadString('\n')
	line = strings.TrimSpace(line)
	if line == "" {
		return 0, false
	}
	v, err := strconv.Atoi(line)
	if err != nil {
		return 0, false
	}
	return v, true
}

func main() {
	reader := bufio.NewReader(os.Stdin)

	fmt.Print("Введите ёмкость кэша: ")
	cap, ok := readInt(reader)
	if !ok {
		fmt.Println("Error: invalid input")
		return
	}
	cache := newLFUCache(cap)

	fmt.Print("Введите количество запросов: ")
	q, ok := readInt(reader)
	if !ok {
		fmt.Println("Error: invalid input")
		return
	}

	fmt.Println("Введите запросы (например: SET 1 10, GET 1):")
	for i := 0; i < q; i++ {
		line, _ := reader.ReadString('\n')
		line = strings.TrimSpace(line)

		if line == "" {
			fmt.Println("Error: empty query")
			return
		}

		parts := strings.Fields(line)
		if len(parts) == 0 {
			continue
		}

		switch parts[0] {
		case "SET":
			if len(parts) < 3 {
				fmt.Println("Error: invalid SET format (expected: SET key value)")
				return
			}
			v, err := strconv.Atoi(parts[2])
			if err != nil {
				fmt.Println("Error: invalid value")
				return
			}
			cache.set(parts[1], v)
			fmt.Println("OK")

		case "GET":
			if len(parts) < 2 {
				fmt.Println("Error: invalid GET format (expected: GET key)")
				return
			}
			fmt.Println(cache.get(parts[1]))

		default:
			fmt.Printf("Error: unknown command '%s'\n", parts[0])
		}
	}
}