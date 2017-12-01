#include "filer.h"
#include "utils/utils.h"
#include <fstream>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

// INI LINE

IniLine::IniLine() :
	type(Type::av) 
{}

IniLine::IniLine(const string& ARG, const string& VAL) :
	type(Type::av),
	arg(ARG),
	val(VAL) 
{}

IniLine::IniLine(const string& ARG, const string& KEY, const string& VAL) :
	type(Type::akv),
	arg(ARG),
	key(KEY),
	val(VAL) 
{}

IniLine::IniLine(const string& TIT) :
	type(Type::title),
	arg(TIT)
{}

string IniLine::line() const {
	if (type == Type::av)
		return arg + '=' + val;
	if (type == Type::akv)
		return arg + '[' + key + "]=" + val;
	return '[' + arg + ']';
}

void IniLine::setVal(const string& ARG, const string& VAL) {
	type = Type::av;
	arg = ARG;
	key.clear();
	val = VAL;
}

void IniLine::setVal(const std::string& ARG, const std::string& KEY, const std::string& VAL) {
	type = Type::akv;
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

bool IniLine::setLine(const string& lin) {
	// check if title
	if (lin[0] == '[' && lin[lin.length()-1] == ']') {
		arg = lin.substr(1, lin.length()-2);
		type = Type::title;
		return true;
	}

	// find position of the = to split line into argument and value
	sizt i0;;
	if (!findChar(lin, '=', i0))
		return false;

	val = lin.substr(i0 + 1);
	string left = lin.substr(0, i0);

	// get key if availible
	sizt i1 = 0, i2 = 0;
	type = (findChar(left, '[', i1) && findChar(left, ']', i2)) ? Type::akv : Type::av;
	if (type == Type::akv && i1 < i2) {
		arg = lin.substr(0, i1);
		key = lin.substr(i1+1, i2-i1-1);
	} else
		arg = left;
	return true;
}

void IniLine::clear() {
	type = Type::av;
	arg.clear();
	val.clear();
	key.clear();
}

// FILER

const string Filer::dirExec = Filer::getDirExec();
#ifdef _WIN32
const vector<string> Filer::dirFonts = {Filer::dirExec, string(std::getenv("SystemDrive")) + "\\Windows\\Fonts\\"};
#else
const vector<string> Filer::dirFonts = {Filer::dirExec, "/usr/share/fonts/", string(std::getenv("HOME")) + "/.fonts/"};
#endif

Settings Filer::loadSettings() {
	Settings sets;
	vector<string> lines;
	if (!readTextFile(dirExec + Default::fileSettings, lines, false)) {	// if file not availible return default settings
		sets.initFont();
		return sets;
	}

	// read settings file's lines
	for (string& line : lines) {
		IniLine il;
		if (!il.setLine(line) || il.type == IniLine::Type::title)
			continue;

		if (il.arg == Default::iniKeywordFont)
			sets.font = il.val;
		else if (il.arg == Default::iniKeywordRenderer)
			sets.renderer = il.val;
		else if (il.arg == Default::iniKeywordMaximized)
			sets.maximized = stob(il.val);
		else if (il.arg == Default::iniKeywordFullscreen)
			sets.fullscreen = stob(il.val);
		else if (il.arg == Default::iniKeywordResolution)
			sets.setResolution(il.val);
		else if (il.arg == Default::iniKeywordViewport)
			sets.setViewport(il.val);
		else if (il.arg == Default::iniKeywordScrollSpeed)
			sets.scrollSpeed = stoi(il.val);
	}
	sets.initFont();
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
		IniLine(Default::iniKeywordScrollSpeed, to_string(sets.scrollSpeed)).line()
	};
	writeTextFile(dirExec + Default::fileSettings, lines);
}

