/*
 * scheme.hpp
 *
 *  Created on: 29.03.2017
 *      Author: joscha
 */

#ifndef INTERPRETER_HPP_
#define INTERPRETER_HPP_

//Utility functions

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <fstream>      // std::filebuf

using namespace std;
//Class to store and process an s-expression
//internally this is a vector of strings
class PList {
	vector<string> store;

public:
	PList();

	//Init a PList from a vector of strings
	PList(vector<string>&vv);

	//Init a PList from a string
	PList(string &ss);

	//Print the content of an s-expression
	void print();

	//Convert the contents to a single string
	string toString();

	//Get a copy of the vector uses to store the PList
	vector<string> get_store();

	//Clear the contents
	void clear();

	//Get the number of s-expressions contained on a level
	//for example:
	//(+ aa bb) will return 3
	//(+ (- 2 3)) will return 2
	size_t size();

	//Get the sub PList stored at pos
	//e.g. (+ (- 2 3) (* 1 1)) get(0) will return "+"
	PList get(size_t pos);

	//Get the full size of vector used to store the PList
	size_t full_size();

	//Get the elem at pos from the vector used to store the PList
	string elem(size_t pos);

	//Add a string at the end of the current vector
	void puts(string ss);
};

//Class used to store a Scheme defined procedure
//the procedure is stored in two PLists: arguments and body
class Procedure {
	PList arg_list;
	PList body;

	//Add a PList to the end of the vector V
	void addPList(vector<string>&V, PList a);

public:
	Procedure();

	//Init a procedure from a PList
	Procedure(PList &pp);

	//Get the list of arguments of the current procedure
	PList get_arg_list();

	//Get the body of the current procedure
	PList get_body();
};
//Simple class to store a string or a procedure
class Cell {
	string st;
	Procedure proc;

public:
	Cell();

	//Init the Cell with a string
	Cell(string aa);

	//Init the Cell with a procedure
	Cell(Procedure &procedure);

	//Get the values stored in a Cell (string or procedure)
	string get_str();
	Procedure get_proc();
};
//Class to store a native defined procedures (C++), Scheme defined procedures and strings

class Object {
	string (*pp)(vector<string>&);
	Procedure proc;
	string value;
	string kind;
	bool native_proc;

public:
	Object();

	//Store a string
	Object(string &ss);

	//Store a pointer to a C++ function (this can be accessed from Scheme)
	Object(string (*p_)(vector<string>&));

	//Store a Scheme defined procedure
	Object(Procedure &procedure);

	//Get the type of data stored - variable or procedure
	string get_kind();

	//Get the value stored
	string get_value();

	//Return true if the procedure is native (C++) and false if it was defined in Scheme
	bool check_native_procedure();

	//Run the current procedure with the arguments V
	string apply(vector<string>&V);

	//Run the current procedure with no arguments
	string apply();

	//Get the list of arguments of the current procedure
	PList get_arg_list();

	//Get the body of the current procedure
	PList get_body();

	//Get the Scheme procedure
	Procedure get_proc();
};

//Basic procedures:
//Each procedure receives as input a vector of strings and returns a string

//Print one or more variables; e.g. (print aa), (print aa bb cc)
string print(vector<string>&vv);

//Add one or more variables; e.g. (+ aa), (+ aa bb cc)
string add(vector<string>&vv);

//Diff, e.g.: (- aa bb)
string diff(vector<string>&vv);

//Division, e.g. (/ aa bb cc ...)
string divv(vector<string>&vv);

//Product, e.g. (* aa bb cc ...)
string prod(vector<string>&vv);

//Quit the interpreter, e.q. (quit) or (exit)
string scheme_quit(vector<string>&vv);

//Logical operators: <, <=, >, >=, =, !=

string less_than(vector<string>&vv);

string less_or_equal(vector<string>&vv);

string great_than(vector<string>&vv);

string great_or_equal(vector<string>&vv);

string equall(vector<string>&vv);

string not_equal(vector<string>&vv);
//Utility functions

//Print the Scheme prompt
void prompt();

//Read the user input character by character in a STL string.
string get_input();

//START_NOTE
//"replace_substr" and "clean_input" can be replaced by a more elegant regex solution
//unfortunateley gcc does not fully support regex (at the time of this implementation)
//Visual Studio 2010 fully supports regex
//END_NOTE

//Replace substring target with substring res in string inp
void replace_substr(string &inp, string target, string res);

//Split the input string in components and store them in a STL vector of strings
vector<string> clean_input(string &str);

//START_NOTE
//This ugly hack should be replaced with a regex that can decide if we have int,double or complex numbers
//END_NOTE
//Check if the string contains a number
bool number(string inp);
//Simple class to store a string or a procedure

void execute(const char *);

#endif /* INTERPRETER_HPP_ */
