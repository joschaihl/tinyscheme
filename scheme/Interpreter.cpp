/*
 * Interpreter.cpp
 *
 *  Created on: 29.03.2017
 *      Author: joscha
 */

#ifndef INTERPRETER_CPP_
#define INTERPRETER_CPP_

#include "Interpreter.hpp"


Cell::Cell() {
}

//Init the Cell with a string
Cell::Cell(string aa) {
	st = aa;
}

//Init the Cell with a procedure
Cell::Cell(Procedure &procedure) {
	proc = procedure;
	st = "";
}

//Get the values stored in a Cell (string or procedure)

string Cell::get_str() {
	return st;
}

Procedure Cell::get_proc() {
	return proc;
}
//Class to store a native defined procedures (C++), Scheme defined procedures and strings

Object::Object(): pp(NULL), value(""), native_proc(false){

}

//Store a string
Object::Object(string &ss) {
	value = ss;
	kind = "variable";
	pp = NULL;
	native_proc = false;
}

//Store a pointer to a C++ function (this can be accessed from Scheme)
Object::Object(string (*p_)(vector<string>&)) {
	pp = p_;
	kind = "procedure";
	value = "";
	native_proc = true;
}

//Store a Scheme defined procedure
Object::Object(Procedure &procedure) {
	pp = NULL;
	proc = procedure;
	kind = "procedure";
	value = "";
	native_proc = false;
}

//Get the type of data stored - variable or procedure
string Object::get_kind() {
	return kind;
}

//Get the value stored
string Object::get_value() {
	return value;
}

//Return true if the procedure is native (C++) and false if it was defined in Scheme
bool Object::check_native_procedure() {
	return native_proc;
}

//Run the current procedure with the arguments V
string Object::apply(vector<string>&V) {
	return pp(V);
}

//Run the current procedure with no arguments
string Object::apply() {
	vector<string> V;
	return pp(V);
}

//Get the list of arguments of the current procedure
PList Object::get_arg_list() {
	return proc.get_arg_list();
}

//Get the body of the current procedure
PList Object::get_body() {
	return proc.get_body();
}

//Get the Scheme procedure
Procedure Object::get_proc() {
	return proc;
}

PList::PList() {
}

//Init a PList from a vector of strings
PList::PList(vector<string>&vv) {
	store = vv;
}

//Init a PList from a string
PList::PList(string &ss) {
	store.push_back("(");
	store.push_back(ss);
	store.push_back(")");
}

//Print the content of an s-expression
void PList::print() {
	for (size_t i = 0; i < store.size(); i++)
		cout << store[i] << " ";
	cout << endl;
}

//Convert the contents to a single string
string PList::toString() {
	string aux = "";
	for (size_t i = 0; i < store.size(); i++)
		aux = aux + store[i] + " ";
	return aux;
}

//Get a copy of the vector uses to store the PList
vector<string> PList::get_store() {
	return store;
}

//Clear the contents
void PList::clear() {
	store.clear();
}

//Get the number of s-expressions contained on a level
//for example:
//(+ aa bb) will return 3
//(+ (- 2 3)) will return 2
size_t PList::size() {
	size_t nn = 0, flag = 0;
	size_t left = 0, right = 0;
	if (store.size() == 0)
		return nn;
	if (store.size() == 1) {
		nn = 1;
		return nn;
	}
	for (size_t i = 1; i < store.size() - 1; i++) {
		if (store[i] == "(")
			left++;
		if (store[i] == ")")
			right++;

		if (left == 0 && left == right)
			nn++;
		else if (left == 1 && flag == 0) {
			nn++;
			flag = 1;
		} else if (left != 0 && left == right) {
			flag = 0;
			left = 0;
			right = 0;
		}
	}
	return nn;
}

