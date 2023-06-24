#include "include/Operations.h"
#include "return_codes.h"

#include <stack>

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cerr << MSG_INVALID_PARAMETER << std::endl;
		return ERROR_PARAMETER_INVALID;
	}

	try
	{
		std::string token;
		std::stack< LN > st;
		std::ifstream in(argv[1]);
		std::ofstream out(argv[2]);

		if (!in.is_open())
		{
			throw std::ifstream::failure(MSG_CANNOT_OPEN_FILE);
		}

		std::unordered_map< std::string, bin_f > bin_ops = init_bin_ops();
		std::unordered_map< std::string, unary_f > un_ops = init_unary_ops();

		while (in >> token)
		{
			if (bin_ops.count(token))
			{
				eval_binary(st, bin_ops[token]);
			}
			else if (un_ops.count(token))
			{
				eval_unary(st, un_ops[token]);
			}
			else if (token == "~" || token == "*" || token == "/" || token == "%")
			{
				throw std::logic_error(MSG_UNSUPPORTED);
			}
			else
			{
				st.emplace(token);
			}
		}

		while (!st.empty())
		{
			out << st.top().to_string() << std::endl;
			st.pop();
		}
	} catch (const std::bad_alloc&)
	{
		std::cerr << MSG_NOT_ENOUGH_MEMORY << std::endl;
		return ERROR_OUT_OF_MEMORY;
	} catch (const std::ifstream::failure& e)
	{
		std::cerr << e.what() << std::endl;
		return ERROR_CANNOT_OPEN_FILE;
	} catch (const std::invalid_argument& e)
	{
		std::cerr << e.what() << std::endl;
		return ERROR_DATA_INVALID;
	} catch (const std::logic_error& e)
	{
		std::cerr << e.what() << std::endl;
		return ERROR_UNSUPPORTED;
	} catch (const std::overflow_error& e)
	{
		std::cerr << e.what() << std::endl;
		return ERROR_DATA_INVALID;
	} catch (const std::exception&)
	{
		std::cerr << MSG_UNKNOWN << std::endl;
		return ERROR_UNKNOWN;
	}
	return SUCCESS;
}
