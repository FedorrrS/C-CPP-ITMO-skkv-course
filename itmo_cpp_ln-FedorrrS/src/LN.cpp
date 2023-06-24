#include "../include/LN.h"

LN::LN(ll v)
{
	if (v < 0)
	{
		sign_ = Sign::Minus;
		v = -v;
	}
	while (v > 0)
	{
		data_.push(static_cast< uint32_t >(v & UINT32_MAX));
		v >>= sizeof(uint32_t) * 8;
	}
}

LN::LN(const char *s)
{
	if (*s == '-')
	{
		sign_ = Sign::Minus;
		s++;
	}
	parse_from_string_range(s, s + std::strlen(s));
}

LN::LN(std::string_view s)
{
	if (s[0] == '-')
	{
		sign_ = Sign::Minus;
		s = s.substr(1);
	}
	parse_from_string_range(s.data(), s.data() + s.size());
}

LN LN::operator+(const LN &v) const
{
	LN res = { 0LL };
	if (nanable_ || v.nanable_)
	{
		return LN::nan();
	}
	if (sign_ == v.sign_)
	{
		res = add(*this, v);
		res.sign_ = sign_;
		return res;
	}
	else
	{
		switch (universal_comparison(v))
		{
		case ComparisonResult::Less:
			res = subtract(v, *this);
			res.sign_ = v.sign_;
			return res;
		case ComparisonResult::Equivalent:
			return res;
		case ComparisonResult::Greater:
			res = subtract(*this, v);
			res.sign_ = sign_;
			return res;
		default:
			throw std::invalid_argument(MSG_DATA_INVALID);
		}
	}
}

LN LN::operator-(const LN &v) const
{
	LN res = { 0LL };
	if (nanable_ || v.nanable_)
	{
		return LN::nan();
	}
	if (sign_ != v.sign_)
	{
		res = add(*this, v);
		res.sign_ = sign_;
		return res;
	}
	else
	{
		switch (universal_comparison(v))
		{
		case ComparisonResult::Less:
			res = subtract(v, *this);
			res.sign_ = invert_sign(sign_);
			return res;
		case ComparisonResult::Equivalent:
			return res;
		case ComparisonResult::Greater:
			res = subtract(*this, v);
			res.sign_ = sign_;
			return res;
		default:
			throw std::invalid_argument(MSG_DATA_INVALID);
		}
	}
}

LN LN::operator-() const
{
	LN res = *this;
	res.sign_ = invert_sign(sign_);
	return res;
}

std::partial_ordering LN::operator<=>(const LN &v) const
{
	if (nanable_ || v.nanable_)
	{
		return std::partial_ordering::unordered;
	}
	if (data_.empty() && v.data_.empty())
	{
		return std::partial_ordering::equivalent;
	}
	if (sign_ != v.sign_)
	{
		return (sign_ < v.sign_) ? std::partial_ordering::less : std::partial_ordering::greater;
	}
	else
	{
		switch (universal_comparison(v))
		{
		case ComparisonResult::Less:
			return (sign_ == Sign::Plus) ? std::partial_ordering::less : std::partial_ordering::greater;
		case ComparisonResult::Equivalent:
			return std::partial_ordering::equivalent;
		case ComparisonResult::Greater:
			return (sign_ == Sign::Minus) ? std::partial_ordering::greater : std::partial_ordering::less;
		default:
			throw std::invalid_argument(MSG_DATA_INVALID);
		}
	}
}

LN::operator long long() const
{
	if (*this > LONG_LONG_MAX || *this < LONG_LONG_MIN)
	{
		throw std::overflow_error(MSG_OVERFLOW);
	}
	else if (nanable_)
	{
		throw std::invalid_argument(MSG_NAN);
	}
	ll res = 0;
	for (size_t i = data_.get_size(); i > 0; --i)
	{
		res <<= sizeof(uint32_t) * 8;
		res |= static_cast< ll >(data_[i - 1]);
	}
	if (sign_ == Sign::Plus)
	{
		return res;
	}
	return -res;
}

