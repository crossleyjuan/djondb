// =====================================================================================
//  Filename:  filterparser.cpp
// 
//  Description:  This file contains the implementation of the expression parser used
//                to create the vocabulary and the logic to analyze where expressions.
// 
//  Version:  1.0
//  Created:  04/24/2012 09:45:47 AM
//  Revision:  none
//  Compiler:  gcc
// 
//  Author:  Juan Pablo Crossley (crossley@gmail.com), 
// 
// License:
// 
// This file is part of the djondb project, for license information please refer to the LICENSE file,
// the application and libraries are provided as-is and free of use under the terms explained in the file LICENSE
// Its authors create this application in order to make the world a better place to live, but you should use it on
// your own risks.
// 
// Also, be adviced that, the GPL license force the committers to ensure this application will be free of use, thus
// if you do any modification you will be required to provide it for free unless you use it for personal use (you may 
// charge yourself if you want), bare in mind that you will be required to provide a copy of the license terms that ensures
// this program will be open sourced and all its derivated work will be too.
// =====================================================================================

#include "filterparser.h"
#include <string.h>
#include "util.h"
#include "baseexpression.h"
#include "constantexpression.h"
#include "unaryexpression.h"
#include "simpleexpression.h"
#include "binaryexpression.h"
#include "expressionresult.h"
#include "filter_expressionLexer.h"
#include "filter_expressionParser.h"
#include    <antlr3treeparser.h>
#include    <antlr3defs.h>

BaseExpression* solveExpression(std::list<Token*>& tokens, std::list<Token*>::iterator& i);
const int BUFFER_SIZE = 1024;

FilterParser::FilterParser(const std::string& expression, BaseExpression* root, std::list<Token*> tokens):
	_expression(expression)
{
	_root = root;
	_tokens = tokens;
}

FilterParser::FilterParser(const FilterParser& orig) {
	_expression = orig._expression;
	_root = NULL;
	if (orig._root != NULL) {
		EXPRESSION_TYPE type = orig._root->type();
		switch (type) {
			case ET_SIMPLE:
				_root = new SimpleExpression(*(SimpleExpression*)orig._root);
				break;
			case ET_UNARY:
				_root = new UnaryExpression(*(UnaryExpression*)orig._root);
				break;
			case ET_BINARY:
				_root = new BinaryExpression(*(BinaryExpression*)orig._root);
				break;
		}
	}
}

FilterParser::~FilterParser() {
	if (_root != NULL) delete _root;
	for (std::list<Token*>::iterator i = _tokens.begin(); i != _tokens.end(); i++) {
		Token* token = *i;
		delete token;
	}
	_tokens.clear();
}

ExpressionResult* FilterParser::eval(const BSONObj& bson) {
	ExpressionResult* result = NULL;
	if (_root != NULL) {
		result = _root->eval(bson);
	} else {
		bool bres = true;
		result = new ExpressionResult(bres);
	}

	return result;
}

Token::TOKEN_TYPE checkTokenType(const char* chrs, int& pos) {
	std::list<char*> matchOperators;
	matchOperators.push_back("(");
	matchOperators.push_back(")");
	matchOperators.push_back("==");
	matchOperators.push_back("and");
	matchOperators.push_back("or");
	matchOperators.push_back("<");
	matchOperators.push_back(">");
	matchOperators.push_back("!");

	char buffer[1024];
	memset(buffer, 0, 1024);
	int posBuffer = 0;

	Token::TOKEN_TYPE type = Token::TT_NOTTOKEN;

	while (true) {
		buffer[posBuffer] = chrs[pos];
		posBuffer++;
		std::list<char*> newmatch;
		for (std::list<char*>::iterator i = matchOperators.begin(); i != matchOperators.end(); i++) {
			char* oper = *i;
			if (strncmp(oper, buffer, posBuffer) == 0) {
				newmatch.push_back(oper);
			}
			if (strcmp(oper, buffer) == 0) {
				if (strcmp(buffer, "==") == 0) {
					type = Token::TT_EQUALS;
				} else if (strcmp(buffer, "and") == 0) {
					type = Token::TT_AND;
				} else if (strcmp(buffer, "or") == 0) {
					type = Token::TT_OR;
				} else if (strcmp(buffer, "(") == 0) {
					type = Token::TT_OPENPARENTESIS;
				} else if (strcmp(buffer, ")") == 0) {
					type = Token::TT_CLOSEPARENTESIS;
				} else if (strcmp(buffer, "<") == 0) {
					if (chrs[pos+1] == '=') {
						type = Token::TT_LESSEQUALTHAN;
						pos++;
					} else {
						type = Token::TT_LESSTHAN;
					}
				} else if (strcmp(buffer, ">") == 0) {
					if (chrs[pos+1] == '=') {
						type = Token::TT_GREATEREQUALTHAN;
						pos++;
					} else {
						type = Token::TT_GREATERTHAN;
					}
				}
				return type;
				// FOUND
			}
		}
		pos++;
		matchOperators.clear();
		matchOperators.insert(matchOperators.begin(), newmatch.begin(), newmatch.end());
		if (matchOperators.size() == 0) {
			break;
		}
	}
	return type;
}