//Get the sub PList stored at pos
//e.g. (+ (- 2 3) (* 1 1)) get(0) will return "+"
PList PList::get(size_t pos) {
	PList pp;
	if (store.size() == 1)
		pp = *this;
	size_t nn = 0, flag = 0, flag_read = 0;
	size_t left = 0, right = 0;
	for (size_t i = 1; i < store.size() - 1; i++) {
		if (store[i] == "(")
			left++;
		if (store[i] == ")")
			right++;

		if (left == 0 && left == right) {
			nn++;
			if (pos == nn - 1) {
				pp.store.push_back(store[i]);
				break;
			}
		} else if (left == 1 && flag == 0) {
			nn++;
			flag = 1;
			if (pos == nn - 1) {
				flag_read = 1;
			}
		} else if (left != 0 && left == right) {
			flag = 0;
			left = 0;
			right = 0;
			flag_read = 0;
		} else if (flag_read == 1)
			pp.store.push_back(store[i]);
	}
	if (pp.store.size() == 1)
		return pp;
	else {
		PList cc;
		cc.store.push_back("(");
		for (size_t ii = 0; ii < pp.full_size(); ii++)
			cc.store.push_back(pp.store[ii]);
		cc.store.push_back(")");
		return cc;
	}
}

//Get the full size of vector used to store the PList
size_t PList::full_size() {
	return store.size();
}

//Get the elem at pos from the vector used to store the PList
string PList::elem(size_t pos) {
	string inp = store[pos];
	//Clean all trailing empty spaces
	while (inp[inp.size() - 1] == ' ')
		inp.erase(inp.size() - 1); //clean some empty spaces
	return inp;
}

//Add a string at the end of the current vector
void PList::puts(string ss) {
	store.push_back(ss);
}
//Class used to store a Scheme defined procedure
//the procedure is stored in two PLists: arguments and body

Procedure::Procedure() {
}

//Init a procedure from a PList
Procedure::Procedure(PList &pp) {
	arg_list = pp.get(1);
	vector<string> aux;
	aux.push_back("(");
	aux.push_back("begin");
	for (unsigned int i = 2; i < pp.size(); i++) {
		addPList(aux, pp.get(i));
	}
	aux.push_back(")");
	body = PList(aux);
}

//Add a PList to the end of the vector V
void Procedure::addPList(vector<string>&V, PList a) {
	vector<string> aux = a.get_store();
	for (unsigned int i = 0; i < aux.size(); i++) {
		V.push_back(aux[i]);
	}
}

//Get the list of arguments of the current procedure
PList Procedure::get_arg_list() {
	return arg_list;
}

//Get the body of the current procedure
PList Procedure::get_body() {
	return body;
}
//Basic procedures:
//Each procedure receives as input a vector of strings and returns a string

//Print one or more variables; e.g. (print aa), (print aa bb cc)
string print(vector<string>&vv) {
	unsigned count = 0;
	stringstream result("");
	for(auto i: vv) {
		//cout << i << " ";
		result << i << " ";
		count++;
		//cout <<  i << " ";
	}

	/*for (unsigned int i = 0; i < vv.size(); i++) {
		cout << vv[i] << " ";
	}*/
	string res = result.str();
	if(count==1)
	   res = res.substr(1, res.length()-3);
	//if(count==1
	//(res.begin(), res.end(), ptr_fun(::ispunct)), res.end());
		//res.erase(std::find(res.begin(), res.end(), '"'));

	cout << res << endl;
	return ("");
}

//Add one or more variables; e.g. (+ aa), (+ aa bb cc)
string add(vector<string>&vv) {
	if (vv.size() == 0)
		return "Wrong number of arguments for procedure +";
	stringstream ss;
	double sum = strtod(vv[0].c_str(), NULL);
	for (size_t i = 1; i < vv.size(); i++)
		sum += strtod(vv[i].c_str(), NULL);
	ss << sum;
	return ss.str();
}

