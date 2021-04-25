#pragma once

#include <iostream>
#include <vector>
#include <string>

static constexpr size_t MAX_NAIVE = 128;
static constexpr size_t divDigits = 1000;

class long_d {
public:
	long_d() noexcept;
	long_d(long double d) noexcept;
	long_d(const std::string& s);
	long_d(const long_d& other) noexcept;
	long_d(long_d&& other) noexcept;
	long_d& operator=(const long_d& other) noexcept;
	long_d& operator=(long_d&& other) noexcept;
	long_d operator-() const noexcept;
	friend bool operator<(const long_d& lhs, const long_d& rhs) noexcept;
	friend bool operator>(const long_d& lhs, const long_d& rhs) noexcept;
	friend bool operator<=(const long_d& lhs, const long_d& rhs) noexcept;
	friend bool operator>=(const long_d& lhs, const long_d& rhs) noexcept;
	friend bool operator==(const long_d& lhs, const long_d& rhs) noexcept;
	friend bool operator!=(const long_d& lhs, const long_d& rhs) noexcept;
	friend long_d operator+(const long_d& lhs, const long_d& rhs) noexcept;
	friend long_d operator-(const long_d& lhs, const long_d& rhs) noexcept;
	friend long_d operator*(const long_d& lhs, const long_d& rhs) noexcept;
	friend long_d operator/(const long_d& lhs, const long_d& rhs);
	friend std::ostream& operator<<(std::ostream& os, const long_d& ld) noexcept;
	friend std::istream& operator>>(std::istream& is, long_d& ld);
	void swap(long_d& other) noexcept;
	long_d inverse() const;//returns 1/*this
private:
	std::vector<int> digits;
	bool sign;//0(false) for positive, 1(true) for negative
	int32_t exponent;
	void DeleteZeroes() noexcept;
};

std::vector<int> naive_mul(const std::vector<int>& lhs, const std::vector<int>& rhs) noexcept;
std::vector<int> karatsuba_mul(const std::vector<int>& lhs, const std::vector<int>& rhs) noexcept;
long_d abs(const long_d& ld) noexcept;