BaseExpression* solveToken(Token* token) {
	BaseExpression* result = NULL;
	EXPRESSION_TYPE extype;
	FILTER_OPERATORS oper;
	switch (token->type()) {
		case Token::TT_EQUALS:
			extype = ET_BINARY;
			oper = FO_EQUALS;
			break;
		case Token::TT_AND:
			extype = ET_BINARY;
			oper = FO_AND;
			break;
		case Token::TT_LESSEQUALTHAN:
			extype = ET_BINARY;
			oper = FO_LESSEQUALTHAN;
			break;
		case Token::TT_LESSTHAN:
			extype = ET_BINARY;
			oper = FO_LESSTHAN;
			break;
		case Token::TT_GREATEREQUALTHAN:
			extype = ET_BINARY;
			oper = FO_GREATEREQUALTHAN;
			break;
		case Token::TT_GREATERTHAN:
			extype = ET_BINARY;
			oper = FO_GREATERTHAN;
			break;
		case Token::TT_OR:
			extype = ET_BINARY;
			oper = FO_OR;
			break;
		case Token::TT_CONSTANT:
			extype = ET_CONSTANT;
			break;
		case Token::TT_EXPRESION:
			extype = ET_SIMPLE;
			break;
	}
	switch (extype) {
		case ET_CONSTANT:
			result = new ConstantExpression(const_cast<char*>(token->content()->c_str()));
			break;
		case ET_SIMPLE: 
			result = new SimpleExpression(token->content()->c_str());
			break;
		case ET_BINARY:
			result = new BinaryExpression(oper);

			break;	
	}
	return result;	
}

BaseExpression* solveParentesis(std::list<Token*>& tokens, std::list<Token*>::iterator& i) {
	BaseExpression* expression;
	// Jumps the starting parentesis
	i++;
	//Token* currentToken = *i;
	//	if (currentToken->type() == Token::TT_OPENPARENTESIS) {
	//		expression = solveParentesis(tokens, i);
	//	} else {
	expression = solveExpression(tokens, i);
	i++;
	//	}
	return expression;
}

BaseExpression* solveExpression(std::list<Token*>& tokens, std::list<Token*>::iterator& i) {
	std::list<BaseExpression*> waitingList;
	if (tokens.size() == 0) {
		return NULL;
	}
	while (i != tokens.end()) {
		Token* token = *i;
		if (token->type() == Token::TT_CLOSEPARENTESIS) {
			break;
		}
		BaseExpression* tempExpression = NULL;

		BaseExpression* expression = NULL;
		if (token->type() == Token::TT_OPENPARENTESIS) {
			expression = solveParentesis(tokens, i);
		} else {
			expression = solveToken(token);
			switch(expression->type()) {
				case ET_CONSTANT:
				case ET_SIMPLE:
					i++;
					break;	  
				case ET_BINARY:
					((BinaryExpression*)expression)->push(waitingList.back());
					waitingList.pop_back();
					i++;
					((BinaryExpression*)expression)->push(solveExpression(tokens, i));
					break;
			}
		}
		waitingList.push_back(expression);

	}

	// the last token will not be processed, this could be the last parentesis
	// or the end token, it will be processed by the caller
	//	i--;
	if (waitingList.size() == 1) {
		return waitingList.back();
	} else {
		throw "Error parsing the expression";
	}
}

BaseExpression* createTree(std::list<Token*> tokens) {
	// process binary operators
	std::list<Token*>::iterator i = tokens.begin();
	BaseExpression* expression = solveExpression(tokens, i);

	return expression;// tokens.front();
}

void pushBuffer(std::list<Token*>& tokens, char* buffer, int& posBuffer) {
	if (strlen(buffer) > 0) {
		int tempPos = 0;
		Token::TOKEN_TYPE type = checkTokenType(buffer, tempPos);
		if (type == Token::TT_NOTTOKEN) {
			tokens.push_back(new Token(Token::TT_CONSTANT, buffer));
			memset(buffer, 0, BUFFER_SIZE);
			posBuffer = 0;
		} else {
			tokens.push_back(new Token(type, buffer));
		}
	}
}
const std::set<std::string> FilterParser::tokens() const {
	return _xpathTokens;
}
void FilterParser::setTokens(std::set<std::string> tokens) {
	_xpathTokens = tokens;
}

// static
FilterParser* FilterParser::parse(const std::string& expression) throw(ParseException) {
	Logger* log = getLogger(NULL);
	BaseExpression* rootExpression = NULL;
	std::list<Token*> lTokens;
	std::set<std::string> xpathTokens;

	int errorCode = -1;
	const char* errorMessage;
	if (expression.length() != 0) {
		//throw (ParseException) {
		pANTLR3_INPUT_STREAM           input;
		pfilter_expressionLexer               lex;
		pANTLR3_COMMON_TOKEN_STREAM    tokens;
		pfilter_expressionParser              parser;

		const char* filter = expression.c_str();
		if (log->isDebug()) log->debug("filter expression: %s, len: %d, Size Hint: %d", filter, strlen(filter), ANTLR3_SIZE_HINT);
		input  = antlr3StringStreamNew((pANTLR3_UINT8)filter, 8, (ANTLR3_UINT32)strlen(filter), (pANTLR3_UINT8)"name");
		lex    = filter_expressionLexerNew                (input);
		tokens = antlr3CommonTokenStreamSourceNew  (ANTLR3_SIZE_HINT, TOKENSOURCE(lex));
		parser = filter_expressionParserNew               (tokens);

		rootExpression = parser ->start_point(parser);
		xpathTokens = __parser_tokens();
		if (parser->pParser->rec->state->exception != NULL) {
			errorCode = D_ERROR_PARSEERROR;
			errorMessage = (char*)parser->pParser->rec->state->exception->message;
		}

		// Must manually clean up
		//
		parser ->free(parser);
		tokens ->free(tokens);
		lex    ->free(lex);
		input  ->close(input);
	}
	if (errorCode > -1) {
		throw ParseException(errorCode, errorMessage);
	}
	FilterParser* filterparser = new FilterParser(expression, rootExpression, lTokens);
	filterparser->setTokens(xpathTokens);

	return filterparser;
	}

	std::set<std::string> FilterParser::xpathTokens() {
		return _xpathTokens;
	}



