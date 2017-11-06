#pragma once

#include "utils/settings.h"

enum FileType : uint8 {
	FTYPE_FILE = 0x1,
	FTYPE_DIR  = 0x2,
	FTYPE_LINK = 0x4
};
FileType operator~(FileType a);
FileType operator&(FileType a, FileType b);
FileType operator&=(FileType& a, FileType b);
FileType operator^(FileType a, FileType b);
FileType operator^=(FileType& a, FileType b);
FileType operator|(FileType a, FileType b);
FileType operator|=(FileType& a, FileType b);

struct IniLine {
	IniLine();
	IniLine(const string& ARG, const string& VAL);
	IniLine(const string& ARG, const string& KEY, const string& VAL);
	IniLine(const string& TIT);

	enum class Type : uint8 {
		av,		// argument, value, no key, not title
		akv,	// argument, key, value, no title
		title	// title, no everything else
	} type;
	string arg;	// argument, aka. the thing before the equal sign/brackets
	string key;	// the thing between the brackets (empty if there are no brackets)
	string val;	// value, aka. the thing after the equal sign

	string line() const;				// get the actual INI line
	void setVal(const string& ARG, const string& VAL);
	void setVal(const string& ARG, const string& KEY, const string& VAL);
	void setTitle(const string& TIT);
	bool setLine(const string& lin);	// returns false if not an INI line
	void clear();
};

// handles all filesystem interactions
class Filer {
public:
	static Settings loadSettings();	// read settings file
	static void saveSettings(const Settings& sets);	// write settings file
	static vector<Formula> loadUsers(map<string, double>& vars);	// read formulas and variables from file
	static void saveUsers(const vector<Formula>& forms, const map<string, double>& vars);	// write formulas and variables to file

	static bool readTextFile(const string& file, vector<string>& lines, bool printMessage=true);	// returns true on success
	static bool writeTextFile(const string& file, const vector<string>& lines);	// returns true on success
	static vector<string> listDir(const string& dir, FileType filter=FTYPE_FILE | FTYPE_DIR | FTYPE_LINK, const vector<string>& extFilter={});
	static vector<string> listDirRecursively(const string& dir, sizt offs=0);
	static FileType fileType(const string& path);
	static bool fileExists(const string& path);		// can be used for directories

#ifdef _WIN32
	static vector<char> listDrives();	// get list of drive letters under windows
#endif
	static string findFont(const string& font);	// on success returns absolute path to font file, otherwise returns empty path
	
	static const string dirExec;	// directory in which the executable should currently be
	static const vector<string> dirFonts;	// os's font directories

private:
	static string getDirExec();		// for setting dirExec
	static string checkDirForFont(const string& font, const string& dir);	// necessary for FindFont()
	static std::istream& readLine(std::istream& ifs, string& str);
};

// path functions
bool isAbsolute(const string& path);
string getParentPath(const string& path);
string getFilename(const string& path);	// get filename from path
string getExt(const string& path);		// get file extension
bool hasExt(const string& path, const string& ext);	// whether file has an extension
string delExt(const string& path);		// returns filepath without extension
string appendDsep(const string& path);	// append directory separator if necessary
bool isDriveLetter(const string& path);	// check if path is a drive letter (plus colon and optionally dsep). only for windows
