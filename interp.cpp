
#include "parserInterp.h"

map<string, bool> defVar;
map<string, Token> type;
map<string, Value> vals; //Container of temporary locations of Value objects for results of expressions, variables values and constants 
queue <Value> * ValQue; //declare a pointer variable to a queue of Value objects

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if( pushed_back ) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem & t) {
		if( pushed_back ) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;	
	}

}

static int error_count = 0;

int ErrCount()
{
    return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}

//Program is: Prog = PROGRAM IDENT CompStmt
bool Prog(istream& in, int& line)
{
	bool sl = false;
	LexItem tok = Parser::GetNextToken(in, line);
		
	if (tok.GetToken() == PROGRAM) {
		tok = Parser::GetNextToken(in, line);
		if (tok.GetToken() == IDENT) {
			sl = CompStmt(in, line);
			if( !sl  )
			{
				ParseError(line, "Invalid Program");
				return false;
			}
			cout << "(DONE)" << endl;
			return true;
		}
		else
		{
			ParseError(line, "Missing Program name");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	
	else
	{
		ParseError(line, "Missing Program keyword");
		return false;
	}
}//End of Prog

//StmtList ::= Stmt { Stmt }
bool StmtList(istream& in, int& line)
{
	bool status;
	LexItem tok;
	
	status = Stmt(in, line);
	tok = Parser::GetNextToken(in, line);
	while(status && tok != RBRACE)
	{
		Parser::PushBackToken(tok);
		
		status = Stmt(in, line);
		tok = Parser::GetNextToken(in, line);
	}
	if(!status)
	{
		ParseError(line, "Syntactic error in statement list.");
			
		return false;
	}
	Parser::PushBackToken(tok);
	return true;
}//End of StmtList

//DeclStmt ::= ( INT | FLOAT | BOOL | CHAR | STRING) VarList 
bool DeclStmt(istream& in, int& line) {
	bool status = false;
	LexItem tok;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	if(t == INT || t == FLOAT || t == CHAR || t == BOOL || t == STRING ) {
		// pass the type as idtok to VarList
		status = VarList(in, line, t);
		
		if (!status)
		{
			ParseError(line, "Incorrect variable in Declaration Statement.");
			return status;
		}
		return true;	
	}
	
	Parser::PushBackToken(t);
	return false;
}//End of Decl

//Stmt ::= DeclStmt ; | ControlStmt ; | CompStmt
bool Stmt(istream& in, int& line) {
	bool status = true;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case PRINT: case IF: case IDENT:
		Parser::PushBackToken(t);
		status = ControlStmt(in, line);
		
		if(!status)
		{
			ParseError(line, "Invalid control statement.");
			return false;
		}
		
		break;
	case INT: case FLOAT: case CHAR: case BOOL: case STRING:
		Parser::PushBackToken(t);
		status = DeclStmt(in, line);
		if(!status)
		{
			ParseError(line, "Invalid declaration statement.");
			return false;
		}
		t = Parser::GetNextToken(in, line);
		if(t == SEMICOL)	
		{
			return true;
		}
		else
		{
			ParseError(line, "Missing semicolon at end of Statement.");
			return false;
		}	
		break;
	case LBRACE:
		Parser::PushBackToken(t);
		status = CompStmt(in, line);
		if(!status)
		{
			ParseError(line, "Invalid compound statement.");
			return false;
		}
			
		break;
	default:
		Parser::PushBackToken(t);
		
		return false;
	}

	return status;
}//End of Stmt

bool CompStmt(istream& in, int& line)
{
	bool status=false;
	
	LexItem t = Parser::GetNextToken(in, line);
	if(t == LBRACE)
	{
		status = StmtList(in, line);
		if(!status)
		{
			ParseError(line, "Incorrect statement list");
			return false;
		}
		LexItem t = Parser::GetNextToken(in, line);
		if(t == RBRACE)
		{
			return true;
		}
		else if(t == DONE)
		{
			ParseError(line, "Missing end of program right brace.");
			return false;
		}
		else
		{
			ParseError(line, "Missing right brace.");
			return false;
		}
	}
	return status;
}//End of CompStmt

bool ControlStmt(istream& in, int& line) {
	bool status;
	
	LexItem t = Parser::GetNextToken(in, line);
	
	switch( t.GetToken() ) {

	case PRINT:
		status = PrintStmt(in, line);
		
		if(!status)
		{
			ParseError(line, "Incorrect Print Statement");
			return false;
		}	
		t = Parser::GetNextToken(in, line);
		
		if(t == SEMICOL)	
		{
			return true;
		}
		else
		{
			ParseError(line, "Missing semicolon at end of Statement.");
			return false;
		}
		
		break;

	case IDENT:
		Parser::PushBackToken(t);
        status = AssignStmt(in, line);
		if(!status)
		{
			ParseError(line, "Incorrect Assignment Statement");
			return false;
		}
		t = Parser::GetNextToken(in, line);
		
		if(t == SEMICOL)	
		{
			return true;
		}
		else
		{
			ParseError(line, "Missing semicolon at end of Statement.");
			return false;
		}
		
		break;
	case IF:
		status = IfStmt(in, line);
		
		if(!status)
		{
			ParseError(line, "Incorrect IF Statement");
			return false;
		}	
		
		break;
	
	default:
		Parser::PushBackToken(t);
		return true;
	}

	return status;
}//End of ControlStmt

//VarList ::= Var [= Expr] {, Var [= Expr]}
bool VarList(istream& in, int& line, LexItem & idtok) {
	// idtok is the type of all of the variables in this varlist
	bool status = false, exprstatus = false;
	string identstr;

	LexItem tok = Parser::GetNextToken(in, line);
	// next token is a variable
	if(tok == IDENT) {
		// get name of variable
		identstr = tok.GetLexeme();
		// if variable not defined yet, set to true
		if (!(defVar.find(identstr)->second))
		{
			defVar[identstr] = true;
			type[identstr] = idtok.GetToken(); // record the type of this variable
		}
		else
		{
			ParseError(line, "Variable Redefinition");
			return false;
		}
		
	}
	else
	{
	
		ParseError(line, "Missing Variable Name");
		return false;
	}
		
	tok = Parser::GetNextToken(in, line);
	if(tok == ASSOP)
	{
		// need to make sure that the Expression matches the type idtok
		Value retval;
		exprstatus = Expr(in, line, retval);
		if(!exprstatus)
		{
			ParseError(line, "Incorrect initialization for a variable.");
			return false;
		}
		
		switch(idtok.GetToken()) {
			case INT:
				if (retval.IsReal()) {
					retval = Value((int)retval.GetReal());
				}
				else if (!retval.IsInt()) {
					ParseError(line, "Type mismatch in INT declaration.");
					return false;
				}
				break;
			case FLOAT:
				if (retval.IsInt()) {
					retval = Value((float)retval.GetInt());
				}
				else if (!retval.IsReal()) {
					ParseError(line, "Type mismatch in FLOAT declaration.");
					return false;
				}
				break;
			case CHAR:
				if (retval.IsInt()) {
					retval = Value((char)retval.GetInt());
				}
				else if (!retval.IsChar()) {
					ParseError(line, "Type mismatch in CHAR declaration.");
					return false;
				}
				break;
			case STRING:
				if (retval.IsChar()) {
					retval = Value(retval.GetChar() + "");
				}
				else if (!retval.IsString()) {
					ParseError(line, "Type mismatch in STRING declaration.");
					return false;
				}
				break;
			case BOOL:
				if (!retval.IsBool()) {
					ParseError(line, "Type mismatch in BOOL declaration.");
					return false;
				}
				break;
			default:
				break;
		}
		// type conversions and checking
		
		//cout<< "Initialization of the variable " << identstr <<" in the declaration statement at line " << line << endl;
		// initialize
		//cout << "retval is " << retval << endl;
		type[identstr] = idtok.GetToken();
		vals[identstr] = retval;

		//cout << "vals[" << identstr << "] " << vals[identstr] << endl;
		//cout << "type[" << identstr << "] " << type[identstr] << endl;

		
		tok = Parser::GetNextToken(in, line);
		
		if (tok == COMMA) {
			
			status = VarList(in, line, idtok);
		
		}
		else
		{
			Parser::PushBackToken(tok);
			return true;
		}
	}
	else if (tok == COMMA) {
		
		status = VarList(in, line, idtok);
	}
	else if(tok == ERR)
	{
		ParseError(line, "Unrecognized Input Pattern");
		
		return false;
	}
	else {
	
		Parser::PushBackToken(tok);
		return true;
	}
	
	return status;
	
}//End of VarList

//PrintStmt:= PRINT (ExpreList) 
bool PrintStmt(istream& in, int& line) {
	LexItem t;
	ValQue = new queue<Value>;

	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	bool ex = ExprList(in, line);
	
	if( !ex ) {
		ParseError(line, "Missing expression list after Print");
		while (!(*ValQue).empty())
		{
			ValQue->pop();
		}
		delete ValQue;
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
		
		ParseError(line, "Missing Right Parenthesis");
		while (!(*ValQue).empty())
		{
			ValQue->pop();
		}
		delete ValQue;
		return false;
	}
	//Evaluate: print out the list of expressions' values
	while (!(*ValQue).empty())
	{
		Value nextVal = (*ValQue).front();
		cout << nextVal;
		ValQue->pop();
	}
	delete ValQue;
	cout << endl;
	return true;
}//End of PrintStmt

bool IfStmt(istream& in, int& line) {
	bool ex=false, status;
	static int nestlevel = 0;
	LexItem t;
	
	t = Parser::GetNextToken(in, line);
	if( t != LPAREN ) {
		ParseError(line, "Missing Left Parenthesis");
		return false;
	}
	
	Value retval;
	ex = Expr(in, line, retval);

	if( !ex ) {
		ParseError(line, "Missing if statement condition");
		return false;
	}
	if (!retval.IsBool()) {
		ParseError(line, "Invalid type for If statement condition.");
		return false;
	}
	
	t = Parser::GetNextToken(in, line);
	if(t != RPAREN ) {
	
		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	nestlevel +=1;
	//cout << "in IfStmt then-clause at nesting level: " << nestlevel << endl;
	
	// Only execute THEN clause when the condition is true
	Token skip;
	if (retval.GetBool()) {
		//cout << "If condition true." << endl;
		// execute then clause
		status = Stmt(in, line);
		
		if(!status) {
			ParseError(line, "If-Stmt Syntax Error");
			return false;
		}

		// look ahead
		t = Parser::GetNextToken(in, line);

		// there is an ELSE clause
		if (t == ELSE) {
			t = Parser::GetNextToken(in, line);

			// if next token is {, need to skip CompStmt
			if (t == LBRACE) {
				skip = RBRACE;
			}
			// else skipping a single statement
			else {
				skip = SEMICOL;
			}
			
			while (t != skip) {
				t = Parser::GetNextToken(in, line);
				if (t == IF) {
					Token skip2;
					// Nested If statement: parse like normal
					while (t != RPAREN) {
						t = Parser::GetNextToken(in, line);
					}
					t = Parser::GetNextToken(in, line);
					if (t == LBRACE) {
						skip2 = RBRACE;
					}
					else {
						skip2 = SEMICOL;
					}

					while (t != skip2) {
						t = Parser::GetNextToken(in, line);
					}
					t = Parser::GetNextToken(in, line);

					if (t == ELSE) {
						t = Parser::GetNextToken(in, line);
						
						if (t == LBRACE) {
							skip2 = RBRACE;
						}
						else {
							skip2 = SEMICOL;
						}

						while (t != skip2) {
							t = Parser::GetNextToken(in, line);
						}
						t = Parser::GetNextToken(in, line);
					}
					else {
						Parser::PushBackToken(t);
					}
				}
			}
		}
		else {
			// no ELSE clause, continue
			Parser::PushBackToken(t);
			return true;
		}
	}
	// condition is false, skip THEN clause
	else {
		//cout << "If condition false." << endl;
		// peek ahead
		t = Parser::GetNextToken(in, line);

		// { => need to skip CompStmt
		if (t == LBRACE) {
			skip = RBRACE;
		}
		// otherwise need to skip a single statement
		else {
			skip = SEMICOL;
		}

		while (t != skip) {
			t = Parser::GetNextToken(in, line);
			if (t == IF) {
					Token skip2;
					// Nested If statement: parse like normal
					while (t != RPAREN) {
						t = Parser::GetNextToken(in, line);
					}
					t = Parser::GetNextToken(in, line);
					if (t == LBRACE) {
						skip2 = RBRACE;
					}
					else {
						skip2 = SEMICOL;
					}

					while (t != skip2) {
						t = Parser::GetNextToken(in, line);
					}
					t = Parser::GetNextToken(in, line);

					if (t == ELSE) {
						t = Parser::GetNextToken(in, line);
						
						if (t == LBRACE) {
							skip2 = RBRACE;
						}
						else {
							skip2 = SEMICOL;
						}

						while (t != skip2) {
							t = Parser::GetNextToken(in, line);
						}
						t = Parser::GetNextToken(in, line);
					}
					else {
						Parser::PushBackToken(t);
					}
				}
		}

		// get next tok after } or ;
		t = Parser::GetNextToken(in, line);

		// check if there is an ELSE clause
		if (t == ELSE) {
			status = Stmt(in, line);

			if(!status)
			{
				ParseError(line, "Missing Statement for If-Stmt Else-Part");
				return false;
			}
			
			nestlevel--;
			return true;
		}
		else {
			Parser::PushBackToken(t);
			return true;
		}
	}

	//Parser::PushBackToken(t);
	return true;
}//End of IfStmt function

//Var:= ident
bool Var(istream& in, int& line, LexItem & idtok) {
	string identstr;
	
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == IDENT){
		identstr = tok.GetLexeme();
		
		// variable not declared
		if (!(defVar.find(identstr)->second))
		{
			ParseError(line, "Undeclared Variable");
			return false;
		}
		// get the type of this variable
		// Token = type (not IDENT), lexeme = name of var
		idtok = LexItem(type[tok.GetLexeme()], tok.GetLexeme(), line);
		return true;
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	return false;
}//End of Var

//AssgnStmt ::= Var ( = | += | -= | *= | /= | %= ) Expr
bool AssignStmt(istream& in, int& line) {
	//cout << "Begin assignment." << endl;
	
	bool varstatus = false, status = false;
	LexItem op;
	
	// var:
	// lexeme is the name of the variable (the ident)
	// token is the type (found using lookup w/ the lexeme)
	LexItem var;
	varstatus = Var(in, line, var);
	//cout << "Assignment statement. lexeme is " << var.GetLexeme() << endl;
	
	if (varstatus){
		op = Parser::GetNextToken(in, line);
		
		if (op == ASSOP || op == ADDASSOP || op == SUBASSOP || op == MULASSOP || op == DIVASSOP || op == REMASSOP)
		{
			// operation is a combined assignment, variable must be initialized first
			if (op != ASSOP) {
				if (vals.count(var.GetLexeme()) == 0) {
					ParseError(line, "Invalid assignment, variable does not have a value.");
					return false;
				}
			}

			// evaluate the expression
			Value ret;
			//cout << "Evaluating expression." << endl;
			status = Expr(in, line, ret);
			//cout << "Finished evaluating." << endl;
			if(!status) {
				ParseError(line, "Missing Expression in Assignment Statement");
				return status;
			}

			// check matching types
			// var's token is one of INT, FLOAT, etc.
			//cout << "Var= " << var << endl;
			//cout << var.GetLexeme() << endl;
			switch(var.GetToken()) {
				case INT:
					if (ret.IsReal()) {
						// convert expr value to an int
						ret = Value((int)ret.GetReal());
					}
					else if (!ret.IsInt()) {
						ParseError(line, "Illegal mixed-mode assignment operation.");
						return false;
					}
					break;
				case FLOAT:
					if (ret.IsInt()) {
						ret = Value((float)ret.GetInt());
					}
					else if (!ret.IsReal()) {
						ParseError(line, "Illegal mixed-mode assignment operation.");
						return false;
					}
					break;
				case CHAR:
					if (ret.IsInt()) {
						// THIS IS WHERE CHAR* EXCEPTION HAPPENED
						//ret = Value((char)ret.GetInt());
					}
					else if (!ret.IsChar()) {	
						ParseError(line, "Illegal mixed-mode assignment operation.");
						return false;
					}
					break;
				case STRING:
					if (ret.IsChar()) {
						ret = Value(ret.GetChar() + "");
					}
					else if (!ret.IsString()) {
						ParseError(line, "Illegal mixed-mode assignment operation.");
						return false;
					}
					break;
				case BOOL:
					if (!ret.IsBool()) {
						ParseError(line, "Illegal mixed-mode assignment operation.");
						return false;
					}
					break;
				default:
					cout << "Assignment default case. Should not execute." << endl;
					return false;
					break; 
			}

			// initialize the variable using the value from the expression
			//vals[var.GetLexeme()] = ret;

			// implement mixed-assignment here
			switch (op.GetToken()) {
				case ASSOP:
					// normal assignment
					vals[var.GetLexeme()] = ret;
					break;
				case ADDASSOP:
					// variable += expr value
					vals[var.GetLexeme()] = vals[var.GetLexeme()] + ret;
					break;
				case SUBASSOP:
					vals[var.GetLexeme()] = vals[var.GetLexeme()] - ret;
					break;
				case MULASSOP:
					vals[var.GetLexeme()] = vals[var.GetLexeme()] * ret;
					break;
				case DIVASSOP:
					vals[var.GetLexeme()] = vals[var.GetLexeme()] / ret;
					break;
				case REMASSOP:
					vals[var.GetLexeme()] = vals[var.GetLexeme()] % ret;
					break;
				default:
					return false;
					break;
			}


			return true;
		}
		else if(op.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << op.GetLexeme() << ")" << endl;
			return false;
		}
		else {
			ParseError(line, "Missing Assignment Operator");
			return false;
		}
	}
	else {
		ParseError(line, "Missing Left-Hand Side Variable in Assignment statement");
		return false;
	}
	return status;	
}//End of AssignStmt

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	Value retVal;
	
	status = Expr(in, line, retVal);
	if(!status){
		ParseError(line, "Missing Expression");
		return false;
	}
	ValQue->push(retVal);
	LexItem tok = Parser::GetNextToken(in, line);
	
	if (tok == COMMA) {
		
		status = ExprList(in, line);
		
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else{
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}//End of ExprList

//Expr ::= LogANDExpr { || LogANDRxpr }
bool Expr(istream& in, int& line, Value & retVal) {
	
	LexItem tok;

	bool t1 = LogANDExpr(in, line, retVal);
		
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == OR ) 
	{
		Value nextval;
		t1 = LogANDExpr(in, line, nextval);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}

		retVal = retVal || nextval;
		if (retVal.IsErr()) {
			ParseError(line, "Illegal operand types for the operation.");
			return false;
		}
	}
	Parser::PushBackToken(tok);
	return true;
}//End of Expr/LogORExpr

//LogANDExpr ::= EqualExpr { && EqualExpr }
bool LogANDExpr(istream& in, int& line, Value & retVal) {
	LexItem tok;
	bool t1 = EqualExpr(in, line, retVal);
		
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == AND ) 
	{
		Value nextval;
		t1 = EqualExpr(in, line, nextval);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
		retVal = retVal && nextval;
		if (retVal.IsErr()) {
			ParseError(line, "Illegal operand type for logical AND operator.");
			return false;
		}
	}
	
	Parser::PushBackToken(tok);
	return true;
}//End of LogANDExpr

