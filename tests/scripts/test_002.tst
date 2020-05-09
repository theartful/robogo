loadsgf ../../sgf_matches/Game_022.sgf 101

play black K15
color K15
#? black
findlib K15
#? J16 K14 L15 L18 M17 M16
countlib K15
#? 6

findlib G15
#? G14
countlib G15
#? 1

captures white
#? 0
play white G14
color G14
#? white


countlib G15
#? 0
color G15
#? empty
captures white
#? 2

is_legal black J16
#? true
play black J16
color J16
#? black
play white G15
color G15
#? white
play black H15
color H15
#? black
captures black
#? 1

is_legal white J15
#? false
is_suicide white J15
#? false
play white J15
color J15
#? empty
captures white
#? 2
