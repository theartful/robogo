loadsgf games/Game_023.sgf 23

play black F19

captures black 
#? 1
countlib F18
#? 0
color F18
#? empty

play white F18
color F18
#? empty
is_legal white F18
#? false
is_suicide white F18
#? true

is_legal white F19
#? false
play white F19 
color F19
#? black

countlib O17
#? 2
findlib O17
#? N17
#? O16

play white B17
# no captures, when no attack
captures white
#? 0
