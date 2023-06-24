#include "CVector.h"
#include "Error_msg.h"
#include <string_view>

#include <algorithm>
#include <compare>
#include <fstream>
#include <iostream>
#include <limits>
#include <mutex>

#define NaN "Nan"
#define Zero "0"

/* for universal_comparison */
enum class ComparisonResult
{
	Less = -1,
	Equivalent = 0,
	Greater = 1
};

enum class Sign
{
	Plus = 1,
	Minus = -1
};

static constexpr char hex_digits[] = "0123456789ABCDEF";
class LN
{
  public:
	using ll = long long;
	/* ctors */
	LN(const LN &v) = default;
	LN(LN &&v) = default;
	LN(ll v = 0);
	LN(const char *s);
	LN(std::string_view s);

	LN &operator=(const LN &v) = default;
	LN &operator=(LN &&v) = default;

	/* binary */
	LN operator+(const LN &v) const;
	LN &operator+=(const LN &v)
	{
		*this = *this + v;
		return *this;
	};
	LN operator-(const LN &v) const;
	LN &operator-=(const LN &v)
	{
		*this = *this - v;
		return *this;
	};

	/* unary */
	LN operator-() const;

	/* order relationship
	 * https://stackoverflow.com/questions/47466358/what-is-the-spaceship-three-way-comparison-operator-in-c
	 * operators based on <=> */
	bool operator<(const LN &v) const = default;
	bool operator<=(const LN &v) const = default;
	bool operator>(const LN &v) const = default;
	bool operator>=(const LN &v) const = default;
	bool operator!=(const LN &v) const = default;
	/* except for == */
	bool operator==(const LN &v) const { return (*this <=> v) == std::partial_ordering::equivalent; }

	explicit operator long long() const;
	explicit operator bool() const;
	[[nodiscard]] std::string to_string() const;

  private:
	Sign sign_ = Sign::Plus;
	bool nanable_ = false;
	CVector< uint32_t > data_;

	std::partial_ordering operator<=>(const LN &v) const;
	static LN nan();
	[[nodiscard]] ComparisonResult universal_comparison(const LN &v) const;
	[[nodiscard]] uint32_t get_data(size_t idx) const { return idx >= data_.get_size() ? 0 : data_[idx]; };
	static LN add(const LN &a, const LN &b);
	static LN subtract(const LN &a, const LN &b);
	static Sign invert_sign(Sign s);
	void parse_from_string_range(const char *l, const char *r);
};

LN operator""_ln(const char *s);