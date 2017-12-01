#pragma once

#include "utils/defaults.h"

class Subfunction {
public:
	virtual double solve() const = 0;
};

class SubfunctionF1 : public Subfunction {
public:
	SubfunctionF1(mf1ptr a=nullptr, Subfunction* b=nullptr);

	virtual double solve() const;

private:
	mf1ptr mf1;
	uptr<Subfunction> func;
};

class SubfunctionF2 : public Subfunction {
public:
	SubfunctionF2(mf2ptr a=nullptr, Subfunction* b=nullptr, Subfunction* c=nullptr);

	virtual double solve() const;

private:
	mf2ptr mf2;
	uptr<Subfunction> funcL, funcR;
};

class SubfunctionNum : public Subfunction {
public:
	SubfunctionNum(double a=0.0);

	virtual double solve() const;

private:
	double num;
};

class SubfunctionVar : public Subfunction {
public:
	SubfunctionVar(const string& a="");

	virtual double solve() const;

private:
	string var;
};

class Function {
public:
	Function(bool SHW=true, const string& TXT="", SDL_Color CLR=Default::colorGraph);
	Function(const string& line);

	bool visible() const { return show && func; }
	void set(const string& line);
	bool setFunc();
	void clear();
	double solve(double x) const;

	bool show;
	SDL_Color color;
	string text;			// function text used to create func
private:
	Subfunction* func;		// function tree used to calculate y
};
