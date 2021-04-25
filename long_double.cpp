#include "long_double.h"

#include <vector>
#include <deque>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <stdexcept>

using namespace std;

long_d::long_d() noexcept : digits(1), sign(0), exponent(1) {}

void long_d::DeleteZeroes() noexcept {
	size_t n = max(1, exponent);
	while (digits.size() > n && !digits.back()) {
		digits.pop_back();
	}
	size_t i = 0;
	for (i; i < digits.size() && !digits[i]; ++i);
	digits.erase(digits.begin(), digits.begin() + i);
	exponent -= i;
	if (digits.empty() || digits.size() == 1 && !digits[0]) {
		digits.resize(1);
		exponent = 1;
		sign = 0;
	}
}

long_d::long_d(const string& s) {
	size_t i = 0;
	sign = s[0] == '-' ? ++i : 0;
	exponent = s.size() - i;
	digits.reserve(exponent);
	while (i < s.size()) {
		if (s[i] == '.') {
			exponent = sign ? i - 1 : i;
		}
		else if (isdigit(s[i])) {
			digits.push_back(s[i] - '0');
		}
		else {
			throw invalid_argument("String must contain only digits and '.'");
		}
		++i;
	}
	DeleteZeroes();
}

long_d::long_d(long double d) noexcept {
	ostringstream os;
	os << setprecision(15) << d;
	*this = long_d(os.str());
}

void long_d::swap(long_d& other) noexcept {
	std::swap(digits, other.digits);
	std::swap(exponent, other.exponent);
	std::swap(sign, other.sign);
}

long_d::long_d(const long_d& other) noexcept
	: digits(other.digits), sign(other.sign), exponent(other.exponent) {}

long_d::long_d(long_d&& other) noexcept
	: digits(move(other.digits)), sign(other.sign), exponent(other.exponent) {
	other.sign = 0;
	other.exponent = 1;
}

long_d& long_d::operator=(const long_d& other) noexcept {
	if (this == &other) {
		return *this;
	}
	sign = other.sign;
	exponent = other.exponent;
	digits = other.digits;
	return *this;
}

long_d& long_d::operator=(long_d&& other) noexcept {
	if (this == &other) {
		return *this;
	}
	sign = other.sign;
	exponent = other.exponent;
	digits = move(other.digits);
	other.sign = 0;
	other.exponent = 1;
	return *this;
} 

ostream& operator<<(ostream& os, const long_d& ld) noexcept {
	if (ld.sign) {
		os << '-';
	}
	if (ld.exponent > 0) {
		size_t i = 0;
		size_t e = ld.exponent;
		while (i < ld.digits.size() && i < e) {
			os << ld.digits[i++];
		}
		while (i < e) {
			os << "0";
			++i;
		}
		if (i < ld.digits.size()) {
			os << ".";
			while (i < ld.digits.size()) {
				os << ld.digits[i++];
			}
		}
	}
	else if (!ld.exponent) {
		os << "0.";
		for (size_t i = 0; i < ld.digits.size(); ++i)
			os << ld.digits[i];
	}
	else {
		os << "0.";
		size_t exp = -ld.exponent;
		for (size_t i = 0; i < exp; ++i) {
			os << "0";
		}
		for (size_t i = 0; i < ld.digits.size(); ++i) {
			os << ld.digits[i];
		}
	}
	return os;
}

istream& operator>>(istream& is, long_d& ld) {
	string s;
	is >> s;
	ld = long_d(s);
	return is;
}

bool operator<(const long_d& lhs, const long_d& rhs) noexcept {
	if (lhs.sign != rhs.sign) {
		return lhs.sign > rhs.sign; 
	}
	if (lhs.exponent != rhs.exponent) {
		return (lhs.exponent < rhs.exponent) ^ lhs.sign; 
	}
	return (lhs.digits < rhs.digits) ^ lhs.sign;
}

bool operator>(const long_d& lhs, const long_d& rhs) noexcept {
	return lhs != rhs && !(lhs < rhs);
}

bool operator==(const long_d& lhs, const long_d& rhs) noexcept {
	return lhs.sign == rhs.sign 
		&& lhs.exponent == rhs.exponent 
		&& lhs.digits == rhs.digits;
}

