dmenu = {}

-- -b  option; if false, dmenu appears at bottom
dmenu.topbar = true

dmenu.fonts = {
	"monospace:size=10"
}

-- -p  option; prompt to the left of input field
dmenu.prompt = ""

-- colors
dmenu.colors = {
    SchemeNorm = { "#bbbbbb", "#222222" },
    SchemeSel = { "#eeeeee", "#005577" },
    SchemeOut = { "#000000", "#00ffff" },
}

-- -l option; if nonzero, dmenu uses vertical list with given number of lines
dmenu.lines = 0

-- Characters not considered part of a word while deleting words
--  for example: " /?\"&[]"
dmenu.worddelimiters = ""