//Diff, e.g.: (- aa bb)
string diff(vector<string>&vv) {
	if (vv.size() == 0)
		return "Wrong number of arguments for procedure -";
	stringstream ss;
	double sum = strtod(vv[0].c_str(), NULL);
	for (size_t i = 1; i < vv.size(); i++)
		sum -= strtod(vv[i].c_str(), NULL);
	ss << sum;
	return ss.str();
}

//Division, e.g. (/ aa bb cc ...)
string divv(vector<string>&vv) {
	if (vv.size() == 0)
		return "Wrong number of arguments for procedure /";
	stringstream ss;
	double sum = strtod(vv[0].c_str(), NULL);
	for (size_t i = 1; i < vv.size(); i++)
		sum /= strtod(vv[i].c_str(), NULL);
	ss << sum;
	return ss.str();
}

//Product, e.g. (* aa bb cc ...)
string prod(vector<string>&vv) {
	if (vv.size() == 0)
		return "Wrong number of arguments for procedure *";
	stringstream ss;
	double sum = strtod(vv[0].c_str(), NULL);
	for (size_t i = 1; i < vv.size(); i++)
		sum *= strtod(vv[i].c_str(), NULL);
	ss << sum;
	return ss.str();
}

//Quit the interpreter, e.q. (quit) or (exit)
string scheme_quit(vector<string>&vv) {
	exit(1);
	return "ok";
}

//Logical operators: <, <=, >, >=, =, !=

string less_than(vector<string>&vv) {
	if (vv.size() == 1)
		return "#t";
	for (size_t i = 0; i < vv.size() - 1; i++) {
		if (!(strtod(vv[i].c_str(), NULL) < strtod(vv[i + 1].c_str(), NULL)))
			return "#f";
	}
	return "#t";
}

string less_or_equal(vector<string>&vv) {
	if (vv.size() == 1)
		return "#t";
	for (size_t i = 0; i < vv.size() - 1; i++)
		if (!(strtod(vv[i].c_str(), NULL) <= strtod(vv[i + 1].c_str(), NULL)))
			return "#f";
	return "#t";
}

string great_than(vector<string>&vv) {
	if (vv.size() == 1)
		return "#t";
	for (size_t i = 0; i < vv.size() - 1; i++)
		if (!(strtod(vv[i].c_str(), NULL) > strtod(vv[i + 1].c_str(), NULL)))
			return "#f";
	return "#t";
}

string great_or_equal(vector<string>&vv) {
	if (vv.size() == 1)
		return "#t";
	for (size_t i = 0; i < vv.size() - 1; i++)
		if (!(strtod(vv[i].c_str(), NULL) >= strtod(vv[i + 1].c_str(), NULL)))
			return "#f";
	return "#t";
}

string equall(vector<string>&vv) {
	if (vv.size() == 1)
		return "#t";
	for (size_t i = 0; i < vv.size() - 1; i++)
		if (!(strtod(vv[i].c_str(), NULL) == strtod(vv[i + 1].c_str(), NULL)))
			return "#f";
	return "#t";
}

string not_equal(vector<string>&vv) {
	if (vv.size() == 1)
		return "#t";
	for (size_t i = 0; i < vv.size() - 1; i++)
		if (!(strtod(vv[i].c_str(), NULL) != strtod(vv[i + 1].c_str(), NULL)))
			return "#f";
	return "#t";
}

typedef map<string, Object> Environment;

string apply_proc(Procedure &proc, PList &args, Environment env);
Cell eval(PList &pp, Environment &env);
string apply_proc(Procedure &proc, PList &args, Environment env);
void REPL(Environment &env);

bool show_err1_flag = true;
bool show_err2_flag = true;

//START_NOTE
//It will be nice to implement some history for the commands, this way a user won't be forced to retype again a command.
//No plan for implementing this now.
//If you want to use syntax highlighting, paranthese matching etc... you can use schm from Emacs!
//END_NOTE

