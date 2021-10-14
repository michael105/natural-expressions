### Natural expressions


In contrast to regular expressions, this could be named natural expressions.

Somehow, imho it's way more natural than the common expression machines.
Instead of naming first the object, then the count (using analytic grammar),
and matching in a way quite often backtracking is needed
(going back and trying another matching route),
this expression machine matches as straight as possible from left to right.
Also forced by the grammar.


E.g., - instead of naming the matching chars first, then the count `( "date (\d*\.\d*\.\d*)" )`,
this works the other way: `"date{*\D}%#"`.

The first expression is a regular expression, and matches and extracts a given date.

The second one would be recognized by this machine, 
and matches and extracts with the % the date as well. It's a bad example, 
but the way to match is more or less inverted - match x times all nondigits `{*\D}`, 
extract all following chars, until a space or line end is recognized (#).

There are two big advantages - it is closer to human thoughts.
Noone says apples, two of them. There are two apples.

And the machine scans straight from left to right, 
it just scans for the first digit, then extracts up to the first space, linebreak or end of file.

While writing - me, I'm recognizing I have to work myself more with this machine.
It is not finished yet. There are "attributes" lacking. 
And I regularly stumble about the other way this thing works.
If you are common with regular expressions, it is just - the other way round.


I upload the "simple" matching engine for now, have to finish the extended version first.

---


```
text matching engine

little bit simpler version than match_ext.
Consciously named 'text matching', since the inherent logic
is quite different to a regular expression machine.

The engine matches nongreedy straight from left to right,
so backtracking is minimized.
It is a compromise between performance, size
and capabilities.


matches: 

* for every count of any char (nongreedy(!))
+ for 1 or more chars
% for 1 or more chars, and fills in arg 3 (text_match)
? for 1 char
@ matches the beginning of the text or endofline (\n) 
  -> beginning of a line
# for space, endofline, \t, \n, \f, \r, \v  or end of text (0)
$ match end of text
backslash: escape *,?,%,!,+,#,$ and backslash itself.
! : invert the matching of the next character or character class
 
[xyz]: character classes, here x,y or z 
  the characters are matched literally, also \,*,?,+,..
  it is not possible to match the closing bracket (])
  within a character class


predefined character classes:
 \d - digit
 \D - nondigit
 \s - space
 \S - nonspace
 \w - word character ( defined as ascii 32-126,160-255 )
 \W - nonword character ( defined as ascii 0-31,127-159 )


% : matches like a '+', and fills in argument 3,
the text_match struct, when the pointer is non null.
The matching is 'nongreedy'.


returns: 1 on match, 0 on no match
( RE_MATCH / RE_NOMATCH )

if the pointer (argument 3) st_match is nonnull,
the supplied struct text_match will be set to the first matching '%' location;
if there is no match, text_match.len will be set to 0.

The struct is defined as: 
typedef struct _text_match { char* pos; int len; } text_match;

examples: 
"*word*"  matches "words are true" or "true words are rare"
"word*"   matches "words are true" and not "true words are rare"
"word"    matches none of the above two texts (!)
"*words%" extracts with % " are true" and " are rare"
          into text_match

"Some\ntext\nwith\nlinebreaks\n\n"
"*@%#*" matches with % "Some"
"*@line%#*" matches % = "breaks"
"*text\n%"  % = "with linebreaks\n\n"


(memo) When the regex ist defined within C/cpp source code,
a backslash has to be defined as double backslash.

(note) - be careful when negating a following *, or ?.
 somehow - it is logical, but seems to me I overshoot a bit,
 and tapped into a logical paradox.
 Negating EVERYTHING translates to true.
 However, since truth is negated as,... well, there's a problem.

 (I'm not kidding here. Just don't do a regex with !* or !?.,
 or you might experience the meaning of full featured. 
 Maybe I should say, it's not allowed?)

 A "!+" will translate into nongreedy matching of any char, however;
 "%!+" will match with % everything but the last char;
 while "%+" matches with % only the first char.
 !+ basically sets the greedyness of the left * or % higher.
```

