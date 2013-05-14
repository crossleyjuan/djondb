grammar dql;

options {
	language = C;
}

@parser::includes {
//#include <stdlib.h>
   #include "util.h"
   #include "filterdefs.h"
   #include "constantexpression.h"
   #include "unaryexpression.h"
   #include "simpleexpression.h"
   #include "binaryexpression.h"
   #include "findcommand.h"
   #include "insertcommand.h"
   #include "updatecommand.h"
   #include "removecommand.h"
   #include "memorystream.h"
   #include <stdlib.h>
   #include <limits.h>
   #include <stdio.h>
   #ifndef WINDOWS
   #include <strings.h>
#endif
   #include <string>
   #include <iostream>
}

@postinclude {
static void displayRecognitionErrorNew  (pANTLR3_BASE_RECOGNIZER recognizer, pANTLR3_UINT8 * tokenNames) throw(ParseException)
{ 
/*
    switch (recognizer->state->exception->type) {
    	case ANTLR3_UNWANTED_TOKEN_EXCEPTION:
		break;
	default:
		throw ParseException(1, (char*)recognizer->state->exception->message);
    }
    */
}
static void reportOverride(pANTLR3_BASE_RECOGNIZER recognizer) {
}
 }

@parser::apifuncs {
 RECOGNIZER->displayRecognitionError       = displayRecognitionErrorNew;
// RECOGNIZER->reportError = reportOverride;
//  RECOGNIZER->antlr3RecognitionExceptionNew = antlr3RecognitionExceptionNewNew;
//  RECOGNIZER->mismatch                      = mismatchNew;
}

@rulecatch {
}

start_point	returns [Command* val]
	:	dql {
	    $val = $dql.val;
	};

dql	returns [Command* val]
	:   query_expr { $val = $query_expr.val; } 
	| insert_expr  { $val = $insert_expr.val; } 
	| update_expr  { $val = $update_expr.val; } 
	| remove_expr  { $val = $remove_expr.val; };

query_expr	returns [Command* val]
@init {
     FindCommand* cmd = new FindCommand();
     BSONObj options;
     MemoryStream ms(500);
     $val = cmd;
}
	:	SELECT (TOP top=NUMBER {
#ifdef WINDOWS
		__int64 d = (__int64)_atoi64((char*)$top.text->chars);
#else
      __int64 d = atoll((char*)$NUMBER.text->chars);
#endif
	    options.add("limit", d);
    	    cmd->setOptions(&options);
	})? (ALL_FIELDS {
	    cmd->setSelect("*");
	}| (x1=XPATH {
	    std::string t1((char*)$x1.text->chars);
	    ms.writeRaw(t1.c_str(), t1.length());
	} (COMMA x2=XPATH)* {
	    std::string t2((char*)$x2.text->chars);
	    ms.writeRaw(", ", 2);
	    ms.writeRaw(t2.c_str(), t2.length());
	}) {
	    cmd->setSelect(ms.toChars());
	}) FROM d1=DB_NS {
	    cmd->setDB(std::string((char*)$d1.text->chars));
	} COLON ns=DB_NS {
	    cmd->setNameSpace(std::string((char*)$ns.text->chars));
	} (WHERE filter=filter_expr {
	    cmd->setFilter(std::string((char*)$filter.text->chars));
	})?;

insert_expr returns [Command* val]
@init {
     InsertCommand* cmd = new InsertCommand();
     $val = cmd;
}	: INSERT json_expr {
		BSONObj* obj = BSONParser::parse((char*)$json_expr.text->chars);
		cmd->setBSON(obj);
		if (!obj->has("_id")) { 
		    std::string* id = uuid(); 
		    obj->add("_id", const_cast<char*>(id->c_str())); 
		    delete id; 
		} 
		if (!obj->has("_revision")) { 
		    std::string* rev = uuid(); 
		    obj->add("_revision", const_cast<char*>(rev->c_str())); 
		    delete rev; 
		} 
	} INTO db=DB_NS COLON ns=DB_NS {
	    cmd->setDB(std::string((char*)$db.text->chars));
	    cmd->setNameSpace(std::string((char*)$ns.text->chars));
	};

update_expr returns [Command* val]
@init {
     UpdateCommand* cmd = new UpdateCommand();
     $val = cmd;
}	:	UPDATE json_expr {
		BSONObj* obj = BSONParser::parse((char*)$json_expr.text->chars);
		
		if (!obj->has("_id") || !obj->has("_revision")) {
			delete obj;
			delete cmd;
			throw DjondbException(D_ERROR_INVALID_STATEMENT, "The update command requires a document with _id and _revision.");
		}
		cmd->setBSON(*obj);
		delete obj;
	} INTO db=DB_NS COLON ns=DB_NS {
	    cmd->setDB(std::string((char*)$db.text->chars));
	    cmd->setNameSpace(std::string((char*)$ns.text->chars));
	};
	
