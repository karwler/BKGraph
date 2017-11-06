#pragma once

template <typename T>
struct vec2 {
	vec2(const T& N=T(0)) :
		x(N), y(N)
	{}
	vec2(const T& X, const T& Y) :
		x(X), y(Y)
	{}
	template <typename A>
	vec2(const vec2<A>& V) :
		x(V.x), y(V.y)
	{}

	T& operator[](char i) {
		if (i == 0)
			return x;
		return y;
	}
	const T& operator[](char i) const {
		if (i == 0)
			return x;
		return y;
	}

	vec2& operator=(const vec2& v) {
		x = v.x;
		y = v.y;
		return *this;
	}
	vec2& operator+=(const vec2& v) {
		x += v.x;
		y += v.y;
		return *this;
	}
	vec2& operator-=(const vec2& v) {
		x -= v.x;
		y -= v.y;
		return *this;
	}
	vec2& operator*=(const vec2& v) {
		x *= v.x;
		y *= v.y;
		return *this;
	}
	vec2& operator/=(const vec2& v) {
		x /= v.x;
		y /= v.y;
		return *this;
	}

	bool isNull() const {
		return x == T(0) && y == T(0);
	}
	bool hasNull() const {
		return x == T(0) || y == T(0);
	}

	union { T x, w; };
	union { T y, h; };
};

template <typename A, typename B>
vec2<A> operator+(const vec2<A>& a, const vec2<B>& b) {
	return vec2<A>(a.x + b.x, a.y + b.y);
}
template <typename A, typename B>
vec2<A> operator+(const vec2<A>& a, const B& b) {
	return vec2<A>(a.x + b, a.y + b);
}
template <typename A, typename B>
vec2<A> operator+(const A& a, const vec2<B>& b) {
	return vec2<A>(a + b.x, a + b.y);
}

template <typename A, typename B>
vec2<A> operator-(const vec2<A>& a, const vec2<B>& b) {
	return vec2<A>(a.x - b.x, a.y - b.y);
}
template <typename A, typename B>
vec2<A> operator-(const vec2<A>& a, const B& b) {
	return vec2<A>(a.x - b, a.y - b);
}
template <typename A, typename B>
vec2<A> operator-(const A& a, const vec2<B>& b) {
	return vec2<A>(a - b.x, a - b.y);
}

template <typename A, typename B>
vec2<A> operator*(const vec2<A>& a, const vec2<B>& b) {
	return vec2<A>(a.x * b.x, a.y * b.y);
}
template <typename A, typename B>
vec2<A> operator*(const vec2<A>& a, const B& b) {
	return vec2<A>(a.x * b, a.y * b);
}
template <typename A, typename B>
vec2<A> operator*(const A& a, const vec2<B>& b) {
	return vec2<A>(a * b.x, a * b.y);
}

template <typename A, typename B>
vec2<A> operator/(const vec2<A>& a, const vec2<B>& b) {
	return vec2<A>(a.x / b.x, a.y / b.y);
}
template <typename A, typename B>
vec2<A> operator/(const vec2<A>& a, const B& b) {
	return vec2<A>(a.x / b, a.y / b);
}
template <typename A, typename B>
vec2<A> operator/(const A& a, const vec2<B>& b) {
	return vec2<A>(a / b.x, a / b.y);
}

template <typename A, typename B>
bool operator==(const vec2<A>& a, const vec2<B>& b) {
	return a.x == b.x && a.y == b.y;
}
template <typename A, typename B>
bool operator==(const vec2<A>& a, const B& b) {
	return a.x == b && a.y == b;
}
template <typename A, typename B>
bool operator==(const A& a, const vec2<B>& b) {
	return a == b.x && a == b.y;
}

template <typename A, typename B>
bool operator!=(const vec2<A>& a, const vec2<B>& b) {
	return a.x != b.x || a.y != b.y;
}
template <typename A, typename B>
bool operator!=(const vec2<A>& a, const B& b) {
	return a.x != b || a.y != b;
}
template <typename A, typename B>
bool operator!=(const A& a, const vec2<B>& b) {
	return a != b.x || a != b.y;
}
