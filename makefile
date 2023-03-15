##
# barname haftegi
#
# @file
# @version 0.1

barname_haftegi: main.cpp
	gcc -lstdc++ -Wall -Wextra -std=c++11 -fsanitize=address -fsanitize=leak main.cpp -o barname-haftegi

# end
