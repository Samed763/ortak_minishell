#!/bin/bash

echo "Testing minishell with debug..."

# Create input for minishell
cat > /tmp/minishell_debug_input << 'EOF'
env | cat << delimiter
hello world
delimiter
exit
EOF

echo "Running with timeout and strace..."
cd /home/sae/Desktop/sigpipe/son_shell
timeout 5 strace -o /tmp/minishell_trace.txt ./minishell < /tmp/minishell_debug_input 2>/dev/null

echo "Exit code: $?"
echo "Last few lines of trace:"
tail -20 /tmp/minishell_trace.txt

rm -f /tmp/minishell_debug_input /tmp/minishell_trace.txt
