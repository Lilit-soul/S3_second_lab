package main

import (
	"bufio"
	"fmt"
	"math"
	"os"
	"strconv"
	"strings"
)

type ChainNode struct {
	key   string
	value int
	next  *ChainNode
}

type ChainingTable struct {
	table []*ChainNode
	size  int
}

func hash1(key string, size int) int {
	h := 0
	for _, c := range key {
		h = (h*31 + int(c)) % size
	}
	return h
}

func newChainingTable(size int) *ChainingTable {
	return &ChainingTable{table: make([]*ChainNode, size), size: size}
}

func (ht *ChainingTable) insert(key string, value int) {
	idx := hash1(key, ht.size)
	curr := ht.table[idx]
	for curr != nil {
		if curr.key == key {
			curr.value = value
			return
		}
		curr = curr.next
	}
	newNode := &ChainNode{key: key, value: value, next: ht.table[idx]}
	ht.table[idx] = newNode
}

func (ht *ChainingTable) get(key string) int {
	idx := hash1(key, ht.size)
	curr := ht.table[idx]
	for curr != nil {
		if curr.key == key {
			return curr.value
		}
		curr = curr.next
	}
	return -1
}

type CuckooEntry struct {
	key     string
	value   int
	isEmpty bool
}

type CuckooTable struct {
	table1 []CuckooEntry
	table2 []CuckooEntry
	size   int
}

func hash2(key string, size int) int {
	h := 0
	for _, c := range key {
		h = (h*17 + int(c) + 7) % size
	}
	return h
}

func newCuckooTable(size int) *CuckooTable {
	t1 := make([]CuckooEntry, size)
	t2 := make([]CuckooEntry, size)
	for i := 0; i < size; i++ {
		t1[i].isEmpty = true
		t2[i].isEmpty = true
	}
	return &CuckooTable{table1: t1, table2: t2, size: size}
}

func (ht *CuckooTable) insert(key string, value int, attempt int) {
	if attempt > ht.size {
		fmt.Println("Error: Cuckoo cycle")
		return
	}

	idx1 := hash1(key, ht.size)
	if ht.table1[idx1].isEmpty {
		ht.table1[idx1].key = key
		ht.table1[idx1].value = value
		ht.table1[idx1].isEmpty = false
		return
	}

	evictedKey := ht.table1[idx1].key
	evictedValue := ht.table1[idx1].value
	ht.table1[idx1].key = key
	ht.table1[idx1].value = value

	idx2 := hash2(evictedKey, ht.size)
	if ht.table2[idx2].isEmpty {
		ht.table2[idx2].key = evictedKey
		ht.table2[idx2].value = evictedValue
		ht.table2[idx2].isEmpty = false
		return
	}

	evictedKey2 := ht.table2[idx2].key
	evictedValue2 := ht.table2[idx2].value
	ht.table2[idx2].key = evictedKey
	ht.table2[idx2].value = evictedValue

	ht.insert(evictedKey2, evictedValue2, attempt+1)
}

func (ht *CuckooTable) get(key string) int {
	idx1 := hash1(key, ht.size)
	if !ht.table1[idx1].isEmpty && ht.table1[idx1].key == key {
		return ht.table1[idx1].value
	}
	idx2 := hash2(key, ht.size)
	if !ht.table2[idx2].isEmpty && ht.table2[idx2].key == key {
		return ht.table2[idx2].value
	}
	return -1
}

type RouteEdge struct {
	to     string
	weight int
	next   *RouteEdge
}

type GraphNode struct {
	server string
	edges  *RouteEdge
	next   *GraphNode
}

type GraphHashTable struct {
	table []*GraphNode
	size  int
}

func graphHash(key string, size int) int {
	h := 0
	for _, c := range key {
		h = (h*31 + int(c)) % size
	}
	return h
}

func newGraphTable(size int) *GraphHashTable {
	return &GraphHashTable{table: make([]*GraphNode, size), size: size}
}

func (g *GraphHashTable) findNode(server string) *GraphNode {
	idx := graphHash(server, g.size)
	curr := g.table[idx]
	for curr != nil {
		if curr.server == server {
			return curr
		}
		curr = curr.next
	}
	return nil
}

func (g *GraphHashTable) getOrCreateNode(server string) *GraphNode {
	node := g.findNode(server)
	if node != nil {
		return node
	}
	node = &GraphNode{server: server, edges: nil}
	idx := graphHash(server, g.size)
	node.next = g.table[idx]
	g.table[idx] = node
	return node
}

func (g *GraphHashTable) addRoute(from, to string, weight int) {
	node := g.getOrCreateNode(from)
	curr := node.edges
	for curr != nil {
		if curr.to == to {
			curr.weight = weight
			return
		}
		curr = curr.next
	}
	newEdge := &RouteEdge{to: to, weight: weight, next: node.edges}
	node.edges = newEdge
	g.getOrCreateNode(to)
}