remove_expr returns [Command* val]
@init {
     RemoveCommand* cmd = new RemoveCommand();
     $val = cmd;
}:	REMOVE id=STRING {
	    cmd->setId(std::string((char*)$id.text->chars));
        } (WITH rev=STRING {
	    cmd->setRevision(std::string((char*)$rev.text->chars));
        })? FROM db=DB_NS COLON ns=DB_NS {
	    cmd->setDB(std::string((char*)$db.text->chars));
	    cmd->setNameSpace(std::string((char*)$ns.text->chars));
	};

filter_expr :	boolean_expr ;

boolean_expr 
	:	b1=boolean_term 
	(OR b2=boolean_term);

boolean_term 
	:	b1=boolean_value
	 (AND b2=boolean_value);
	
boolean_value
	:	parenthesized_boolean  |
	nonparentherized_boolean;	
	
parenthesized_boolean
	: LPAREN boolean_expr RPAREN;
	
nonparentherized_boolean 
	: u1=unary_expr  ( OPER u2=unary_expr );


		
unary_expr
	: (c1=constant_expr | x1=xpath_expr);
	
xpath_expr
	: XPATH;

id_expr	: ID;

constant_expr
	: (NUMBER | STRING);

operand_expr
	: OPER;

json_const 
	: STRING | NUMBER | json_array_expr | json_expr;
	
json_array_expr
	: LBRAK json_expr (COMMA json_expr)* RBRAK;
	
json_expr	: LBRAN json_fieldname COLON json_const
			(COMMA json_fieldname COLON json_const)*
                  RBRAN;
                  
json_fieldname
	: STRING;
	
NUMBER :	'0'..'9'+;
fragment LETTER :	'a'..'z' | 'A'..'Z';
fragment ID	:	LETTER (LETTER | NUMBER | '_' | '.')*;
fragment DOLLAR 	: '$';
fragment ADM   	        : ':';
	
XPATH   : DOLLAR STRING;

SELECT	:	('s'|'S') ('e'|'E') ('l'|'L') ('e'|'E') ('c'|'C') ('t'|'T');
INSERT	:	('i'|'I')('n'|'N')('s'|'S')('e'|'E')('r'|'R')('t'|'T');
UPDATE	:	('u'|'U')('p'|'P')('d'|'D')('a'|'A')('t'|'T')('e'|'E');
REMOVE	:	('r'|'R')('e'|'E')('m'|'M')('o'|'O')('v'|'V')('e'|'E');
FROM	:	('f'|'F')('r'|'R') ('o'|'O') ('m'|'M');
WHERE	:	('w'|'W')('h'|'H')('e'|'E')('r'|'R')('e'|'E');
INTO	:	('i'|'I')('n'|'N')('t'|'T')('o'|'O');
WITH	:	('w'|'W')('i'|'I')('t'|'T')('h'|'H');
NOT	:	('n'|'N')('o'|'O')('t'|'T');
OPER	:	('==' | '>' | '>=' | '<' | '<=' | '!=' );
OR	:	('o' | 'O') ('R' | 'r');
AND	:	('a' | 'A') ('n' | 'N') ('d' | 'D');
TOP	:	('t'|'T')('o'|'O')('p'|'P');

DB_NS
	:	ID;
ALL_FIELDS
	:	'*';

FLOAT
    :   NUMBER '.' (NUMBER)* EXPONENT?
    |   '.' (NUMBER)+ EXPONENT?
    |   (NUMBER)+ EXPONENT
    ;

COMMENT
    :   '//' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    |   '/*' ( options {greedy=false;} : . )* '*/' {$channel=HIDDEN;}
    ;

WS  :   ( ' ' | '\t' | '\r' | '\n')+ {$channel=HIDDEN;}
    ;

STRING 		: 	'\"' ( options{ greedy=false; }: (~('\"') | ('\\"')) )* '\"' | '\'' ( options{ greedy=false; }: (~('\'') | ('\\\'')) )* '\'' ;

fragment
EXPONENT : ('e'|'E') ('+'|'-')? ('0'..'9')+ ;

fragment
HEX_DIGIT : ('0'..'9'|'a'..'f'|'A'..'F') ;

fragment
ESC_SEQ
    :   '\\' ('b'|'t'|'n'|'f'|'r'|'\"'|'\''|'\\')
    |   UNICODE_ESC
        |   OCTAL_ESC
    ;

fragment
OCTAL_ESC
    :   '\\' ('0'..'3') ('0'..'7') ('0'..'7')
    |   '\\' ('0'..'7') ('0'..'7')
    |   '\\' ('0'..'7')
    ;

fragment
UNICODE_ESC
    :   '\\' 'u' HEX_DIGIT HEX_DIGIT HEX_DIGIT HEX_DIGIT
    ;

COMMA
	:	',';
LPAREN :	'(';
RPAREN :	')';
LBRAN :	'{';
RBRAN :	'}';
LBRAK
	:	'[';
RBRAK
	:	']';
COLON
	:	':';
SEMICOLON
        :       ';';

// CHAR:  '\'' ( ESC_SEQ | ~('\''|'\\') ) '\''
//    ;
