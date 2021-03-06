/*
 *	MetaCall Command Line Interface by Parra Studios
 *	Copyright (C) 2016 - 2019 Vicente Eduardo Ferrer Garcia <vic798@gmail.com>
 *
 *	A command line interface example as metacall wrapper.
 *
 */

/* -- Headers -- */

#include "application.hpp"
#include "tokenizer.hpp"
#include "parser.hpp"

#include <algorithm>
#include <iostream>

/* -- Namespace Declarations -- */

using namespace metacallcli;

/* -- Private Methods -- */

bool command_cb_help(application & /*app*/, tokenizer & /*t*/)
{
	std::cout << "MetaCall Command Line Interface by Parra Studios" << std::endl;
	std::cout << "Copyright (C) 2016 - 2019 Vicente Eduardo Ferrer Garcia <vic798@gmail.com>" << std::endl;
	std::cout << std::endl << "A command line interface example as metacall wrapper" << std::endl;

	/* Command list */
	std::cout << std::endl << "Command list :" << std::endl << std::endl;

	/* Call command */
	std::cout << "\tcall <expression>(<arg0>, <arg1>, ... , <argN>)" << std::endl;
	std::cout << "\t\texpression : alphanumeric string beginning by letter [method1, method2, hello]" << std::endl;
	std::cout << "\t\targuments  :" << std::endl;
	std::cout << "\t\t\tbool [true | false]," << std::endl;
	std::cout << "\t\t\tchar ['A', 'B', 'C']," << std::endl;
	std::cout << "\t\t\tint [234, 0, -16673]," << std::endl;
	std::cout << "\t\t\tlong [58L, 9857 ... 3942]," << std::endl;
	std::cout << "\t\t\tfloat [5.42f, 458E3f]," << std::endl;
	std::cout << "\t\t\tdouble [5.42, 458E12]," << std::endl;
	std::cout << "\t\t\tpointer [0x07d97e32]," << std::endl;
	std::cout << "\t\t\tstring [\"hello world\"]" << std::endl;

	/* Load command */
	std::cout << std::endl << "\tload <tag> <script0> <script1> ... <scriptN>" << std::endl;
	std::cout << "\t\ttag : loader tag will be used to run scripts [rb, js, py, mock, node]" << std::endl;

	/* Clear command */
	std::cout << std::endl << "\tclear <tag> <script0> <script1> ... <scriptN>" << std::endl;
	std::cout << "\t\ttag : loader tag will be used to run scripts [rb, js, py, mock, node]" << std::endl;

	/* Exit command */
	std::cout << std::endl << "\texit" << std::endl;

	/* Help command */
	std::cout << std::endl << "\thelp" << std::endl;

	return true;
}

bool command_cb_debug(application & app, tokenizer & t)
{
	std::cout << "[DEBUG]" << std::endl;

	app.command_debug(*t.begin(), t);

	return true;
}

bool command_cb_call(application & app, tokenizer & t)
{
	const std::string func_delimiters(" \n\t\r\v\f(,)");

	tokenizer::iterator it = t.begin();

	parser_parameter p(it);

	std::vector<void *> args;

	/* Set custom function delimiters */
	t.delimit(func_delimiters);

	/* Skip command key */
	++it;

	/* Parse function call */
	if (it != t.end())
	{
		std::string func_name(*it);

		const std::string param_delimiters("(,)");

		t.delimit(param_delimiters);

		++it;

		if (it != t.end())
		{
			const std::string param_escape(" \n\t\r\v\f");

			void * v = NULL;

			do
			{
				it.escape(param_escape);

				v = app.argument_parse(p);

				if (v != NULL)
				{
					args.push_back(v);
				}
				else
				{
					std::cout << "\tinvalid argument : {" << *it << "}" << std::endl;
				}

				++it;

			} while (it != t.end());
		}

		void * result = app.metacallv_adaptor(func_name, args);

		if (result != NULL)
		{
			size_t size = 0;

			struct metacall_allocator_std_type std_ctx = { &std::malloc, &std::realloc, &std::free };

			void * allocator = metacall_allocator_create(METACALL_ALLOCATOR_STD, (void *)&std_ctx);

			char * value_str = metacall_serialize(result, &size, allocator);

			std::cout << value_str << std::endl;

			metacall_value_destroy(result);

			metacall_allocator_free(allocator, value_str);

			metacall_allocator_destroy(allocator);
		}
		else
		{
			std::cout << "(null)" << std::endl;
		}

		return true;
	}

	return false;
}

bool command_cb_load(application & app, tokenizer & t)
{
	tokenizer::iterator it = t.begin();

	parser p(it);

	std::string loader_tag;

	++it;

	if (p.is<std::string>())
	{
		loader_tag = *it;
	}

	do
	{
		++it;

	} while (it != t.end() && p.is<std::string>() && app.load(loader_tag, *it));

	return true;
}

bool command_cb_clear(application & app, tokenizer & t)
{
	tokenizer::iterator it = t.begin();

	parser p(it);

	std::string loader_tag;

	++it;

	if (p.is<std::string>())
	{
		loader_tag = *it;
	}

	do
	{
		++it;

	} while (it != t.end() && p.is<std::string>() && app.clear(loader_tag, *it));

	return true;
}

bool command_cb_exit(application & app, tokenizer & /*t*/)
{
	std::cout << "Exiting ..." << std::endl;

	app.shutdown();

	return true;
}

/* -- Methods -- */

application::parameter_iterator::parameter_iterator(application & app, const std::string & tag, application::script_list & scripts) :
	app(app), tag(tag), scripts(scripts)
{

}