bool operator<=(const long_d& lhs, const long_d& rhs) noexcept {
	return !(lhs > rhs);
}
bool operator>=(const long_d& lhs, const long_d& rhs) noexcept {
	return !(lhs < rhs);
}

bool operator!=(const long_d& lhs, const long_d& rhs) noexcept {
	return !(lhs == rhs);
}

long_d long_d::operator-() const noexcept {
	long_d res = *this; 
	res.sign = !sign; 
	return res; 
}

long_d operator+(const long_d& lhs, const long_d& rhs) noexcept {
	long_d result;
	if (lhs.sign == rhs.sign) { 
		auto exp1 = lhs.exponent; 
		auto exp2 = rhs.exponent; 
		auto exp = max(exp1, exp2);
		deque<int> d1(lhs.digits.begin(), lhs.digits.end());
		deque<int> d2(rhs.digits.begin(), rhs.digits.end());
		while (exp1 != exp) {
			d1.push_front(0); 
			++exp1;
		}
		while (exp2 != exp) {
			d2.push_front(0);
			++exp2;
		}
		size_t size = max(d1.size(), d2.size());
		while (d1.size() != size) {
			d1.push_back(0);
		}
		while (d2.size() != size) {
			d2.push_back(0);
		}
		size_t len = 1 + size;
		result.sign = lhs.sign;
		result.exponent = exp + 1;
		result.digits = vector<int>(len, 0);
		for (size_t i = 0; i < size; ++i) {
			result.digits[i + 1] = d1[i] + d2[i];
		}
		for (size_t i = len - 1; i > 0; --i) {
			result.digits[i - 1] += result.digits[i] / 10;
			result.digits[i] %= 10;
		}
		result.DeleteZeroes();
	}
	else if (lhs.sign) {
		result = rhs - (-(lhs));
	}
	else {
		result = lhs - (-rhs);
	}
	return result;
}

long_d operator-(const long_d& lhs, const long_d& rhs) noexcept {
	long_d result;
	if (!lhs.sign && !rhs.sign) {
		bool first_is_bigger = lhs > rhs;
		auto exp1 = first_is_bigger ? lhs.exponent : rhs.exponent;
		auto exp2 = first_is_bigger ? rhs.exponent : lhs.exponent;
		auto exp = max(exp1, exp2); 
		deque<int> d1, d2;
		if (first_is_bigger) {
			d1 = { lhs.digits.begin(), lhs.digits.end() };
			d2 = { rhs.digits.begin(), rhs.digits.end() };
		}
		else {
			d1 = { rhs.digits.begin(), rhs.digits.end() };
			d2 = { lhs.digits.begin(), lhs.digits.end() };
		}
		while (exp1 != exp) {
			d1.push_front(0);
			++exp1;
		}
		while (exp2 != exp) {
			d2.push_front(0);
			++exp2;
		}
		size_t size = max(d1.size(), d2.size());
		while (d1.size() != size) {
			d1.push_back(0);
		}
		while (d2.size() != size) {
			d2.push_back(0);
		}
		size_t len = 1 + size;
		result.sign = first_is_bigger ? 0 : 1;
		result.exponent = exp + 1;
		result.digits = vector<int>(len, 0);
		for (size_t i = 0; i < size; ++i) {
			result.digits[i + 1] = d1[i] - d2[i]; 
		}
		for (size_t i = len - 1; i; --i) {
			if (result.digits[i] < 0) { 
				result.digits[i] += 10; 
				--result.digits[i - 1]; 
			}
		}
		result.DeleteZeroes(); 
	}
	else if (lhs.sign && rhs.sign) {
		result = (-rhs) - (-(lhs)); 
	}
	else {
		result = lhs + (-rhs);
	}
	return result;
}

long_d operator*(const long_d& lhs, const long_d& rhs) noexcept {
	long_d res;
	res.sign = lhs.sign ^ rhs.sign;
	res.exponent = lhs.exponent + rhs.exponent;
	if ((lhs.digits.size() + rhs.digits.size()) < MAX_NAIVE) {
		res.digits = naive_mul(lhs.digits, rhs.digits);
	}
	else {
		res.digits = karatsuba_mul(lhs.digits, rhs.digits);
	}
	for (size_t i = res.digits.size() - 1; i > 0; --i) {
		res.digits[i - 1] += res.digits[i] / 10;
		res.digits[i] %= 10;
	}
	res.DeleteZeroes();
	return res;
}

