cmake_minimum_required(VERSION 3.0)
enable_testing()

add_test( "completions" ./test_completions.sh "al\\\\t"  "alpha.")
add_test( "double_completions" ./test_completions.sh "al\\\\tz\\\\t"  "alpha.zeta")
# basic cursor movement tests
add_test( "left_arrow"  ./test_completions.sh "alph\\\\033[Da" "alpah")
add_test( "ctrl_left_arrow"  ./test_completions.sh "alph\\\\033[1;5Da" "aalph")
# backspace key
add_test( "deletion"    ./test_completions.sh "alpha\\\\0177" "alph")
add_test( "left_arrow_deletion"  ./test_completions.sh "alph\\\\033[D\\\\0177a" "alah")
# type "al" TAB -> "alpha." should appear
# type BACKSPACE -> "alpha" should appear
# type ctrl_left -> cursor to beginning
# type "ga" TAB -> "gammaalpha should appear
add_test( "ctrl_left_arrow_compl"  ./test_completions.sh "al\\\\t\\\\0177\\\\033[1;5Dga\\\\t" "gammaalpha")
