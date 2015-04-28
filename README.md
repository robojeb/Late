# Status of this project
This was a weekend project I decided to do to learn a new parsing algorithm. I will probably contine to work on the project in some freetime and I hope to eventually finish the parser generator aspect of the project.

That being said, development on the project will probably be fairly slow.

# Late
Late is a parser generator based around the Earley parsing algorithm.

Late attempts to make writing grammars easy by not having to use a lexer ahead of time. Instead when it finds a terminal symbol it greedily consumes the input stream if it matches the terminal. Additionally a terminal can be supplied as a regex for increased ease of use. 

Currently it can run on either boost or stl regex's. Latest tests (after a variety of bug-fixes) show that both perform roughly equivalently performance wise.

# Perfomance

This parser is **slow**. It runs a O(N^3) algorithm and I haven't fully optimized it. My basic grammar (which parses grammars for the parser generator) can parse a 15KB file in about 6 seconds, including constructing the AST. 

This was mostly meant for me to learn a parsing algorithm and have fun implementing it. 

That said if you see ways to improve its speed please let me know. I would love to make it faster.