vector<Function> Filer::loadUsers(map<string, double>& vars) {
	vector<Function> funcs;
	vector<string> lines;
	if (!readTextFile(dirExec + Default::fileUsers, lines, false))	// if file not availibe return empty vector
		return funcs;

	// read file's lines
	for (string& line : lines) {
		IniLine il;
		if (!il.setLine(line) || il.type == IniLine::Type::title)
			continue;

		if (il.arg == Default::iniKeywordVariable) {
			if (!vars.count(il.key))	// no variables with same name
				vars.insert(make_pair(il.key, stod(il.val)));
		} else if (il.arg == Default::iniKeywordFunction)
			funcs.push_back(Function(il.val));
	}
	return funcs;
}

void Filer::saveUsers(const vector<Function>& funcs, const map<string, double>& vars) {
	vector<string> lines;
	for (const pair<string, double>& it : vars)
		lines.push_back(IniLine(Default::iniKeywordVariable, it.first, to_string(it.second)).line());

	for (const Function& it : funcs)
		lines.push_back(IniLine(Default::iniKeywordFunction, btos(it.show) + ' ' + to_string(short(it.color.r)) + ' ' + to_string(short(it.color.g)) + ' ' + to_string(short(it.color.b)) + ' ' + to_string(short(it.color.a)) + ' ' + it.text).line());
	writeTextFile(dirExec + Default::fileUsers, lines);
}

bool Filer::readTextFile(const string& file, vector<string>& lines, bool printMessage) {
	std::ifstream ifs(file.c_str());
	if (!ifs.good()) {
		if (printMessage)
			cerr << "couldn't open file " << file << endl;
		return false;
	}
	lines.clear();

	for (string line; readLine(ifs, line);)
		if (!line.empty())		// skip empty lines
			lines.push_back(line);
	return true;
}

bool Filer::writeTextFile(const string& file, const vector<string>& lines) {
	std::ofstream ofs(file.c_str());
	if (!ofs.good()) {
		cerr << "couldn't write file " << file << endl;
		return false;
	}
	for (const string& line : lines)
		ofs << line << endl;
	return true;
}

vector<string> Filer::listDir(const string& dir, FileType filter, const vector<string>& extFilter) {
	vector<string> entries;
#ifdef _WIN32
	WIN32_FIND_DATAW data;
	HANDLE hFind = FindFirstFileW(stow(dir+"*").c_str(), &data);
	if (hFind == INVALID_HANDLE_VALUE)
		return {};
	do {
		if (data.cFileName == wstring(L".") || data.cFileName == wstring(L".."))
			continue;
		
		string name = wtos(data.cFileName);
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (filter & FTYPE_DIR)
				entries.push_back(name);
		} else if (data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
			if (filter & FTYPE_LINK)
				entries.push_back(name);
		} else if (filter & FTYPE_FILE) {
			if (extFilter.empty())
				entries.push_back(name);
			else
				for (const string& ext : extFilter)
					if (hasExtension(name, ext)) {
						entries.push_back(name);
						break;
					}
		}
	} while (FindNextFileW(hFind, &data) != 0);
	FindClose(hFind);
#else
	DIR* directory = opendir(dir.c_str());
	if (directory) {
		dirent* data = readdir(directory);
		while (data) {
			if (data->d_name == string(".") || data->d_name == string("..")) {
				data = readdir(directory);
				continue;
			}

			if (data->d_type == DT_DIR) {
				if (filter & FTYPE_DIR)
					entries.push_back(data->d_name);
			} else if (data->d_type == DT_LNK) {
				if (filter & FTYPE_LINK)
					entries.push_back(data->d_name);
			} else if (filter & FTYPE_FILE) {
				if (extFilter.empty())
					entries.push_back(data->d_name);
				else
					for (const string& ext : extFilter)
						if (hasExtension(data->d_name, ext)) {
							entries.push_back(data->d_name);
							break;
						}
			}
			data = readdir(directory);
		}
		closedir(directory);
	}
#endif
	std::sort(entries.begin(), entries.end());
	return entries;
}