//EqualExpr ::= RelExpr [ (== | !=) RelExpr ]
bool EqualExpr(istream& in, int& line, Value & retVal) {
	LexItem tok;
	bool t1 = RelExpr(in, line, retVal);
		
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == EQ || tok == NEQ ) 
	{
		Value nextval;
		t1 = RelExpr(in, line, nextval);
	
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}
		
		// update retval
		if (tok == EQ) {
			retVal = retVal == nextval;
		}
		else {
			retVal = retVal != nextval;
		}
		if (retVal.IsErr()) {
			ParseError(line, "Illegal operand type for the operation.");
			return false;
		}
		tok = Parser::GetNextToken(in, line);
		if(tok == EQ || tok == NEQ)
		{
			ParseError(line, "Illegal Equality Expression.");
			return false;
		}
		else if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
	}
	Parser::PushBackToken(tok);
	
	return true;
}//End of EqualExpr

//RelExpr ::= AddExpr [ ( < | > ) AddExpr ]
bool RelExpr(istream& in, int& line, Value & retVal) {
	LexItem tok;
	bool t1 = AddExpr(in, line, retVal);
		
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	if ( tok == LTHAN || tok == GTHAN) 
	{
		Value nextval;
		t1 = AddExpr(in, line, nextval);

		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (tok == LTHAN) {
			retVal = retVal < nextval;
		}
		else {
			retVal = retVal > nextval;
		}

		if (retVal.IsErr()) {
			ParseError(line, "Illegal operands for relational operator.");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		
		if(tok == LTHAN || tok == GTHAN)
		{
			ParseError(line, "Illegal Relational Expression.");
			return false;
		}
		else if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of RelExpr

//AddExpr :: MultExpr { ( + | - ) MultExpr }
bool AddExpr(istream& in, int& line, Value & retVal) {

	bool t1 = MultExpr(in, line, retVal);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	
	tok = Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	while ( tok == PLUS || tok == MINUS ) 
	{
		Value nextval;
		t1 = MultExpr(in, line, nextval);
		if( !t1 ) 
		{
			ParseError(line, "Missing operand after operator");
			return false;
		}

		// update retVal
		if (tok == PLUS) {
			retVal = retVal + nextval;
		}
		else {
			retVal = retVal - nextval;
		}

		if (retVal.IsErr()) {
			ParseError(line, "Illegal operand type for the operation.");
			return false;
		}
		
		tok = Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}		
		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of AddExpr

//MultExpr ::= UnaryExpr { ( * | / | %) UnaryExpr }
bool MultExpr(istream& in, int& line, Value & retVal) {
	
	bool t1 = UnaryExpr(in, line, retVal);
	LexItem tok;
	
	if( !t1 ) {
		return false;
	}
	
	tok	= Parser::GetNextToken(in, line);
	if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
	}
	while ( tok == MULT || tok == DIV  || tok == REM)
	{
		/*
		if (retVal.IsString()) {
			ParseError(line, "Illegal operand type for the operation.");
			return false;
		}
		*/
		Value nextval;
		t1 = UnaryExpr(in, line, nextval);
	
		if( !t1 ) {
			ParseError(line, "Missing operand after operator");
			return false;
		}

		if (tok == MULT) {
			retVal = retVal * nextval;
		}
		else if (tok == DIV) {
			// check for div by 0
			// not handled by checking IsErr b/c division by 0.0f is defined (inf)
			if (nextval.IsInt()) {
				if (nextval.GetInt() == 0) {
					ParseError(line, "Run-Time Error-Illegal division by 0");
					return false;
				}
			}
			else if (nextval.IsReal()) {
				if (nextval.GetReal() == 0.0) {
					ParseError(line, "Run-Time Error-Illegal division by 0.0");
					return false;
				}
			}
			
			retVal = retVal / nextval;
		}
		else {
			retVal = retVal % nextval;
		}

		if (retVal.IsErr()) {
			ParseError(line, "Illegal operand type for the operation.");
			return false;
		}
		
		tok	= Parser::GetNextToken(in, line);
		if(tok.GetToken() == ERR){
			ParseError(line, "Unrecognized Input Pattern");
			cout << "(" << tok.GetLexeme() << ")" << endl;
			return false;
		}
		
	}
	Parser::PushBackToken(tok);
	return true;
}//End of MultExpr

//UnaryExpr ::= ( - | + | ! ) PrimaryExpr | PrimaryExpr
bool UnaryExpr(istream& in, int& line, Value & retVal){
	LexItem t = Parser::GetNextToken(in, line);

	bool status;
	int sign = 0;
	if(t == MINUS )
	{
		sign = -1;
	}
	else if(t == PLUS)
	{
		sign = 1;
	}
	else if (t == NOT)
	{
		sign = 2;
	}
	else
		Parser::PushBackToken(t);
		
	//cout << "Unary, sign is " << sign << endl;
	status = PrimaryExpr(in, line, sign, retVal);
	return status;
}//End of UnaryExpr



//PrimaryExpr ::= IDENT | ICONST | RCONST | SCONST | BCONST | CCONST | ( Expr )
bool PrimaryExpr(istream& in, int& line, int sign, Value & retVal) {
	//cout << "Primary, sign is " << sign << endl;
	LexItem tok = Parser::GetNextToken(in, line);

	//cout << "tok= " << tok;
	
	// If this is an ident:
	// Check if variable declared
	// Check if variable initialized
	// Check if compatible type
	if( tok == IDENT ) {

		string lexeme = tok.GetLexeme();
		//cout << "lexeme= " << lexeme << endl;
		if (!(defVar.find(lexeme)->second)) {
			ParseError(line, "Using Undeclared Variable");
			return false;	
		}
		else if (vals.count(lexeme) == 0) {
			// Variable does not have a value => uninitialized
			ParseError(line, "Using uninitialized variable.");
			return false;
		}

		//cout << "type[" << lexeme << "]= " << type[lexeme] << endl;
		
		// Check for compatible type
		switch(type[lexeme]) {
			case INT:
				// ! does not apply to INT
				if (sign == 2) {
					ParseError(line, "Illegal Operand Type for Sign Operator");
					return false;
				}

				retVal = vals[lexeme];

				// negate if sign is -
				if (sign == -1) {
					retVal = retVal * sign;
				}
				return true;
				break;
			case FLOAT:
				if (sign == 2) {
					ParseError(line, "Illegal Operand Type for Sign Operator");
					return false;
				}

				retVal = vals[lexeme];

				// negate if sign is -
				if (sign == -1) {
					retVal = retVal * sign;
				}
				return true;
				break;
			case CHAR:
				if (sign != 0) {
					ParseError(line, "Illegal Operand Type for Sign Operator");
					return false;
				}
				retVal = vals[lexeme];
				return true;
			case STRING:
				if (sign != 0) {
					ParseError(line, "Illegal Operand Type for Sign Operator");
					return false;
				}
				retVal = vals[lexeme];
				return true;
			case BOOL:
				if (sign == 1 || sign == -1) {
					ParseError(line, "Illegal Operand Type for Sign Operator");
					return false;
				}

				if (sign == 2) {
					retVal = Value(!vals[lexeme]);
				}
				else {
					retVal = vals[lexeme];
				}
				return true;
				break;
			default:
				break;
		}
		cout << "should not execute." << endl;
		return true;
	}
	// Literals
	else if (ICONST <= tok.GetToken() && tok.GetToken() <= CCONST) {
		string lexeme = tok.GetLexeme();
		bool val;
		switch(tok.GetToken()) {
			// int literal
			case ICONST:
				if (sign == 2) {
					ParseError(line, "Illegal Operand Type for Sign Operator");
					return false;
				}

				// convert the lexeme to an integer
				retVal = Value(std::stoi(lexeme));

				// negate if sign is -
				if (sign == -1) {
					retVal = retVal * sign;
				}
				return true;
				break;
			// float literal
			case RCONST:
				if (sign == 2) {
					ParseError(line, "Illegal Operand Type for Sign Operator");
					return false;
				}

				// convert the lexeme to a float
				retVal = Value(std::stof(lexeme));

				// negate if sign is -
				if (sign == -1) {
					retVal = retVal * sign;
				}
				return true;
				break;
			// string literal
			case SCONST:
				// cannot apply +, -, or ! to a string
				if (sign != 0) {
					ParseError(line, "Illegal Operand Type for Sign Operator");
					return false;
				}
				retVal = Value(lexeme);
				return true;
				break;
			case CCONST:
				if (sign != 0) {
					ParseError(line, "Illegal Operand Type for Sign Operator");
					return false;
				}
				retVal = Value(lexeme[0]);
				return true;
				break;
			case BCONST:
				// +, - do not apply to boolean
				if (sign == 1 || sign == -1) {
					ParseError(line, "Illegal Operand Type for Sign Operator");
					return false;
				}

				// determine boolean value based on lexeme
				if (lexeme == "true") {
					val = true;
				}
				else {
					val = false;
				}

				// negate if sign is !
				if (sign == 2) {
					retVal = Value(!val);
				}
				else {
					retVal = Value(val);
				}
				return true;
				break;
			default:
				break;
		}
	}
	else if( tok == ICONST ) {
		if (sign == 2) {
			ParseError(line, "Illegal Operand Type for NOT operator.");
			return false;
		}

		retVal = Value(std::stoi(tok.GetLexeme())) * sign;
		return true;
	}
	else if( tok == SCONST ) {
		if (sign == -1) {
			ParseError(line, "Illegal Operand Type for Sign operator.");
			return false;
		}
		else if (sign == 2) {
			ParseError(line, "Illegal Operand Type for NOT operator.");
			return false;
		}

		retVal = Value(tok.GetLexeme());
		return true;
	}
	else if( tok == RCONST ) {

		if (sign == 2) {
			ParseError(line, "Illegal Operand Type for NOT operator.");
			return false;
		}

		retVal = Value(std::stof(tok.GetLexeme())) * sign;
		return true;
	}
	else if( tok == BCONST ) {
		if (sign == 1 || sign == -1) {
			ParseError(line, "Illegal Operand for Sign operator.");
			return false;
		}

		if (tok.GetLexeme() == "true") {
			retVal = true;
		}
		else {
			retVal = false;
		}
		return true;
	}
	else if ( tok == CCONST )
	{
		retVal = Value(tok.GetLexeme()[0]);
		return true;
	}
	else if( tok == LPAREN ) {
		bool ex = Expr(in, line, retVal);
		if( !ex ) {
			ParseError(line, "Missing expression after Left Parenthesis");
			return false;
		}
		if( Parser::GetNextToken(in, line) == RPAREN )
			return ex;
		else 
		{
			Parser::PushBackToken(tok);
			ParseError(line, "Missing right Parenthesis after expression");
			return false;
		}
	}
	else if(tok.GetToken() == ERR){
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}

	return false;
}//End of PrimaryExpr