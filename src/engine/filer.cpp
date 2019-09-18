#include "filer.h"
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;

// SETTINGS

Settings::Settings(bool MAX, bool FSC, const vec2i& RES, const vec2f& VPS, const vec2f& VSZ, const string& FNT, const string& RND, int SSP) :
	maximized(MAX),
	fullscreen(FSC),
	resolution(RES),
	viewPos(VPS),
	viewSize(VSZ),
	font(FNT),
	renderer(RND),
	scrollSpeed(SSP)
{}

string Settings::getResolutionString() const {
	ostringstream ss;
	ss << resolution.x << ' ' << resolution.y;
	return ss.str();
}

void Settings::setResolution(const string& line) {
	vector<vec2t> elems = getWords(line);
	if (elems.size() >= 1)
		resolution.x = stoi(line.substr(elems[0].l, elems[0].u));
	if (elems.size() >= 2)
		resolution.y = stoi(line.substr(elems[1].l, elems[1].u));
}

string Settings::getViewportString() const {
	ostringstream ss;
	ss << viewPos.x << ' ' << viewPos.y << ' ' << viewPos.x + viewSize.x << ' ' << viewPos.y + viewSize.y;
	return ss.str();
}

void Settings::setViewport(const string& line) {
	vector<vec2t> elems = getWords(line);
	if (elems.size() >= 1)
		viewPos.x = stof(line.substr(elems[0].l, elems[0].u));
	if (elems.size() >= 2)
		viewPos.y = stof(line.substr(elems[1].l, elems[1].u));
	if (elems.size() >= 3)
		viewSize.x = stof(line.substr(elems[2].l, elems[2].u)) - viewPos.x;
	if (elems.size() >= 4)
		viewSize.y = stof(line.substr(elems[3].l, elems[3].u)) - viewPos.y;
}

int Settings::getRendererIndex() {
	// get index of currently selected renderer
	for (int i=0; i<SDL_GetNumRenderDrivers(); i++)
		if (getRendererName(i) == renderer)
			return i;

	// if name doesn't match, choose the first renderer
	renderer = getRendererName(0);
	return 0;
}

// INI LINE

IniLine::IniLine() :
	type(Type::empty)
{}

IniLine::IniLine(const string& ARG, const string& VAL) :
	type(Type::argVal),
	arg(ARG),
	val(VAL)
{}

IniLine::IniLine(const string& ARG, const string& KEY, const string& VAL) :
	type(Type::argKeyVal),
	arg(ARG),
	key(KEY),
	val(VAL)
{}

IniLine::IniLine(const string& TIT) :
	type(Type::title),
	arg(TIT)
{}

string IniLine::line() const {
	if (type == Type::argVal)
		return arg + '=' + val;
	if (type == Type::argKeyVal)
		return arg + '[' + key + "]=" + val;
	if (type == Type::title)
		return '[' + arg + ']';
	return "";
}

void IniLine::setVal(const string& ARG, const string& VAL) {
	type = Type::argVal;
	arg = ARG;
	key.clear();
	val = VAL;
}

void IniLine::setVal(const string& ARG, const string& KEY, const string& VAL) {
	type = Type::argKeyVal;
	arg = ARG;
	key = KEY;
	val = VAL;
}

void IniLine::setTitle(const string& TIT) {
	type = Type::title;
	arg = TIT;
	key.clear();
	val.clear();
}

bool IniLine::setLine(const string& str) {
	// clear line in case the function will return false
	clear();
	if (str.empty())
		return false;

	// check if title
	if (str[0] == '[' && str.back() == ']') {
		arg = str.substr(1, str.length()-2);
		type = Type::title;
		return true;
	}

	// find position of the '=' to split line into argument and value
	sizt i0 = str.find_first_of('=');
	if (i0 == string::npos)
		return false;
	val = str.substr(i0+1);

	// get arg and key if availible
	sizt i1 = str.find_first_of('[');
	sizt i2 = str.find_first_of(']', i1);
	if (i1 < i2 && i2 < i0) {	// if '[' preceeds ']' and both preceed '='
		arg = str.substr(0, i1);
		key = str.substr(i1+1, i2-i1-1);
		type = Type::argKeyVal;
	} else {
		arg = str.substr(0, i0);
		type = Type::argVal;
	}
	return true;
}

void IniLine::clear() {
	type = Type::empty;
	arg.clear();
	val.clear();
	key.clear();
}

// FILER

string Filer::dirExec;
vector<string> Filer::dirFonts;

void Filer::init() {
	if (char* path = SDL_GetBasePath()) {
		dirExec = path;
		SDL_free(path);
	}
#ifdef _WIN32
	dirFonts = {Filer::dirExec, string(std::getenv("SystemDrive")) + "\\Windows\\Fonts\\"};
#else
	dirFonts = {Filer::dirExec, "/usr/share/fonts/", string(std::getenv("HOME")) + "/.fonts/"};
#endif
}

