cmake_minimum_required(VERSION 3.0)
enable_testing()

add_test( "basic_completions" ./test_completions.sh "al\\\\tz\\\\t"  "alpha.zeta")
add_test( "basic_editing"     ./test_completions.sh "alpha\\\\0177" "alph")
