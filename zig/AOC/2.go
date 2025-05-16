package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

func isSafe(values []int) bool {
	if len(values) < 2 {
		return false
	}

	diff := values[1] - values[0]
	if diff == 0 || diff < -3 || diff > 3 {
		return false
	}

	increasing := diff > 0

	for i := 1; i < len(values); i++ {
		step := values[i] - values[i-1]

		if increasing {
			if step < 1 || step > 3 {
				return false
			}
		} else {
			if step > -1 || step < -3 {
				return false
			}
		}
	}

	return true
}

func main() {
	file, err := os.Open("input2")
	if err != nil {
		panic(err)
	}
	defer file.Close()

	scanner := bufio.NewScanner(file)
	count := 0

	for scanner.Scan() {
		line := strings.TrimSpace(scanner.Text())
		if line == "" {
			continue
		}
		parts := strings.Fields(line)
		nums := make([]int, 0, len(parts))
		for _, part := range parts {
			n, err := strconv.Atoi(part)
			if err != nil {
				fmt.Printf("Invalid number: %s\n", part)
				continue
			}
			nums = append(nums, n)
		}

		if isSafe(nums) {
			count++
		}
	}

	fmt.Printf("Safe count: %d\n", count)
}
