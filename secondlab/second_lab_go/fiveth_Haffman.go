package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

type Node struct {
	ch    byte
	freq  int
	left  *Node
	right *Node
}

type ListNode struct {
	treeNode *Node
	next     *ListNode
}

func insertSorted(head **ListNode, treeNode *Node) {
	newNode := &ListNode{treeNode: treeNode, next: nil}

	if *head == nil || (*head).treeNode.freq >= treeNode.freq {
		newNode.next = *head
		*head = newNode
		return
	}

	curr := *head
	for curr.next != nil && curr.next.treeNode.freq < treeNode.freq {
		curr = curr.next
	}
	newNode.next = curr.next
	curr.next = newNode
}

func popMin(head **ListNode) *ListNode {
	if *head == nil {
		return nil
	}
	temp := *head
	*head = (*head).next
	return temp
}

func buildHuffmanTree(text string) *Node {
	freq := make([]int, 256)
	for i := 0; i < len(text); i++ {
		freq[text[i]]++
	}

	var head *ListNode
	for i := 0; i < 256; i++ {
		if freq[i] > 0 {
			n := &Node{ch: byte(i), freq: freq[i]}
			insertSorted(&head, n)
		}
	}

	for head != nil && head.next != nil {
		leftList := popMin(&head)
		rightList := popMin(&head)

		parent := &Node{
			ch:    0,
			freq:  leftList.treeNode.freq + rightList.treeNode.freq,
			left:  leftList.treeNode,
			right: rightList.treeNode,
		}
		insertSorted(&head, parent)
	}

	if head == nil {
		return nil
	}
	return head.treeNode
}

func generateCodes(root *Node, code string, chars *[]byte, codes *[]string) {
	if root == nil {
		return
	}
	if root.left == nil && root.right == nil {
		(*chars)[root.ch] = root.ch
		(*codes)[root.ch] = code
	}
	generateCodes(root.left, code+"0", chars, codes)
	generateCodes(root.right, code+"1", chars, codes)
}

func encode(text string, codes []string) string {
	var sb strings.Builder
	for i := 0; i < len(text); i++ {
		sb.WriteString(codes[text[i]])
	}
	return sb.String()
}

func decode(root *Node, encoded string) string {
	var sb strings.Builder
	curr := root
	for i := 0; i < len(encoded); i++ {
		if encoded[i] == '0' {
			curr = curr.left
		} else {
			curr = curr.right
		}
		if curr.left == nil && curr.right == nil {
			sb.WriteByte(curr.ch)
			curr = root
		}
	}
	return sb.String()
}

func clearTree(root *Node) {
	if root == nil {
		return
	}
	clearTree(root.left)
	clearTree(root.right)
}

func main() {
	reader := bufio.NewReader(os.Stdin)
	fmt.Print("Введите текст для кодирования: ")
	text, _ := reader.ReadString('\n')
	text = strings.TrimSpace(text)

	if text == "" {
		fmt.Println("Error: empty text")
		return
	}

	root := buildHuffmanTree(text)
	if root == nil {
		return
	}

	chars := make([]byte, 256)
	codes := make([]string, 256)
	generateCodes(root, "", &chars, &codes)

	fmt.Println("\nТаблица кодов:")
	for i := 0; i < 256; i++ {
		if codes[i] != "" {
			fmt.Printf("%c: %s\n", chars[i], codes[i])
		}
	}

	encoded := encode(text, codes)
	fmt.Printf("\nЗакодированная строка: %s\n", encoded)

	decoded := decode(root, encoded)
	fmt.Printf("Декодированная строка: %s\n", decoded)

	clearTree(root)
}