application::parameter_iterator::~parameter_iterator()
{

}

void application::parameter_iterator::operator()(const char * parameter)
{
	std::string script(parameter);

	if (app.load(tag, script) == false)
	{
		std::cout << "Invalid parameter (" << parameter << ")" << std::endl;
	}
}

bool application::load(const std::string & tag, const std::string & script)
{
	const char * script_list[] =
	{
		script.c_str()
	};

	if (metacall_load_from_file(tag.c_str(), script_list, sizeof(script_list) / sizeof(script_list[0]), NULL) != 0)
	{
		std::cout << "Script (" << script << ") load error in loader (" << tag << ")" << std::endl;

		return false;
	}

	scripts.push_back(script);

	std::cout << "Script (" << script << ") loaded correctly" << std::endl;

	return true;
}

bool application::clear(const std::string & tag, const std::string & script)
{
	void * handle = metacall_handle(tag.c_str(), script.c_str());

	if (handle == NULL)
	{
		std::cout << "Script (" << script << ") not found in loader (" << tag << ")" << std::endl;

		return false;
	}

	if (metacall_clear(handle) != 0)
	{
		std::cout << "Script (" << script << ") clear error in loader (" << tag << ")" << std::endl;

		return false;
	}

	script_list::iterator it = std::find(scripts.begin(), scripts.end(), script);

	if (it != scripts.end())
	{
		scripts.erase(it);
	}

	std::cout << "Script (" << script << ") removed correctly" << std::endl;

	return true;
}

application::application(int argc, char * argv[]) : exit_condition(false)
{
	/* Initialize MetaCall */
	if (metacall_initialize() != 0)
	{
		/* Exit from application */
		shutdown();
	}

	/* Print MetaCall information */
	metacall_print_info();

	/* Parse program arguments if any */
	if (argc > 1)
	{
		parameter_iterator param_it(*this, argv[1], scripts);

		/* Parse program parameters */
		std::for_each(&argv[2], argv + argc, param_it);
	}

	/* Define available commands */
	define("help", &command_cb_help);

	define("debug", &command_cb_debug);

	define("call", &command_cb_call);

	define("load", &command_cb_load);

	define("clear", &command_cb_clear);

	define("exit", &command_cb_exit);
}

application::~application()
{
	if (metacall_destroy() != 0)
	{
		std::cout << "Error while destroying MetaCall." << std::endl;
	}
}

void application::run()
{
	/* Show welcome message  */
	std::cout << "Welcome to Tijuana, tequila, sexo & marijuana." << std::endl;

	while (exit_condition != true)
	{
		std::string input;

		/* Show prompt line */
		std::cout << "> ";

		/* Get whole line */
		std::getline(std::cin, input);

		/* Check for valid data */
		if (input.length() > 0)
		{
			/* Create tokenizer from input string */
			tokenizer t(input);

			/* Execute the command */
			execute(t);
		}

	} while (exit_condition != true);
}

void application::shutdown()
{
	exit_condition = true;
}

void application::command_debug(const std::string & key, const tokenizer & t)
{
	std::cout << "{" << std::endl
		<< "\tkey : " << key << "," << std::endl
		<< "\targuments :" << std::endl;

	for (tokenizer::iterator it = t.begin(); it != t.end(); ++it)
	{
		std::cout << "\t\t[" << it.position() << "] " << *it << std::endl;
	}

	std::cout << "}" << std::endl;
}

void application::execute(tokenizer & t)
{
	tokenizer::iterator it = t.begin();

	command_callback cb = commands[*it];

	if (cb == nullptr)
	{
		std::cout << "[WARNING]: Invalid command" << std::endl;

		command_debug(*it, t);

		return;
	}

	if (cb(*this, t) == false)
	{
		std::cout << "[WARNING]: Invalid command execution" << std::endl;

		command_debug(*it, t);

		return;
	}
}

void application::define(const char * key, application::command_callback command_cb)
{
	std::string cmd(key);

	commands[cmd] = command_cb;
}

void * application::argument_parse(parser_parameter & p)
{
	if (p.is<bool>())
	{
		bool b = p.to<bool>();

		boolean bo = static_cast<boolean>(b);

		return metacall_value_create_bool(bo);
	}
	else if (p.is<char>())
	{
		char c = p.to<char>();

		return metacall_value_create_char(c);
	}
	else if (p.is<int>())
	{
		int i = p.to<int>();

		return metacall_value_create_int(i);
	}
	else if (p.is<long>())
	{
		long l = p.to<long>();

		return metacall_value_create_long(l);
	}
	else if (p.is<float>())
	{
		float f = p.to<float>();

		return metacall_value_create_float(f);
	}
	else if (p.is<double>())
	{
		double d = p.to<double>();

		return metacall_value_create_double(d);
	}
	else if (p.is<void *>())
	{
		void * ptr = p.to<void *>();

		return metacall_value_create_ptr(ptr);
	}
	else if (p.is<std::string>())
	{
		std::string str = p.to<std::string>();

		return metacall_value_create_string(str.c_str(), str.length());
	}

	return NULL;
}

void * application::metacallv_adaptor(const std::string & name, const std::vector<void *> & args)
{
	void ** args_ptr = new void * [args.size()];

	void * result = NULL;

	if (args_ptr != nullptr)
	{
		size_t iterator = 0;

		std::for_each(args.begin(), args.end(), [&iterator, args_ptr](void * v)
		{
			args_ptr[iterator] = v;

			++iterator;
		});

		result = metacallv(name.c_str(), args_ptr);

		delete[] args_ptr;
	}

	return result;
}