Cell eval(PList &pp, Environment &env) {
	int N = pp.size();
	if (N == 1) { //Check for symbol, constant literal, procedure with no argument
		if (pp.elem(0) == "(" && pp.elem(pp.full_size() - 1) == ")") {
			PList aux = pp.get(0);
			string inp = aux.elem(0);
			for (unsigned int i = 1; i < pp.full_size() - 2; i++)
				inp = inp + pp.elem(i);
			//Check for procedure with no argument, e.g. (quit)
			if (env.find(inp) != env.end()) {
				if (env[inp].get_kind() == "procedure"
						&& env[inp].check_native_procedure() == true)
					return env[inp].apply();
				else
					return env[inp].get_value();
			} else {
				return (("Error! Unbound variable: " + inp));
			}
		} else {
			string inp = pp.elem(0);
			//Check if character
			if (inp[0] == '#' && inp[1] == '\\')
				return Cell("character type not yet implemented");
			//Check if string
			if (inp[0] == '\"' && inp[inp.size() - 1] == '\"')
				return inp;
			//Check if number
			if (number(inp))
				return inp;
			//Check if variable or procedure
			if (env.find(inp) != env.end()) {
				if (env[inp].get_kind() == "variable")
					return env[inp].get_value();
				else {
					if (show_err1_flag)
						cout << env[inp].get_kind() << " ";
					show_err1_flag = true;
					return inp;
				}
			} else {
				string res;
				if (show_err2_flag)
					res = "Error! Unbound variable: " + inp;
				show_err2_flag = true;
				return res;
			}
		}
	} else {
		show_err1_flag = false;
		show_err2_flag = false;
		string proc;
		PList aux = pp.get(0);
		if (aux.size() == 1)
			proc = aux.elem(0);
		else {
			PList aux2 = aux.get(0);
			string tst = aux2.elem(0);
			if (tst == "lambda") {
				Procedure anonymous = Procedure(aux);
				//Collect the arguments of the lambda expression:
				PList args;
				args.puts("(");
				for (int i = 1; i < N; i++) {
					PList piece = pp.get(i);
					string res = (eval(piece, env)).get_str();
					args.puts(res);
				}
				args.puts(")");
				return apply_proc(anonymous, args, env);
			} else {
				proc = (eval(aux, env)).get_str();
			}
		}
		if (proc == "define") {
			if (pp.size() != 3)
				return Cell("Ill-formed special form: define");
			else {
				string name = (pp.get(1)).elem(0);
				PList value = pp.get(2);
				Cell res = eval(value, env);
				if (res.get_str() == "") {
					Procedure prr = res.get_proc();
					env[name] = prr;
				} else {
					string stt = res.get_str();
					env[name] = stt;
				}
				return Cell("");
			}
		} else if (proc == "set!") {
			if (pp.size() != 3)
				return Cell("Ill-formed special form: set!");
			else {
				string name = (pp.get(1)).elem(0);
				if (env.find(name) == env.end()) {
					return Cell("Error! Unbound variable: " + name);
				}
				PList value = pp.get(2);
				string res = (eval(value, env)).get_str();
				env[name] = res;
				return Cell("");
			}
		} else if (proc == "quote") {
			if (pp.size() != 2)
				return Cell("Ill-formed special form: quote");
			else {
				PList value = pp.get(1);
				return value.toString();
			}
		} else if (proc == "if") {
			if (pp.size() == 3) {
				PList cond = pp.get(1);
				PList if_true = pp.get(2);
				string aux = (eval(cond, env)).get_str();
				//If cond is a number evaluate the TRUE branch, if cond is a boolean evaluate accordingly
				if (number(aux))
					return eval(if_true, env);
				if (aux == "#t")
					return eval(if_true, env);
				else
					return Cell("");
			}
			if (pp.size() == 4) {
				PList cond = pp.get(1);
				PList if_true = pp.get(2);
				PList if_false = pp.get(3);
				string aux = (eval(cond, env)).get_str();
				//If cond is a number evaluate the TRUE branch, if cond is a boolean evaluate accordingly
				if (number(aux))
					return eval(if_true, env);
				if (aux == "#t")
					return eval(if_true, env);
				else
					return eval(if_false, env);
			} else {
				return Cell("Ill-formed special form: if");
			}
		} else if (proc == "lambda") {
			Procedure pr = Procedure(pp);
			return pr;
		} else if (proc == "begin") {
			if (pp.size() < 2)
				return Cell("Ill-formed special form: begin");
			string res;
			for (unsigned int i = 1; i < pp.size(); i++) {
				PList aux = pp.get(i);
				res = (eval(aux, env)).get_str();
			}
			return res;
		} else {
			PList exps;
			exps.puts("(");
			for (int i = 0; i < N; i++) {
				PList piece = pp.get(i);
				string aux = (eval(piece, env)).get_str();
				if (aux == "")
					aux = (piece.get(0)).elem(0);
				exps.puts(aux);
			}
			exps.puts(")");
			string pr = (exps.get(0)).elem(0);
			vector<string> args;
			for (unsigned int i = 1; i < exps.size(); i++)
				args.push_back((exps.get(i)).elem(0));
			if (env.find(pr) != env.end()) {
				if (env[pr].check_native_procedure()) {
					return env[pr].apply(args);
				} else {
					Procedure prt = env[pr].get_proc();
					PList argss;
					argss.puts("(");
					for (int i = 1; i < N; i++) {
						PList piece = pp.get(i);
						string res = (eval(piece, env)).get_str();
						argss.puts(res);
					}
					argss.puts(")");
					return apply_proc(prt, argss, env);
				}

			} else {
				return Cell("Error! Unbound variable: " + pr);
			}
		}
	}
}