Settings Filer::loadSettings() {
	Settings sets;
	vector<string> lines;
	if (!readTextFile(dirExec + Default::fileSettings, lines))	// if file not readable return default settings
		return sets;

	// read settings file's lines
	for (string& line : lines) {
		IniLine il;
		if (!il.setLine(line) || il.getType() == IniLine::Type::title)
			continue;

		if (il.getArg() == Default::iniKeywordFont)
			sets.font = il.getVal();
		else if (il.getArg() == Default::iniKeywordRenderer)
			sets.renderer = il.getVal();
		else if (il.getArg() == Default::iniKeywordMaximized)
			sets.maximized = stob(il.getVal());
		else if (il.getArg() == Default::iniKeywordFullscreen)
			sets.fullscreen = stob(il.getVal());
		else if (il.getArg() == Default::iniKeywordResolution)
			sets.setResolution(il.getVal());
		else if (il.getArg() == Default::iniKeywordViewport)
			sets.setViewport(il.getVal());
		else if (il.getArg() == Default::iniKeywordScrollSpeed)
			sets.scrollSpeed = stoi(il.getVal());
	}
	return sets;
}

void Filer::saveSettings(const Settings& sets) {
	vector<string> lines = {
		IniLine(Default::iniKeywordFont, sets.font).line(),
		IniLine(Default::iniKeywordRenderer, sets.renderer).line(),
		IniLine(Default::iniKeywordMaximized, btos(sets.maximized)).line(),
		IniLine(Default::iniKeywordFullscreen, btos(sets.fullscreen)).line(),
		IniLine(Default::iniKeywordResolution, sets.getResolutionString()).line(),
		IniLine(Default::iniKeywordViewport, sets.getViewportString()).line(),
		IniLine(Default::iniKeywordScrollSpeed, ntos(sets.scrollSpeed)).line()
	};
	writeTextFile(dirExec + Default::fileSettings, lines);
}

vector<Function> Filer::loadUsers(map<string, double>& vars) {
	vector<Function> funcs;
	vector<string> lines;
	if (!readTextFile(dirExec + Default::fileUsers, lines))	// if file not readable return empty vector
		return funcs;

	// read file's lines
	for (string& line : lines) {
		IniLine il;
		if (!il.setLine(line) || il.getType() == IniLine::Type::title)
			continue;

		if (il.getArg() == Default::iniKeywordVariable) {
			if (!vars.count(il.getKey()))	// no variables with same name
				vars.insert(make_pair(il.getKey(), stod(il.getVal())));
		} else if (il.getArg() == Default::iniKeywordFunction)
			funcs.push_back(Function(il.getVal()));
	}
	return funcs;
}

void Filer::saveUsers(const vector<Function>& funcs, const map<string, double>& vars) {
	vector<string> lines;
	for (const pair<const string, double>& it : vars)
		lines.push_back(IniLine(Default::iniKeywordVariable, it.first, ntos(it.second)).line());

	for (const Function& it : funcs) {
		ostringstream ss;
		ss << btos(it.show) << ' ' << short(it.color.r) << ' ' << short(it.color.g) << ' ' << short(it.color.b) << ' ' << short(it.color.a) << ' ' << it.text;
		lines.push_back(IniLine(Default::iniKeywordFunction, ss.str()).line());
	}
	writeTextFile(dirExec + Default::fileUsers, lines);
}

bool Filer::readTextFile(const string& file, vector<string>& lines) {
	lines.clear();
	std::ifstream ifs(file.c_str());
	if (!ifs.good())
		return false;

	for (string line; readLine(ifs, line);)
		lines.push_back(line);
	return true;
}

bool Filer::writeTextFile(const string& file, const vector<string>& lines) {
	std::ofstream ofs(file.c_str());
	if (!ofs.good()) {
		cerr << "Couldn't write file " << file << endl;
		return false;
	}

	for (const string& line : lines)
		ofs << line << endl;
	return true;
}

string Filer::findFont(const string& font) {
	if (isAbsolute(font) && fs::is_regular_file(font))	// check if font refers to a file
		return font;

	for (const string& dir : dirFonts) {	// check font directories
		try {
			for (fs::recursive_directory_iterator it(dir, fs::directory_options::follow_directory_symlink | fs::directory_options::skip_permission_denied), end; it != end; it++)
				if (it->is_regular_file())
					if (string name = it->path().filename().string(); strcmpCI(string(name.begin(), name.end() - pdft(it->path().extension().string().length())), font))
						return it->path().string();
		} catch (const fs::filesystem_error& e) {
			cerr << e.what() << endl;
		}
	}
	return "";	// nothing found
}

std::istream& Filer::readLine(std::istream& ifs, string& str) {
	str.clear();
	std::istream::sentry(ifs, true);
	std::streambuf* sbf = ifs.rdbuf();

	while (true) {
		int c = sbf->sbumpc();
		if (c == '\n')
			return ifs;
		else if (c == '\r') {
			if (sbf->sgetc() == '\n')
				sbf->sbumpc();
			return ifs;
		} else if (c == EOF) {
			if (str.empty())
				ifs.setstate(std::ios::eofbit);
			return ifs;
		} else
			str += char(c);
	}
}
