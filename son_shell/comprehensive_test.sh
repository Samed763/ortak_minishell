#!/bin/bash

echo "=== Comprehensive SIGPIPE and Pipe Tests ==="

cd /home/sae/Desktop/sigpipe/son_shell

echo "1. Testing env | head -1 (SIGPIPE scenario):"
echo -e "env | head -1\nexit" | ./minishell
echo ""

echo "2. Testing env | cat << delimiter (pipe + heredoc):"
echo -e "env | cat << delimiter\nhello world\ndelimiter\nexit" | ./minishell
echo ""

echo "3. Testing simple pipe:"
echo -e "echo hello | cat\nexit" | ./minishell
echo ""

echo "4. Testing multiple pipes:"
echo -e "echo hello | cat | cat\nexit" | ./minishell
echo ""

echo "5. Testing heredoc alone:"
echo -e "cat << eof\ntest content\neof\nexit" | ./minishell
echo ""

echo "6. Testing with valgrind for memory leaks and signals:"
echo -e "env | head -1\nexit" | timeout 10 valgrind --quiet --leak-check=no ./minishell 2>&1 | grep -E "(SIGPIPE|error|killed)" || echo "No SIGPIPE errors detected"

echo ""
echo "=== All tests completed ==="
