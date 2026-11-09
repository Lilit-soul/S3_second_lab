package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

type StackNode struct {
	data int
	next *StackNode
}

type MyStack struct {
	topNode *StackNode
}

func (s *MyStack) isEmpty() bool { return s.topNode == nil }
func (s *MyStack) push(val int)  { s.topNode = &StackNode{data: val, next: s.topNode} }
func (s *MyStack) pop() int {
	if s.isEmpty() { return -1 }
	v := s.topNode.data
	s.topNode = s.topNode.next
	return v
}
func (s *MyStack) top() int {
	if s.isEmpty() { return -1 }
	return s.topNode.data
}
func (s *MyStack) clear() { for !s.isEmpty() { s.pop() } }

func applyOp(op byte, a, b int) int {
	switch op {
	case '&': return a & b
	case '|': return a | b
	case '^': return a ^ b
	case '!':
		if a == 0 { return 1 }
		return 0
	}
	return 0
}

func precedence(op byte) int {
	switch op {
	case '!': return 3
	case '&': return 2
	case '|', '^': return 1
	}
	return 0
}

func isDigit(c byte) bool { return c >= '0' && c <= '9' }

// Безопасное применение унарного '!'
func applyUnary(values *MyStack) bool {
	if values.isEmpty() {
		fmt.Println("Error: missing operand for '!'")
		return false
	}
	values.push(applyOp('!', values.pop(), 0))
	return true
}

func evaluateExpression(expr string) (int, bool) {
	values := &MyStack{}
	ops := &MyStack{}
	defer values.clear()
	defer ops.clear()

	expectOperand := true
	for i := 0; i < len(expr); i++ {
		c := expr[i]
		if c == ' ' || c == '\t' { continue }

		if isDigit(c) {
			num := 0
			for i < len(expr) && isDigit(expr[i]) {
				num = num*10 + int(expr[i]-'0')
				i++
			}
			i--
			if num != 0 && num != 1 {
				fmt.Println("Error: operands must be 0 or 1")
				return 0, false
			}
			if !expectOperand {
				fmt.Println("Error: missing operator between operands")
				return 0, false
			}
			values.push(num)
			expectOperand = false
		} else if c == '(' {
			if !expectOperand {
				fmt.Println("Error: missing operator before '('")
				return 0, false
			}
			ops.push(int(c))
		} else if c == ')' {
			if expectOperand {
				fmt.Println("Error: missing operand before ')'")
				return 0, false
			}
			for !ops.isEmpty() && ops.top() != int('(') {
				op := byte(ops.pop())
				if op == '!' {
					if !applyUnary(values) { return 0, false }
				} else {
					if values.isEmpty() { fmt.Println("Error: missing operand"); return 0, false }
					b := values.pop()
					if values.isEmpty() { fmt.Println("Error: missing operand"); return 0, false }
					a := values.pop()
					values.push(applyOp(op, a, b))
				}
			}
			if ops.isEmpty() {
				fmt.Println("Error: mismatched parentheses")
				return 0, false
			}
			ops.pop()
			expectOperand = false
		} else if c == '&' || c == '|' || c == '^' {
			if expectOperand {
				fmt.Println("Error: missing operand before operator")
				return 0, false
			}
			for !ops.isEmpty() && precedence(byte(ops.top())) >= precedence(c) {
				op := byte(ops.pop())
				if op == '!' {
					if !applyUnary(values) { return 0, false }
				} else {
					b := values.pop()
					a := values.pop()
					values.push(applyOp(op, a, b))
				}
			}
			ops.push(int(c))
			expectOperand = true
		} else if c == '!' {
			if !expectOperand {
				fmt.Println("Error: missing operator before '!'")
				return 0, false
			}
			for !ops.isEmpty() && precedence(byte(ops.top())) >= precedence(c) {
				op := byte(ops.pop())
				if op == '!' {
					if !applyUnary(values) { return 0, false }
				} else {
					b := values.pop()
					a := values.pop()
					values.push(applyOp(op, a, b))
				}
			}
			ops.push(int(c))
		} else {
			fmt.Println("Error: invalid character")
			return 0, false
		}
	}

	if expectOperand {
		fmt.Println("Error: missing operand at the end")
		return 0, false
	}

	for !ops.isEmpty() {
		op := byte(ops.pop())
		if op == '(' {
			fmt.Println("Error: mismatched parentheses")
			return 0, false
		}
		if op == '!' {
			if !applyUnary(values) { return 0, false }
		} else {
			if values.isEmpty() { fmt.Println("Error: missing operand"); return 0, false }
			b := values.pop()
			if values.isEmpty() { fmt.Println("Error: missing operand"); return 0, false }
			a := values.pop()
			values.push(applyOp(op, a, b))
		}
	}

	if values.isEmpty() {
		fmt.Println("Error: empty expression")
		return 0, false
	}
	result := values.pop()
	if !values.isEmpty() {
		fmt.Println("Error: too many operands")
		return 0, false
	}
	return result, true
}

func main() {
	reader := bufio.NewReader(os.Stdin)
	fmt.Print("Введите логическое выражение (0, 1, &, |, ^, !): ")
	expr, _ := reader.ReadString('\n')
	expr = strings.TrimSpace(expr)

	hasDigit := false
	for _, c := range expr {
		if c == '0' || c == '1' { hasDigit = true; break }
	}
	if expr == "" || !hasDigit {
		fmt.Println("Error: empty or invalid expression")
		return
	}

	if result, ok := evaluateExpression(expr); ok {
		fmt.Printf("Результат: %d\n", result)
	}
}