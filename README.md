# Late
Late is a parser generator based around the Earley parsing algorithm.

Late attempts to make writing grammars easy by not having to use a lexer ahead of time. Instead when it finds a terminal symbol it greedily consumes the input stream if it matches the terminal. Additionally a terminal can be supplied as a regex for increased ease of use. 

Currently it can run on either boost or stl regex's. Latest tests (after a variety of bug-fixes) show that both perform roughly equivalently performance wise.
