#pragma once
#include "LN.h"
#include <unordered_map>

#include <functional>
#include <stack>

template< typename BF >
void eval_binary(std::stack< LN >& st, BF&& f)
{
	LN a = std::move(st.top());
	st.pop();
	LN b = std::move(st.top());
	st.pop();
	st.emplace(f(a, b));
}

template< typename UF >
void eval_unary(std::stack< LN >& st, UF&& f)
{
	LN a = std::move(st.top());
	st.pop();
	st.emplace(f(a));
};

using bin_f = std::function< LN(const LN&, const LN&) >;
using unary_f = std::function< LN(const LN&) >;

std::unordered_map< std::string, bin_f > init_bin_ops()
{
	std::unordered_map< std::string, bin_f > bin_ops;
	bin_ops["+"] = [](const LN& a, const LN& b) -> LN
	{
		return a + b;
	};
	bin_ops["-"] = [](const LN& a, const LN& b) -> LN
	{
		return a - b;
	};
	bin_ops["!="] = [](const LN& a, const LN& b) -> bool
	{
		return a != b;
	};
	bin_ops["=="] = [](const LN& a, const LN& b) -> bool
	{
		return a == b;
	};
	bin_ops[">="] = [](const LN& a, const LN& b) -> bool
	{
		return a >= b;
	};
	bin_ops["<="] = [](const LN& a, const LN& b) -> bool
	{
		return a <= b;
	};
	bin_ops[">"] = [](const LN& a, const LN& b) -> bool
	{
		return a > b;
	};
	bin_ops["<"] = [](const LN& a, const LN& b) -> bool
	{
		return a < b;
	};
	return bin_ops;
}

std::unordered_map< std::string, unary_f > init_unary_ops()
{
	std::unordered_map< std::string, unary_f > un_ops;
	un_ops["_"] = [](const LN& a) -> LN
	{
		return -a;
	};
	return un_ops;
}