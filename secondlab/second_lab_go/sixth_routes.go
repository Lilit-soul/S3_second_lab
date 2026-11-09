package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

type StringArray struct {
	data []string
}
func (a *StringArray) push(s string) { a.data = append(a.data, s) }
func (a *StringArray) size() int     { return len(a.data) }
func (a *StringArray) get(i int) string { return a.data[i] }

type IntArray struct {
	data []int
}
func (a *IntArray) push(v int) { a.data = append(a.data, v) }
func (a *IntArray) size() int  { return len(a.data) }
func (a *IntArray) get(i int) int { return a.data[i] }
func (a *IntArray) set(i, v int)  { a.data[i] = v }

type BoolArray struct {
	data []bool
}
func (a *BoolArray) push(v bool) { a.data = append(a.data, v) }
func (a *BoolArray) size() int   { return len(a.data) }
func (a *BoolArray) get(i int) bool { return a.data[i] }
func (a *BoolArray) set(i int, v bool) { a.data[i] = v }

type SNode struct {
	key  string
	val  int
	next *SNode
}
type StringIntMap struct {
	table []*SNode
	size  int
}
func newStringIntMap(size int) *StringIntMap {
	return &StringIntMap{table: make([]*SNode, size), size: size}
}
func (m *StringIntMap) hash(key string) int {
	h := 0
	for _, c := range key { h = (h*31 + int(c)) % m.size }
	return h
}
func (m *StringIntMap) get(key string) (int, bool) {
	idx := m.hash(key)
	for curr := m.table[idx]; curr != nil; curr = curr.next {
		if curr.key == key { return curr.val, true }
	}
	return 0, false
}
func (m *StringIntMap) set(key string, val int) {
	idx := m.hash(key)
	for curr := m.table[idx]; curr != nil; curr = curr.next {
		if curr.key == key { curr.val = val; return }
	}
	m.table[idx] = &SNode{key: key, val: val, next: m.table[idx]}
}

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
	for _, c := range key { h = (h*31 + int(c)) % size }
	return h
}
func newChainingTable(size int) *ChainingTable {
	return &ChainingTable{table: make([]*ChainNode, size), size: size}
}
func (ht *ChainingTable) insert(key string, value int) {
	idx := hash1(key, ht.size)
	for curr := ht.table[idx]; curr != nil; curr = curr.next {
		if curr.key == key { curr.value = value; return }
	}
	ht.table[idx] = &ChainNode{key: key, value: value, next: ht.table[idx]}
}
func (ht *ChainingTable) get(key string) int {
	idx := hash1(key, ht.size)
	for curr := ht.table[idx]; curr != nil; curr = curr.next {
		if curr.key == key { return curr.value }
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
	for _, c := range key { h = (h*17 + int(c) + 7) % size }
	return h
}
func newCuckooTable(size int) *CuckooTable {
	t1 := make([]CuckooEntry, size)
	t2 := make([]CuckooEntry, size)
	for i := 0; i < size; i++ { t1[i].isEmpty = true; t2[i].isEmpty = true }
	return &CuckooTable{table1: t1, table2: t2, size: size}
}
func (ht *CuckooTable) insert(key string, value int, attempt int) {
	if attempt > ht.size { fmt.Println("Error: Cuckoo cycle"); return }
	idx1 := hash1(key, ht.size)
	if ht.table1[idx1].isEmpty {
		ht.table1[idx1].key = key; ht.table1[idx1].value = value; ht.table1[idx1].isEmpty = false
		return
	}
	ek, ev := ht.table1[idx1].key, ht.table1[idx1].value
	ht.table1[idx1].key = key; ht.table1[idx1].value = value
	idx2 := hash2(ek, ht.size)
	if ht.table2[idx2].isEmpty {
		ht.table2[idx2].key = ek; ht.table2[idx2].value = ev; ht.table2[idx2].isEmpty = false
		return
	}
	ek2, ev2 := ht.table2[idx2].key, ht.table2[idx2].value
	ht.table2[idx2].key = ek; ht.table2[idx2].value = ev
	ht.insert(ek2, ev2, attempt+1)
}
func (ht *CuckooTable) get(key string) int {
	idx1 := hash1(key, ht.size)
	if !ht.table1[idx1].isEmpty && ht.table1[idx1].key == key { return ht.table1[idx1].value }
	idx2 := hash2(key, ht.size)
	if !ht.table2[idx2].isEmpty && ht.table2[idx2].key == key { return ht.table2[idx2].value }
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
	for _, c := range key { h = (h*31 + int(c)) % size }
	return h
}
func newGraphTable(size int) *GraphHashTable {
	return &GraphHashTable{table: make([]*GraphNode, size), size: size}
}
func (g *GraphHashTable) findNode(server string) *GraphNode {
	idx := graphHash(server, g.size)
	for curr := g.table[idx]; curr != nil; curr = curr.next {
		if curr.server == server { return curr }
	}
	return nil
}
func (g *GraphHashTable) getOrCreateNode(server string) *GraphNode {
	if n := g.findNode(server); n != nil { return n }
	n := &GraphNode{server: server}
	idx := graphHash(server, g.size)
	n.next = g.table[idx]
	g.table[idx] = n
	return n
}
func (g *GraphHashTable) addRoute(from, to string, weight int) {
	node := g.getOrCreateNode(from)
	for e := node.edges; e != nil; e = e.next {
		if e.to == to { e.weight = weight; return }
	}
	node.edges = &RouteEdge{to: to, weight: weight, next: node.edges}
	g.getOrCreateNode(to)
}

const maxInt = int(^uint(0) >> 1)

func (g *GraphHashTable) findMinDelay(start, end string) int {
	names := &StringArray{}
	distances := &IntArray{}
	visited := &BoolArray{}
	index := newStringIntMap(1024) // FIX: вместо map[string]int

	addNode := func(name string) int {
		if idx, ok := index.get(name); ok { return idx }
		idx := names.size()
		names.push(name)
		index.set(name, idx)
		distances.push(maxInt)
		visited.push(false)
		return idx
	}

	for i := 0; i < g.size; i++ {
		for curr := g.table[i]; curr != nil; curr = curr.next {
			addNode(curr.server)
			for e := curr.edges; e != nil; e = e.next { addNode(e.to) }
		}
	}

	startIdx, ok := index.get(start)
	if !ok { return -1 }
	distances.set(startIdx, 0)

	for i := 0; i < names.size(); i++ {
		minDist := maxInt
		minIdx := -1
		for j := 0; j < names.size(); j++ {
			if !visited.get(j) && distances.get(j) < minDist {
				minDist = distances.get(j); minIdx = j
			}
		}
		if minIdx == -1 { break }
		visited.set(minIdx, true)
		u := names.get(minIdx)

		node := g.findNode(u)
		if node != nil {
			for e := node.edges; e != nil; e = e.next {
				if vIdx, ok := index.get(e.to); ok {
					if distances.get(minIdx)+e.weight < distances.get(vIdx) {
						distances.set(vIdx, distances.get(minIdx)+e.weight)
					}
				}
			}
		}
	}

	endIdx, ok := index.get(end)
	if !ok || distances.get(endIdx) == maxInt { return -1 }
	return distances.get(endIdx)
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
		if err != nil { fmt.Println("Error: enter a number"); continue }
		if choice == 0 { break }

		if choice == 1 {
			fmt.Print("\nВведите размер таблицы: ")
			line, _ = reader.ReadString('\n')
			size, err := strconv.Atoi(strings.TrimSpace(line))
			if err != nil || size <= 0 { fmt.Println("Error: invalid size"); continue }
			ht := newChainingTable(size)
			fmt.Print("Введите ключи и значения (через пробел): ")
			line, _ = reader.ReadString('\n')
			parts := strings.Fields(line)
			count := 0
			for i := 0; i < len(parts)-1; i += 2 {
				v, err := strconv.Atoi(parts[i+1])
				if err == nil && v >= 0 { ht.insert(parts[i], v); count++ }
			}
			fmt.Printf("Добавлено элементов: %d\n", count)
			fmt.Print("Введите ключ для поиска: ")
			line, _ = reader.ReadString('\n')
			key := strings.TrimSpace(line)
			res := ht.get(key)
			if res == -1 { fmt.Println("Не найдено") } else { fmt.Printf("Значение: %d\n", res) }

		} else if choice == 2 {
			fmt.Print("\nВведите размер таблицы: ")
			line, _ = reader.ReadString('\n')
			size, err := strconv.Atoi(strings.TrimSpace(line))
			if err != nil || size <= 0 { fmt.Println("Error: invalid size"); continue }
			ht := newCuckooTable(size)
			fmt.Print("Введите ключи и значения (через пробел): ")
			line, _ = reader.ReadString('\n')
			parts := strings.Fields(line)
			count := 0
			for i := 0; i < len(parts)-1; i += 2 {
				v, err := strconv.Atoi(parts[i+1])
				if err == nil && v >= 0 { ht.insert(parts[i], v, 0); count++ }
			}
			fmt.Printf("Добавлено элементов: %d\n", count)
			fmt.Print("Введите ключ для поиска: ")
			line, _ = reader.ReadString('\n')
			key := strings.TrimSpace(line)
			res := ht.get(key)
			if res == -1 { fmt.Println("Не найдено") } else { fmt.Printf("Значение: %d\n", res) }

		} else if choice == 3 {
			fmt.Println("\nВведите маршруты (Откуда Куда Задержка ...) или Enter:")
			fmt.Print("Маршруты: ")
			line, _ := reader.ReadString('\n')
			line = strings.TrimSpace(line)
			if line != "" {
				parts := strings.Fields(line)
				count := 0
				for i := 0; i < len(parts)-2; i += 3 {
					w, err := strconv.Atoi(parts[i+2])
					if err == nil && w > 0 { g.addRoute(parts[i], parts[i+1], w); count++ }
				}
				if count > 0 { fmt.Printf("Добавлено маршрутов: %d\n", count) }
			}
			fmt.Print("Введите начальный сервер (Enter - пропустить): ")
			start, _ := reader.ReadString('\n')
			start = strings.TrimSpace(start)
			if start == "" { continue }
			fmt.Print("Введите конечный сервер: ")
			end, _ := reader.ReadString('\n')
			end = strings.TrimSpace(end)
			if end == "" { fmt.Println("Error: empty end server"); continue }
			res := g.findMinDelay(start, end)
			if res == -1 { fmt.Println("Маршрут не найден") } else { fmt.Printf("Минимальная задержка: %d\n", res) }
		}
	}
}