//Use this for named Scheme procedures, e.g. (define square (lambda (x) (* x x)))

string apply_proc(Procedure &proc, PList &args, Environment env) {
	PList arg_list = proc.get_arg_list();
	PList body = proc.get_body();
	for (unsigned int i = 0; i < args.size(); i++) {
		string aux = (arg_list.get(i)).elem(0);
		PList piece = args.get(i);
		string stt = (eval(piece, env)).get_str();
		env[aux] = stt;
	}
	string res = (eval(body, env)).get_str();
	return res;
}

//Simple REPL (read - evaluate - print).

void REPL(Environment &env) {
	prompt();
	for (;;) {
		show_err1_flag = true;
		show_err2_flag = true;
		string inp = get_input();
		if (inp == "")
			continue; //if the input buffer is empty go to the start of the for loop
		vector<string> out = clean_input(inp);
		//Evaluate an expression and print the result
		PList pp = PList(out);
		string res = (eval(pp, env)).get_str();
		if (res == "") {
			prompt();
			continue;
		}
		cout << res << endl;
		prompt();
	}
}

Environment initEnvironment() {
	Environment env;
	env["+"] = add;
	env["-"] = diff;
	env["*"] = prod;
	env["/"] = divv;
	env["exit"] = scheme_quit;
	env["quit"] = scheme_quit;
	env["="] = equall;
	env["!="] = not_equal;
	env["<"] = less_than;
	env["<="] = less_or_equal;
	env[">"] = great_than;
	env[">="] = great_or_equal;
	env["print"] = print;

	string ss = "Ill formed special form: lambda.";
	env["lambda"] = ss;
	ss = "Ill formed special form: quote.";
	env["quote"] = ss;
	ss = "Ill formed special form: begin.";
	env["begin"] = ss;
	ss = "Ill formed special form: set!.";
	env["set!"] = ss;
	ss = "Ill formed special form: define.";
	env["define"] = ss;
	ss = "Ill formed special form: if.";
	env["if"] = ss;
	return env;
}