func (g *GraphHashTable) findMinDelay(start, end string) int {
	const MAX_NODES = 100
	var names []string
	distances := make([]int, MAX_NODES)
	visited := make([]bool, MAX_NODES)
	nodeCount := 0

	addNodeName := func(name string) {
		for i := 0; i < nodeCount; i++ {
			if names[i] == name {
				return
			}
		}
		if nodeCount < MAX_NODES {
			names = append(names, name)
			distances[nodeCount] = math.MaxInt32
			visited[nodeCount] = false
			nodeCount++
		}
	}

	for i := 0; i < g.size; i++ {
		curr := g.table[i]
		for curr != nil {
			addNodeName(curr.server)
			e := curr.edges
			for e != nil {
				addNodeName(e.to)
				e = e.next
			}
			curr = curr.next
		}
	}

	startIdx := -1
	for i := 0; i < nodeCount; i++ {
		if names[i] == start {
			startIdx = i
			break
		}
	}
	if startIdx == -1 {
		return -1
	}
	distances[startIdx] = 0

	for i := 0; i < nodeCount; i++ {
		minDist := math.MaxInt32
		minIdx := -1
		for j := 0; j < nodeCount; j++ {
			if !visited[j] && distances[j] < minDist {
				minDist = distances[j]
				minIdx = j
			}
		}
		if minIdx == -1 {
			break
		}
		visited[minIdx] = true
		u := names[minIdx]

		node := g.findNode(u)
		if node != nil {
			edge := node.edges
			for edge != nil {
				vIdx := -1
				for k := 0; k < nodeCount; k++ {
					if names[k] == edge.to {
						vIdx = k
						break
					}
				}
				if vIdx != -1 && distances[minIdx]+edge.weight < distances[vIdx] {
					distances[vIdx] = distances[minIdx] + edge.weight
				}
				edge = edge.next
			}
		}
	}

	endIdx := -1
	for i := 0; i < nodeCount; i++ {
		if names[i] == end {
			endIdx = i
			break
		}
	}
	if endIdx == -1 || distances[endIdx] == math.MaxInt32 {
		return -1
	}
	return distances[endIdx]
}

func main() {
	g := newGraphTable(100)
	reader := bufio.NewReader(os.Stdin)

	for {
		fmt.Println("\n1.Цепочки 2.Кукушка 3.Граф 0.Выход")
		fmt.Print("Выбор: ")
		line, _ := reader.ReadString('\n')
		line = strings.TrimSpace(line)
		choice, err := strconv.Atoi(line)
		if err != nil {
			fmt.Println("Error: enter a number")
			continue
		}

		if choice == 0 {
			break
		} else if choice == 1 {
			fmt.Print("\nВведите размер таблицы: ")
			line, _ := reader.ReadString('\n')
			line = strings.TrimSpace(line)
			size, err := strconv.Atoi(line)
			if err != nil || size <= 0 {
				fmt.Println("Error: invalid size")
				continue
			}
			ht := newChainingTable(size)

			fmt.Print("Введите ключи и значения (через пробел): ")
			line, _ = reader.ReadString('\n')
			parts := strings.Fields(line)
			count := 0
			for i := 0; i < len(parts)-1; i += 2 {
				k := parts[i]
				v, err := strconv.Atoi(parts[i+1])
				if err == nil && v >= 0 {
					ht.insert(k, v)
					count++
				}
			}
			fmt.Printf("Добавлено элементов: %d\n", count)

			fmt.Print("Введите ключ для поиска: ")
			line, _ = reader.ReadString('\n')
			key := strings.TrimSpace(line)
			res := ht.get(key)
			if res == -1 {
				fmt.Println("Не найдено")
			} else {
				fmt.Printf("Значение: %d\n", res)
			}

		} else if choice == 2 {
			fmt.Print("\nВведите размер таблицы: ")
			line, _ := reader.ReadString('\n')
			line = strings.TrimSpace(line)
			size, err := strconv.Atoi(line)
			if err != nil || size <= 0 {
				fmt.Println("Error: invalid size")
				continue
			}
			ht := newCuckooTable(size)

			fmt.Print("Введите ключи и значения (через пробел): ")
			line, _ = reader.ReadString('\n')
			parts := strings.Fields(line)
			count := 0
			for i := 0; i < len(parts)-1; i += 2 {
				k := parts[i]
				v, err := strconv.Atoi(parts[i+1])
				if err == nil && v >= 0 {
					ht.insert(k, v, 0)
					count++
				}
			}
			fmt.Printf("Добавлено элементов: %d\n", count)

			fmt.Print("Введите ключ для поиска: ")
			line, _ = reader.ReadString('\n')
			key := strings.TrimSpace(line)
			res := ht.get(key)
			if res == -1 {
				fmt.Println("Не найдено")
			} else {
				fmt.Printf("Значение: %d\n", res)
			}

		} else if choice == 3 {
			fmt.Println("\nВведите маршруты (Откуда Куда Задержка ...) или Enter, чтобы просто найти путь")
			fmt.Print("Маршруты: ")
			line, _ := reader.ReadString('\n')
			line = strings.TrimSpace(line)
			if line != "" {
				parts := strings.Fields(line)
				count := 0
				for i := 0; i < len(parts)-2; i += 3 {
					from := parts[i]
					to := parts[i+1]
					w, err := strconv.Atoi(parts[i+2])
					if err == nil && w > 0 {
						g.addRoute(from, to, w)
						count++
					}
				}
				if count > 0 {
					fmt.Printf("Добавлено маршрутов: %d\n", count)
				}
			}

			fmt.Print("Введите начальный сервер (Enter - пропустить поиск): ")
			start, _ := reader.ReadString('\n')
			start = strings.TrimSpace(start)
			if start == "" {
				continue
			}

			fmt.Print("Введите конечный сервер: ")
			end, _ := reader.ReadString('\n')
			end = strings.TrimSpace(end)
			if end == "" {
				fmt.Println("Error: empty end server")
				continue
			}

			res := g.findMinDelay(start, end)
			if res == -1 {
				fmt.Println("Маршрут не найден")
			} else {
				fmt.Printf("Минимальная задержка: %d\n", res)
			}
		}
	}
}
