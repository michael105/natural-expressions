#define RE_NOMATCH 0
#define RE_MATCH 1


int match(char *text, const char *re, text_match *st_match){

	int r = 1;
	if ( ( *re == '*' && *(re+1)=='@' && ( r=2 ) ) ||
			 ( *re=='@' ) ){ // beginning of text or line, here of the text
		if ( _match( text, (re+r), st_match ) == RE_MATCH )
			return( RE_MATCH );
	}

	return( _match( text, re, st_match ) );
}

int _match(char *text, const char *re, text_match *st_match){
	int n_match=0;
	char *matchpos = 0;
	int neg = 0;


	if ( st_match ) st_match->len=0;

	while ( *text!=0 ){
		int match_char = 0;
		neg = 0;
		if ( *re == '!' ){
			re++;
			neg=1;
		}
		switch ( *re ){
			case '?':
				if ( neg )
					return(RE_NOMATCH);
				break; // matches, but only if there's a char (not 0)
			case '[':
				for ( re++; *re && *re!=*text; re++ )
					if ( *re==']' )
						return(neg ^ RE_NOMATCH);
				while ( *re && *re != ']' )
					re++;
				if ( !*re )
					return( RE_ERROR );
				if ( neg )
					return( RE_NOMATCH );
				break;
			case '#': // match end of text, or space chars; here a space
				if ( isspace( *text )){
					if ( neg ) return( RE_NOMATCH );
					break;
				}
				if ( neg ) break;
				return( RE_NOMATCH );
			case '@': // match beginning of text or endofline - here endofline
				// ( -> beginning of new line )
				if ( *text == '\n' ){
					if ( neg ) return( RE_NOMATCH );
					break;
				}
				if ( neg ) break;
				return( RE_NOMATCH );

			case '%':
				matchpos=text;
			case '+': // match one or more chars
				text++; 
				if ( !*text ) return(neg ^ RE_NOMATCH);//
			case '*': // match 0 or more chars
				re++;
				if ( *re == 0){ // match. end of regex.
					if ( matchpos && ( st_match ) ){
						while ( *text )	// find end of text
							text++;
						if ( st_match ){
							st_match->pos = matchpos;
							st_match->len = text-matchpos;
						}
					}
					return(neg ^ RE_MATCH); // no chars anymore. so a match
				}

				while ( !_match(text,re,st_match) ){
					text++;
					if ( !*text ){
						if ( (*re == '#' || *re == '$') && ( re[1]==0 ) )
							goto __MATCHEND2;
						return(neg ^ RE_NOMATCH);
					}
				}
__MATCHEND2:
				if ( matchpos ){
					if ( st_match ){
						st_match->pos = matchpos;
						st_match->len = text-matchpos;
					}
				}

				return(neg ^ RE_MATCH);

			case '\\': // match escaped *,?,backslashes, %
				re++;
#define _MATCH(a,condition) if ( *re == a ){\
	if ( neg ^ condition ) break;\
	else return(RE_NOMATCH);}

				_MATCH('d',isdigit(*text));
				_MATCH('D',!isdigit(*text));
				_MATCH('s',isspace(*text));
				_MATCH('S',!isspace(*text));
				_MATCH('w',(*text>=32 && *text <= 126 ) || ( *text>=160 ) );
				_MATCH('W',(*text<32 ) || (( *text > 126 ) && ( *text<160 )) );

			default:
				if ( *re==0 ) //partial match ( could be spared )
					return(RE_NOMATCH);
				if ( neg ^ (*re!=*text) ){
					return(RE_NOMATCH);
				}
		}
		re++; text++;
	}
	// *text == 0
	if ( ( *re=='#' ) || ( *re=='$') ){ // match end of text 
		re++;
	}

	if ( *re==0 || ( *re=='*' && re[1]==0 ) ){ 
		// * at the end. doesnt match "**", or other pathological cases
		return(RE_MATCH); //matched
	}

	return(RE_NOMATCH); 
	// regex matched text, but the regex is longer than text
	// also the case for text==0
}