void execute(const char *filetoexecute) {
	Environment env = initEnvironment();
	ifstream file;
	file.open(filetoexecute);
	if(file.is_open()) {

		string line;

		while (std::getline(file, line)) {
			//cout << "FILE: " << line << endl;
			show_err1_flag = true;
			show_err2_flag = true;
			vector<string> out = clean_input(line);

			//Evaluate an expression and print the result
			PList pp = PList(out);
			string res = (eval(pp, env)).get_str();
			if(res.length()>0)
			  cout << "RESULT: " << res << endl;
		}
		file.close();
	}
}

//Start the interpreter and load the predefined procedures

//Print the Scheme prompt
void prompt() {
	cout << "schm >>>";
}

//Read the user input character by character in a STL string.

string get_input() {
	string inp;
	char tmp;
	int left = 0, right = 0, ll = 0;
	for (;;) {
		cin.get(tmp);
		//If you find ";" on a line ignore everything until the end of line (comment line)
		if (tmp == ';') {
			while (tmp != '\n')
				cin.get(tmp);
		}
		if (tmp != '\n')
			inp += tmp;
		if (tmp == '(')
			left++;
		else if (tmp == ')')
			right++;
		else if (tmp == '\"')
			ll++;

		//At end of line (user has pressed Enter) check if you have un-matched parantheses or quotation marks
		if (tmp == '\n') {
			if (left == 0 && right == 0 && ll == 0)
				break;
			if (left != 0 && (left == right) && (ll == 0 || (ll & 1) == 0))
				break;
			if (left == 0 && right == 0 && (ll & 1) == 0)
				break;
			if ((ll & 1))
				inp += "\\n";
			else
				inp += " ";
		}
	}
	return inp;
}

//START_NOTE
//"replace_substr" and "clean_input" can be replaced by a more elegant regex solution
//unfortunateley gcc does not fully support regex (at the time of this implementation)
//Visual Studio 2010 fully supports regex
//END_NOTE

//Replace substring target with substring res in string inp

void replace_substr(string &inp, string target, string res) {
	size_t ind = 0;
	for (;;) {
		ind = inp.find(target, ind);
		if (ind == string::npos)
			break;
		inp.replace(ind, 1, res);
		ind += res.length();
	}
}

//Split the input string in components and store them in a STL vector of strings

vector<string> clean_input(string &str) {
	replace_substr(str, "(", " ( ");
	replace_substr(str, ")", " ) ");

	stringstream ss(str);
	string aux;
	vector<string> init_pieces;
	while (ss >> aux)
		init_pieces.push_back(aux);
	if (str.find("\"") == string::npos)
		return init_pieces;
	//Treat the case of 'sentences'; for example "aaa bbb ccc" should not be broken into pieces
	vector<string> pieces;
	aux = "";
	int flag = 0;
	for (size_t i = 0; i < init_pieces.size(); i++) {

		if (init_pieces[i].find("\"") != string::npos) {
			if (init_pieces[i][0] == '\"'
					&& init_pieces[i][init_pieces[i].size() - 1] == '\"') {
				pieces.push_back(init_pieces[i]);
				continue;
			}
			flag = 1;
			if (init_pieces[i][0] == '\"')
				aux = init_pieces[i] + " ";
			else if (init_pieces[i][init_pieces[i].size() - 1] == '\"') {
				aux += init_pieces[i] + " ";
				pieces.push_back(aux);
				aux = "";
				flag = 0;
			} else
				aux += init_pieces[i] + " ";
		} else if (flag == 0) {
			pieces.push_back(init_pieces[i]);
		} else
			aux += init_pieces[i] + " ";
	}
	return pieces;
}

//START_NOTE
//This ugly hack should be replaced with a regex that can decide if we have int,double or complex numbers
//END_NOTE

//Check if the string contains a number

bool number(string inp) {
	char *pend;
	double tst = strtod(inp.c_str(), &pend);
	if (inp[0] != (*pend))
		return true;
	else
		return false;
}

void shell() {
	Environment env = initEnvironment();
	REPL(env);
}


#endif /* INTERPRETER_CPP_ */