vector<string> Filer::listDirRecursively(const string& dir, sizt offs) {
	// I wrote this a long time ago and am too lazy to figure out how and why it works which is why there are no comments
	if (offs == 0)
		offs = dir.length();
	vector<string> entries;
#ifdef _WIN32
	WIN32_FIND_DATAW data;
	HANDLE hFind = FindFirstFileW(stow(dir+"*").c_str(), &data);
	if (hFind == INVALID_HANDLE_VALUE)
		return {};
	do {
		if (data.cFileName == wstring(L".") || data.cFileName == wstring(L".."))
			continue;

		string name = wtos(data.cFileName);
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			vector<string> newEs = listDirRecursively(dir+name+dsep, offs);
			std::sort(entries.begin(), entries.end());
			entries.insert(entries.end(), newEs.begin(), newEs.end());
		} else
			entries.push_back(dir.substr(offs) + name);
	} while (FindNextFileW(hFind, &data) != 0);
	FindClose(hFind);
#else
	DIR* directory = opendir(dir.c_str());
	if (directory) {
		dirent* data = readdir(directory);
		while (data) {
			if (data->d_name == string(".") || data->d_name == string("..")) {
				data = readdir(directory);
				continue;
			}

			if (data->d_type == DT_DIR) {
				vector<string> newEs = listDirRecursively(dir+data->d_name+dsep, offs);
				std::sort(entries.begin(), entries.end());
				entries.insert(entries.end(), newEs.begin(), newEs.end());
			} else
				entries.push_back(dir.substr(offs) + data->d_name);

			data = readdir(directory);
		}
		closedir(directory);
	}
#endif
	return entries;
}

FileType Filer::fileType(const string& path) {
#ifdef _WIN32
	DWORD attrib = GetFileAttributesW(stow(path).c_str());
	if (attrib & FILE_ATTRIBUTE_DIRECTORY)
		return FTYPE_DIR;
	if (attrib & FILE_ATTRIBUTE_SPARSE_FILE)
		return FTYPE_LINK;
#else
	struct stat ps;
	stat(path.c_str(), &ps);
	if (S_ISDIR(ps.st_mode))
		return FTYPE_DIR;
	if (S_ISLNK(ps.st_mode))
		return FTYPE_LINK;
#endif
	return FTYPE_FILE;
}

bool Filer::fileExists(const string& path) {
#ifdef _WIN32
	return GetFileAttributesW(stow(path).c_str()) != INVALID_FILE_ATTRIBUTES;
#else
	struct stat ps;
	return stat(path.c_str(), &ps) == 0;
#endif
}

#ifdef _WIN32
vector<char> Filer::listDrives() {
	vector<char> letters;
	DWORD drives = GetLogicalDrives();
	
	for (char i=0; i<26; i++)
		if (drives & (1 << i))
			letters.push_back('A'+i);
	return letters;
}
#endif

string Filer::getDirExec() {
	string path;
#ifdef _WIN32
	for (DWORD blen=4096; blen<=4096*1024; blen*=4) {
		wchar* buffer = new wchar[blen];
		if (GetModuleFileNameW(0, buffer, blen) != blen) {
			path = wtos(buffer);
			delete[] buffer;
			break;
		}
		delete[] buffer;
	}
#else
	for (sizt blen=4096; blen<=4096*1024; blen*=4) {
		char* buffer = new char[blen];
		ssize_t len = readlink("/proc/self/exe", buffer, blen-1);
		if (len != blen-1 && len != -1) {
			buffer[len] = '\0';
			path = buffer;
			delete[] buffer;
			break;
		}
		delete[] buffer;
	}
#endif
	return path.empty() ? path : parentDir(path);
}

string Filer::findFont(const string& font) {
	if (isAbsolute(font)) {	// check fontpath first
		if (fileType(font) == FTYPE_FILE)
			return font;
		return checkDirForFont(filename(font), parentDir(font));
	}

	for (const string& dir : dirFonts) {	// check global font directories
		string file = checkDirForFont(font, dir);
		if (!file.empty())
			return file;
	}
	return "";	// nothing found
}

string Filer::checkDirForFont(const string& font, const string& dir) {
	for (string& it : listDirRecursively(dir)) {
		string file = findChar(font, '.') ? filename(it) : delExtension(filename(it));
		if (strcmpCI(file, font))
			return dir + it;
	}
	return "";
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
