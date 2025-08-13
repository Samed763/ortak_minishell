#!/bin/bash

echo "Testing env | cat << eof in minishell..."

# Create input for minishell
cat > /tmp/minishell_input << 'EOF'
env | cat << delimiter
hello world
this is a test
delimiter
exit
EOF

echo "Running the test..."
cd /home/sae/Desktop/sigpipe/son_shell
timeout 10 ./minishell < /tmp/minishell_input

echo "Test completed"
rm -f /tmp/minishell_input
