/*  Audio Signal Processing routines in C++
    © Philippe Strauss, 2013  */


#include <cstdio>
#include <dspc/exception.hh>


exception_t::exception_t(string _filename, string _funcname, int _linenumber):
    filename(_filename),
    funcname(_funcname),
    linenumber(_linenumber)
{}

exception_t::exception_t(string _source, string _msg, string _filename,
                          string _funcname, int _linenumber):
    filename(_filename),
    funcname(_funcname),
    linenumber(_linenumber)
{
	add_line(_source, _msg);
}

exception_t::~exception_t(void) { }

void exception_t::add_line(string _source, string _msg) {
	err_msg_line_t err_msg;

	err_msg.source = _source;
	err_msg.msg = _msg;
	linelist.push_back(err_msg);
}

string exception_t::get_msg(void) {
	ostringstream out;

	out << "-- Oops! --" << endl;
	out << "C++ runtime exception mechanism triggered !" << endl; 
	out << "Source file \'" << filename << "\' ; " << "Function : \'" << funcname << "\' "
		<< "at line number : " << linenumber << endl;
	for (list<err_msg_line_t>::iterator itr=linelist.begin(); itr!=linelist.end(); ++itr) {
		out << "Originator : " << itr->source << ", User (dev.) message : " << itr->msg << endl;
	}
	out << "-- End Oops --" << endl;
	if (errno != 0) {
	    perror("-- perror:");
	    out << " --" << endl;
	}

	return out.str();
}

void exception_t::display(void) {
	cerr << get_msg();
}
