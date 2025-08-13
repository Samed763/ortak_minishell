#!/bin/bash

# Test script for pipe management
# Testing the specific cases mentioned in the problem

echo "Testing pipe management with valgrind..."

# Test 1: Basic pipe
echo "=== Test 1: Basic pipe ==="
echo "echo hello | cat" | valgrind --leak-check=full --show-leak-kinds=all --suppressions=minishell.supp --track-fds=yes ./minishell

echo ""

# Test 2: Multiple pipes
echo "=== Test 2: Multiple pipes ==="
echo "echo test | cat | wc -l" | valgrind --leak-check=full --show-leak-kinds=all --suppressions=minishell.supp --track-fds=yes ./minishell

echo ""

# Test 3: Pipe with heredoc (simplified)
echo "=== Test 3: Pipe with redirection ==="
echo "echo test | cat > /tmp/test_output" | valgrind --leak-check=full --show-leak-kinds=all --suppressions=minishell.supp --track-fds=yes ./minishell

echo ""

# Test 4: Complex pipe chain
echo "=== Test 4: Complex pipe chain ==="
echo "ls | grep test | wc -l" | valgrind --leak-check=full --show-leak-kinds=all --suppressions=minishell.supp --track-fds=yes ./minishell

echo ""

# Test 5: Environment variable in pipe
echo "=== Test 5: Environment command in pipe ==="
echo "env | head -5" | valgrind --leak-check=full --show-leak-kinds=all --suppressions=minishell.supp --track-fds=yes ./minishell

echo ""

echo "All tests completed!"
