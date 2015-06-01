cmake_minimum_required(VERSION 3.0)
enable_testing()

add_test( "completions" ./test_completions.sh "al\\\\t"  "alpha.")
add_test( "double_completions" ./test_completions.sh "al\\\\tz\\\\t"  "alpha.zeta")
add_test( "completions_last" ./test_completions.sh "ze\\\\t"  "zeta")
# basic cursor movement tests
# left \\\\033[D
# ctrl+left \\\\033[1;5D
add_test( "left_arrow"  ./test_completions.sh "alph\\\\033[Da" "alpah")
add_test( "ctrl_left_arrow"  ./test_completions.sh "alph\\\\033[1;5Da" "aalph")
add_test( "ctrl_left_arrow2"  ./test_completions.sh "zeta alph\\\\033[1;5Da" "zetaa alph")
add_test( "ctrl_left_arrow3"  ./test_completions.sh "zeta.alph\\\\033[1;5Da" "zetaa.alph")
# home key \\\\033[7\\\\x7e
add_test( "home_key"  ./test_completions.sh "alph\\\\033[7\\\\x7ea" "aalph")
add_test( "home_key_long"  ./test_completions.sh "alpha beta gamm\\\\033[7\\\\x7ea" "aalpha beta gamm")

# home key and right arrow \\\\033[C
add_test( "home_right"  ./test_completions.sh "alph\\\\033[7\\\\x7e\\\\033[CZ" "aZlph")

# home key and ctrl+right arrow \\\\033[C
add_test( "home_ctrl_right"  ./test_completions.sh "alpha bet\\\\033[7\\\\x7e\\\\033[1;5CZ" "alphaZ bet")

# backspace key
add_test( "deletion"    ./test_completions.sh "alpha\\\\0177" "alph")
add_test( "left_arrow_deletion"  ./test_completions.sh "alph\\\\033[D\\\\0177a" "alah")
# type "al" TAB -> "alpha." should appear
# type BACKSPACE -> "alpha" should appear
# type ctrl_left -> cursor to beginning
# type "ga" TAB -> "gammaalpha should appear
add_test( "ctrl_left_arrow_compl"  ./test_completions.sh "al\\\\t\\\\0177\\\\033[1;5Dga\\\\t" "gammaalpha")

# killing tokens

add_test( "killing_tokens"  ./test_completions.sh "alpha gamma beta\\\\033[1;5D\\\\033[D\\\\027" "alpha beta")


add_test( "killing_tokens_and_appending"  ./test_completions.sh "alpha gamma beta\\\\033[1;5D\\\\033[D\\\\027\\\\033[8\\\\x7e gamma" "alpha beta gamma")

# empty history
add_test( "empty_history"    ./test_history.sh "history" "history")

add_test( "multi_lines"    ./test_completions.sh "ls\\\\nll\\\\nlz" "lz")

# single up
add_test( "multi_lines_last_empty"    ./test_completions.sh "ls\\\\nll\\\\nlz\\\\n" "")

add_test( "multi_lines_last_history"    ./test_completions.sh "ls\\\\nll\\\\nlz\\\\n\\\\033[A\\\\033[A" "ll")

add_test( "history_rewind"    ./test_completions.sh "ls\\\\nll\\\\nlz\\\\n\\\\033[A\\\\033[A\\\\033[A\\\\033[A" "lz")

add_test( "history_content"    ./test_history.sh "ls\\\\nll\\\\nlz\\\\n\\\\033[A\\\\033[A\\\\033[A\\\\033[A\\\\nhistory" "ls\\\\nll\\\\nlz\\\\nhistory")

# test possible completions 
add_test( "possible_completions"  ./possible_completions.sh )
