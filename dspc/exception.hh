/*  Audio Signal Processing routines in C++
    © Philippe Strauss, 2013  */


#ifndef _SPS_EXN_HH
#define _SPS_EXN_HH


#include <iostream>
#include <sstream>
#include <list>


/* From GNU/Linux /usr/include/assert.h :

   Version 2.4 and later of GCC define a magical variable `__PRETTY_FUNCTION__'
   which contains the name of the function currently being defined.
   This is broken in G++ before version 2.6.
   C9x has a similar variable called __func__, but prefer the GCC one since
   it demangles C++ function names.  */


using namespace std;


typedef struct {string source; string msg;} err_msg_line_t;
typedef list<err_msg_line_t> list_err_t;


class exception_t {

public:

	exception_t(string filename, string funcname, int linenumber);
	// exception_t(string funcname, string msg, string filename, int linenumber);
	exception_t(string source, string msg, string filename, string funcname, int linenumber);
	~exception_t(void);
	void add_line(string source, string msg);
	string get_msg(void);
	void display(void);

protected:

	string filename;
	string funcname;
	int linenumber;

private:

	list_err_t linelist;

};


#define EXN_MSG_THROW(source, msg)	exception_t(source, msg, __FILE__, __PRETTY_FUNCTION__, __LINE__)


#endif
