#pragma once

#include "utils/functions.h"
#include "utils/utils.h"

enum FileType : uint8 {
	FTYPE_NONE = 0x0,
	FTYPE_FILE = 0x1,
	FTYPE_DIR  = 0x2,
	FTYPE_ANY  = 0xFF
};
inline FileType operator~(FileType a) { return static_cast<FileType>(~static_cast<uint8>(a)); }
inline FileType operator&(FileType a, FileType b) { return static_cast<FileType>(static_cast<uint8>(a) & static_cast<uint8>(b)); }
inline FileType operator&=(FileType& a, FileType b) { return a = static_cast<FileType>(static_cast<uint8>(a) & static_cast<uint8>(b)); }
inline FileType operator^(FileType a, FileType b) { return static_cast<FileType>(static_cast<uint8>(a) ^ static_cast<uint8>(b)); }
inline FileType operator^=(FileType& a, FileType b) { return a = static_cast<FileType>(static_cast<uint8>(a) ^ static_cast<uint8>(b)); }
inline FileType operator|(FileType a, FileType b) { return static_cast<FileType>(static_cast<uint8>(a) | static_cast<uint8>(b)); }
inline FileType operator|=(FileType& a, FileType b) { return a = static_cast<FileType>(static_cast<uint8>(a) | static_cast<uint8>(b)); }

// collection of values that are gonna be saved to a file
class Settings {
public:
	Settings(bool MAX=Default::maximized, bool FSC=Default::fullscreen, const vec2i& RES=Default::resolution, const vec2f& VPS=Default::viewportPosition, const vec2f& VSZ=Default::viewportSize, const string& FNT=Default::font, const string& RND="", int SSP=Default::scrollSpeed);

	string getResolutionString() const;
	void setResolution(const string& line);
	string getViewportString() const;
	void setViewport(const string& line);
	int getRendererIndex();		// sets renderer name to first found renderer if no matching name was found

	bool maximized, fullscreen;
	vec2i resolution;
	vec2f viewPos, viewSize;
	string font;
	string renderer;
	int scrollSpeed;
};

// for interpreting lines in ini files
class IniLine {
public:
	enum class Type : uint8 {
		empty,
		argVal,		// argument, value, no key, no title
		argKeyVal,	// argument, key, value, no title
		title		// title, no everything else
	};

	IniLine();
	IniLine(const string& ARG, const string& VAL);
	IniLine(const string& ARG, const string& KEY, const string& VAL);
	IniLine(const string& TIT);

	Type getType() const { return type; }
	const string& getArg() const { return arg; }
	const string& getKey() const { return key; }
	const string& getVal() const { return val; }
	string line() const;	// get the actual INI line from arg, key and val

	void setVal(const string& ARG, const string& VAL);
	void setVal(const string& ARG, const string& KEY, const string& VAL);
	void setTitle(const string& TIT);
	bool setLine(const string& str);	// returns false if not an INI line
	void clear();

private:
	Type type;
	string arg;	// argument, aka. the thing before the equal sign/brackets
	string key;	// the thing between the brackets (empty if there are no brackets)
	string val;	// value, aka. the thing after the equal sign
};

// handles all filesystem interactions
class Filer {
public:
	static Settings loadSettings();	// read settings file
	static void saveSettings(const Settings& sets);	// write settings file
	static vector<Function> loadUsers(map<string, double>& vars);	// read functinos and variables from file
	static void saveUsers(const vector<Function>& funcs, const map<string, double>& vars);	// write functions and variables to file

	static bool readTextFile(const string& file, vector<string>& lines);	// returns true on success
	static bool writeTextFile(const string& file, const vector<string>& lines);	// returns true on success
	static vector<string> listDir(const string& dir, FileType filter=FTYPE_ANY);
	static vector<string> listDirRecursively(string dir);
	static FileType fileType(const string& path);

	static string findFont(const string& font);	// on success returns absolute path to font file, otherwise returns empty path
	
	static const string dirExec;	// directory in which the executable should currently be
	static const vector<string> dirFonts;	// os's font directories

private:
	static string getDirExec();		// for setting dirExec
	static std::istream& readLine(std::istream& ifs, string& str);
};