LN::operator bool() const
{
	for (size_t i = 0; i < data_.get_size(); ++i)
	{
		if (data_[i] != 0)
		{
			return true;
		}
	}
	return false;
}

std::string LN::to_string() const
{
	if (nanable_)
	{
		return NaN;
	}
	if (bool(*this) == 0)
	{
		return Zero;
	}
	std::string res;
	for (size_t i = 0; i < data_.get_size(); ++i)
	{
		uint32_t b = data_[i];
		for (int j = 0; j < sizeof(uint32_t) * 8; j += 4)
		{
			res.push_back(hex_digits[(b >> j) & 0xF]);
		}
	}
	while (res.back() == '0')
	{
		res.pop_back();
	}
	if (sign_ == Sign::Minus)
	{
		res.push_back('-');
	}
	std::reverse(std::begin(res), std::end(res));
	return res;
}

ComparisonResult LN::universal_comparison(const LN &v) const
{
	if (data_.get_size() < v.data_.get_size())
	{
		return ComparisonResult::Less;
	}
	else if (data_.get_size() > v.data_.get_size())
	{
		return ComparisonResult::Greater;
	}

	for (size_t i = data_.get_size(); i > 0; --i)
	{
		if (data_[i - 1] < v.data_[i - 1])
		{
			return ComparisonResult::Less;
		}
		else if (data_[i - 1] > v.data_[i - 1])
		{
			return ComparisonResult::Greater;
		}
	}
	return ComparisonResult::Equivalent;
}

LN LN::add(const LN &a, const LN &b)
{
	LN res;
	uint32_t carry_bit = 0;
	for (size_t i = 0; i < std::max(a.data_.get_size(), b.data_.get_size()) || carry_bit; ++i)
	{
		uint64_t ea = a.get_data(i);
		uint64_t eb = b.get_data(i);
		uint64_t sum = ea + eb + carry_bit;
		carry_bit = static_cast< uint32_t >(sum >> sizeof(uint32_t) * 8);
		res.data_.push(static_cast< uint32_t >(sum & UINT32_MAX));
	}
	return res;
}

LN LN::subtract(const LN &a, const LN &b)
{
	LN res;
	uint32_t borrow_bit = 0;
	for (size_t i = 0; i < std::max(a.data_.get_size(), b.data_.get_size()) || borrow_bit; ++i)
	{
		uint64_t ea = a.get_data(i);
		uint64_t eb = b.get_data(i) + borrow_bit;
		borrow_bit = 0;
		if (eb > ea)
		{
			ea |= static_cast< uint64_t >(1) << (sizeof(uint32_t) * 8);
			borrow_bit = 1;
		}
		ea -= eb;
		res.data_.push(static_cast< uint32_t >(ea));
	}
	res.data_.remove_trailing_zeroes();
	return res;
}

void LN::parse_from_string_range(const char *l, const char *r)
{
	if (r == l)
	{
		nanable_ = true;
		return;
	}
	while (r > l)
	{
		uint32_t b = 0;
		const char *block_start = r - std::min< size_t >(r - l, sizeof(uint32_t) * 8 / 4);
		for (const char *d = block_start; d < r; ++d)
		{
			b <<= 4;
			static uint32_t hex_to_int[256];
			static std::once_flag fl;
			std::call_once(
				fl,
				[]()
				{
					std::fill(hex_to_int, hex_to_int + 256, UINT32_MAX);
					for (uint32_t i = 0; i < 16; ++i)
					{
						hex_to_int[hex_digits[i]] = i;
					}
				});
			uint32_t d_num = hex_to_int[std::toupper(*d)];
			if (d_num == UINT32_MAX)
			{
				nanable_ = true;
				return;
			}
			b |= d_num;
		}
		data_.push(b);
		r = block_start;
	}
}

LN LN::nan()
{
	LN nan;
	nan.nanable_ = true;
	return nan;
}

Sign LN::invert_sign(Sign s)
{
	if (s == Sign::Plus)
	{
		return Sign::Minus;
	}
	return Sign::Plus;
}

LN operator""_ln(const char *s)
{
	return { s };
}