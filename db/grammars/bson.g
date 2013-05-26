grammar bson_grammar;

options {
	language = C;
}

@parser::includes {
//#include <stdlib.h>
   #include "bsonobj.h"
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
   
const TYPE_INT = 0;
const TYPE_FLOAT = 1;
const TYPE_PTRCHAR = 2;
const TYPE_BSON = 3;
const TYPE_BSONARRAY = 4;

struct element_value {
    int type,
    void* value
};

BSONObj* currentObj;

void addElement(char* field, struct element_value val) {
     switch (val.type) {
         case TYPE_INT:
                    currentObj->add(field, *((__int64*)val.value)));
                    break;
         case TYPE_PTRCHAR:
                    currentObj->add(field, (char*)val.value));
                    break;
         case TYPE_BSON:
                    currentObj->add(field, (BSONObj*)val.value));
                    break;
         case TYPE_BSONARRAY:
                    currentObj->add(field, (BSONArrayObj*)val.value));
                    break;
         default:
            throw "Unsupported type";
     }
}

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

start_point returns [BSONObj* val]
        @init{
	} : json_expr EOF
	{
	    $val = $json_expr.val;
	};
	
json_const returns [struct element_value val]
	: STRING 
	{
	    $val.type = TYPE_PTRCHAR;
	    char* ptext = (char*)$STRING.text->chars;
	    char* text = (char*)malloc(strlen(ptext) - 1);
	    memset(text, 0, strlen(ptext) - 1);
	    memcpy(text, ptext + 1, strlen(ptext) - 2);
	    
	    $val.value = text;
	}
	| NUMBER 
	{
	    $val.type = TYPE_INT;
		 // tries the maximum allowed value, then downsize it to the correct type
#ifdef WINDOWS
		__int64 d = (__int64)_atoi64((char*)$NUMBER.text->chars);
#else
      __int64 d = atoll((char*)$NUMBER.text->chars);
#endif
	    if (d < INT_MAX) {
	          $val = new ConstantExpression((__int32)d);
	    } else if (d < LONG_MAX)  {
	          $val = new ConstantExpression((__int64)d);
	    } else {
	         if (abs((__int64)d) == d) {
	                $val = new ConstantExpression((__int64)d);
	         } else {
	               $val = new ConstantExpression(d);
                 }
	    }
	    $val.value = new __int64(d);
	}
	| json_array_expr {
	    $val.type = TYPE_BSONARRAY;
	    $val.value = $json_array_expr.val;
	} | json_expr {
	    $val.type = TYPE_BSON;
	    $val.value = $json_expr.val;
	};
	
json_array_expr returns [BSONArrayObj* val]
@init {
    BSONArrayObj* obj = new BSONArrayObj();
    $val = obj;
}
	: LBRAK j1=json_expr 
	{
	   obj->add($j1.val);
	}
	(COMMA j2=json_expr	
	{
	   obj->add($j2.val);
	}
)* RBRAK;
	
json_expr returns [BSONObj* val]
@init {
        BSONObj* previous = currentObj;
	BSONObj* obj = new BSONObj();
	$val = obj;
	currentObj = obj;
}
@after {
    	currentObj = previous;
}	
: LBRAN n1=json_fieldname COLON v1=json_const 
{
      addElement($n1.val, $v1.val);
}
			(COMMA n2=json_fieldname COLON v2=json_const
{
      addElement($n1.val, $v1.val);
}
			)*
                  RBRAN;

json_fieldname returns [char* val]
	: STRING 
	{
	    char* ptext = (char*)$STRING.text->chars;
	    char* text = (char*)malloc(strlen(ptext) - 1);
	    memset(text, 0, strlen(ptext) - 1);
	    memcpy(text, ptext + 1, strlen(ptext) - 2);
	    
	    $val = text;
	}| ID
	{
	    char* ptext = (char*)$STRING.text->chars;
	    char* text = (char*)malloc(strlen(ptext) - 1);
	    memset(text, 0, strlen(ptext) - 1);
	    memcpy(text, ptext, strlen(ptext) - 1);
	    
	    $val = text;
	};
	
	
TRUE	:	('t'|'T')('r'|'R')('u'|'U')('e'|'E');
FALSE	:	('f'|'F')('a'|'A')('l'|'L')('s'|'S')('e'|'E');

NUMBER :	'0'..'9'+;

FLOAT
    :   NUMBER '.' (NUMBER)* EXPONENT?
    |   '.' (NUMBER)+ EXPONENT?
    |   (NUMBER)+ EXPONENT
    ;


WS  :   ( ' ' | '\t' | '\r' | '\n')+ {$channel=HIDDEN;}
    ;
COMMENT
    :   '//' ~('\n'|'\r')* '\r'? '\n' {$channel=HIDDEN;}
    |   '/*' ( options {greedy=false;} : . )* '*/' {$channel=HIDDEN;}
    ;


ID  :	('a'..'z'|'A'..'Z'|'_') ('a'..'z'|'A'..'Z'|'0'..'9'|'_')*;

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