vector<int> naive_mul(const vector<int>& lhs, const vector<int>& rhs) noexcept {
	vector<int> res(lhs.size() + rhs.size());
	for (size_t i = 0; i < lhs.size(); ++i) {
		for (size_t j = 0; j < rhs.size(); ++j) {
			res[i + j + 1] += lhs[i] * rhs[j];
		}
	}
	return res;
}

vector<int> karatsuba_mul(const vector<int>& lhs, const vector<int>& rhs) noexcept {
	size_t old_size = max(lhs.size(), rhs.size());
	vector<int> result;
	if ((old_size << 1) < MAX_NAIVE) {
		result = naive_mul(lhs, rhs);
		return result;
	}
	size_t lz = 0;
	size_t rz = 0;
	size_t new_size = old_size;
	while (new_size & (new_size - 1)) {
		++new_size;
	}
	vector<int> left(new_size, 0), right(new_size, 0);
	for (size_t i = 0; i < lhs.size(); ++i) {
		left[i + new_size - lhs.size()] = lhs[i];
	}
	for (size_t i = 0; i < rhs.size(); ++i) {
		right[i + new_size - rhs.size()] = rhs[i];
	}
	size_t k = new_size >> 1;
	vector<int> a{ left.begin(), left.begin() + k };
	vector<int> b{ left.begin() + k, left.end() };
	vector<int> c{ right.begin(), right.begin() + k };
	vector<int> d{ right.begin() + k, right.end() };
	vector<int> ac = karatsuba_mul(a, c);
	vector<int> bd = karatsuba_mul(b, d);
	for (size_t i = 0; i < k; ++i) {
		a[i] += b[i];
		c[i] += d[i];
	}
	a = karatsuba_mul(a, c);
	for (size_t i = 0; i < new_size; ++i) {
		a[i] -= ac[i] + bd[i];
	}
	result.resize(2 * new_size);
	for (size_t i = 0; i < new_size; ++i) {
		result[new_size + i] = bd[i];
	}
	for (size_t i = 0; i < new_size; ++i) {
		result[i] = ac[i];
	}
	for (size_t i = k; i < new_size + k; ++i) {
		result[i] += a[i - k];
	}
	result.erase(result.begin(), result.begin() + new_size - lhs.size() + new_size - rhs.size());
	return result;
}

long_d abs(const long_d& ld) noexcept {
	long_d res;
	res = ld < 0 ? -ld : ld;
	return res;
}

long_d long_d::inverse() const {
	long_d res; 
	if (digits.size() == 1 && !digits[0]) {
		throw invalid_argument("Division by zero"); 
	}
	long_d x = *this;
	x.sign = 0;
	long_d one("1"); 
	res.sign = sign; 
	res.exponent = 1;
	res.digits.clear();
	while (x < one) {
		++x.exponent;
		++res.exponent; 
	}
	while (one < x) {
		one.exponent++;
	}
	res.exponent -= one.exponent - 1;
	size_t totalNumbers = divDigits + max(0, res.exponent); 
	size_t numbers = 0; 
	do {
		int div = 0; 
		while (!(one < x)) {
			++div;
			one = one - x;
		}
		++one.exponent;
		one.DeleteZeroes();
		res.digits.push_back(div);
		++numbers;
	} while (one != 0 && numbers < totalNumbers); 

	return res;
}

long_d operator/(const long_d& lhs, const long_d& rhs) {
	long_d res;
	if (lhs == 0) {
		return lhs;
	} 
	res = lhs * rhs.inverse();
	size_t intPart = max(0, lhs.exponent);
	if (intPart > res.digits.size() - 1) {
		return res;
	}
	size_t i = res.digits.size() - 1 - intPart;
	size_t n = max(0, res.exponent);
	if (i > n && res.digits[i] == 9) {
		while (i > n && res.digits[i] == 9) {
			i--;
		}
		if (res.digits[i] == 9) {
			res.digits.erase(res.digits.begin() + n, res.digits.end());
			res = res + (res.sign ? -1 : 1);
		}
		else {
			res.digits.erase(res.digits.begin() + i + 1, res.digits.end());
			res.digits[i]++;
		}
	}
	return res;
}