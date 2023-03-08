##
# barname haftegi
#
# @file
# @version 0.1

barname_haftegi:
	gcc -lstdc++ -Wall -Wextra -std=c++11 -fsanitize=address -fsanitize=leak main.cpp -o barname_haftegi

# end
