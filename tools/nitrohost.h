#pragma once

#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <sstream>
#include <iostream>

#include <iomanip>
#include <iterator>

#include <fcntl.h>
#include <string.h>

#include <signal.h>
#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#define LOG_REDIS

enum SystemPlatform{
	Windows,
	MacOS,
	Linux,
	RaspberryPi,
	Broadcom,
	WSL2
};

typedef std::string utf8;
typedef std::wstring utf32;

typedef std::string escaped;
typedef std::vector<utf8> Strings;
typedef std::vector<uint8_t> Packet;
typedef std::pair<utf8,escaped> Attribute;
typedef std::map<utf8,escaped> Attributes;
typedef std::vector<Attributes> Data;

// nitrohost cross platform forward declarations

bool fileExists(utf8 path);
int loadString(utf8 path, utf8 &result);
int loadPacket(utf8 path, Packet &result);

int get_uptime();

// nitrohost string util

utf8 joinStrings(const Strings &strings, utf8 separator){
	std::stringstream ss;
	for(size_t i=0;i<strings.size();i++){
		if(i>0) ss << separator;
		ss<<strings[i];
	}
	return ss.str();
}

utf8 escape(const char *s,bool quoted=false){
	std::ostringstream o;
	if(quoted) o<<"\"";
	while(char c=*s++){
		switch (c) {
			case '"': o << "\\\""; break;
			case '\\': o << "\\\\"; break;
			case '\b': o << "\\b"; break;
			case '\f': o << "\\f"; break;
			case '\n': o << "\\n"; break;
			case '\r': o << "\\r"; break;
			case '\t': o << "\\t"; break;
			default:
				if ('\x00' <= c && c <= '\x1f') {
					o << "\\u"
					<< std::hex << std::setw(4) << std::setfill('0') << (int)c;
				} else {
					o << c;
				}
		}
	}
	if(quoted) o<<"\"";
	return o.str();
}

utf8 unquote(utf8 s){
	size_t n=s.size();
	return s.substr(1,n-2);
}

utf8 quoted(utf8 s){
	return escape(s.c_str(),true);
}

utf8 quotedString(const char *s){
	return(s==NULL)?"null":utf8("\"")+s+"\"";
}

utf8 nameValue(utf8 name,utf8 value){
	std::stringstream ss;
	ss<<quoted(name)<<":"<<quoted(value);
	return ss.str();
}

utf8 nameValue(const char *name,int value){
	std::stringstream ss;
	ss<<"{"<<quotedString(name)<<":"<<value<<"}";
	return ss.str();
}

utf8 utf8String(const char *s){
	return(s==NULL)?utf8(""):utf8(s);
}

utf8 trimString(utf8 line){
	while(!line.empty() && line.back()<=' '){
		line.pop_back();
	}
	return line;
}

Strings splitString(utf8 line,char delim){
	Strings split;
	std::stringstream ss(line);
	while(true){
		utf8 line;
		std::getline(ss,line,delim);
		if(line.length()==0) break;
		split.push_back(line);
	}
	return split;
}

utf8 stringReplace(utf8 subject, const utf8& search, const utf8& replace) {
	if (!search.empty()){
		size_t pos = 0;
		while ((pos = subject.find(search, pos)) != utf8::npos) {
			subject.replace(pos, search.length(), replace);
			pos += replace.length();
		}
	}
	return subject;
}

utf8 flatten(utf8 reply){
	utf8 flat1=stringReplace(reply,"\n","\\n");
	utf8 flat2=stringReplace(flat1,"\r","\\r");
	utf8 flat3=stringReplace(flat2,"\t","\\t");
	return flat3;
}

utf8 shorten(utf8 text,int maxlength=80){
	utf8 f=flatten(text);
	if(f.length()>maxlength){
		f=f.substr(0,maxlength)+"...";
	}
	return f;
}

// system specific

//const std::string StaticPath="../../rocksolid27/skypilot";
const std::string StaticPath="../../web";
const std::string CachePath="../../cache";
const std::string GridPath="../../../grid5";

#define AudioFrequency 48000

#define RPCError(num) {if(num){std::cout << "num" << std::endl;return num;}}
#define ErrorResult(num,msg) {if(num){result=msg;std::cout << msg << std::endl;return num;}}
#define ErrorWarn(num,msg) {if(num){std::cout << msg << std::endl;return num;}}
#define ErrorCheck(num,msg) {if(num){std::cout << msg << std::endl;raise(SIGSTOP);return num;}}
#define NullCheck(p,num,msg) {if(p==null){std::cout << msg << std::endl;raise(SIGSTOP);return num;}}

#define Stringify(x) #x

// TODO: FDErrorCheck => FDSignalCheck

#define FDErrorCheck(fd,num,msg) {if(fd<0){std::cout << msg << " errno:" << errno << ":" << strerror(errno) << std::endl;raise(SIGSTOP);return num;}}
#define FDWarnCheck(fd,num,msg) {if(fd<0){std::cout << msg << " errno:" << errno << ":" << strerror(errno) << std::endl;return num;}}

// used by
// int replyGraph(redisReply *r,NitroNode &graph){

struct NitroNode
{
	utf8 name;
	std::vector<NitroNode> kids;

	utf8 toString() const{
		std::stringstream ss;
		ss << quoted(name);
		size_t kidCount=kids.size();
		if(kidCount>0){
			ss<<"[";
			for(int i=0;i<kidCount;i++){
				if(i>0) ss<<",";
				ss<<kids[i].toString();
			}
			ss<<"]";
		}
		return ss.str();
	}
};


#ifdef _WIN32

int fileStat(utf8 path, struct _stat64 &result){
	std::wstring wpath(path.begin(),path.end());
	int error=_wstat64(wpath.c_str(),&result);
	return error;
}

int loadString(utf8 path, utf8 &result){
	int fd=open(path.c_str(),O_RDONLY);	
	FDWarnCheck(fd,1,"loadString open failure for path " << path);
	char buffer[4096];
	int error=0;
	while(true){
		int size=read(fd,buffer,4096);
		if(size<0) {
			error=size;
			break;
		}
		if(size==0) break;
		result.append(buffer,size);
	}
	close(fd);
	return error;
}

int loadPacket(utf8 path, Packet &result){
	int fd=open(path.c_str(),O_RDONLY|O_BINARY);	
	FDWarnCheck(fd,1,"loadPacket open failure for path " << path);
	uint8_t buffer[32768];
	int error=0;
	while(true){
		int size=read(fd,buffer,32768);
		if(size<0) {
			error=size;
			break;
		}
		if(size==0) break;
		result.insert(result.end(),buffer,buffer+size);
	}
	close(fd);
	return error;
}

#endif

#ifdef __APPLE__

#include <sys/sysctl.h>

int get_uptime(){
	struct timeval boottime;
	size_t len = sizeof(boottime);
	int mib[2] = {CTL_KERN, KERN_BOOTTIME};
	int error=sysctl(mib, 2, &boottime, &len, NULL, 0);
	if(error<0) return 0;
	time_t bsec = boottime.tv_sec;
	time_t csec = time(NULL);
	return csec-bsec;
}

size_t get_available(){
	int64_t avail;
	size_t len = sizeof(avail);
	int mib[2] = {CTL_HW, HW_PHYSMEM};
	int error=sysctl(mib, 2, &avail, &len, NULL, 0);
	if(error<0) return 0;
	return avail;
}

#endif

#ifdef __linux__
#include <sys/sysinfo.h>

size_t get_available(){
    struct sysinfo info;
    int error=sysinfo(&info);
    if(error) return 0;
	return info.freeram;
}

int get_uptime()
{
    struct sysinfo s_info;
    int error=sysinfo(&s_info);
    if(error) return 0;
    return (int)s_info.uptime;
}
#endif

#ifndef _WIN32

int loadString(utf8 path, utf8 &result){
	int fd=open(path.c_str(),O_RDONLY);	
	FDWarnCheck(fd,1,"loadString open failure for path " << path);
	char buffer[4096];
	while(true){
		ssize_t size=read(fd,buffer,4096);
		if(size<0) return 2;
		if(size==0) return 0;
		result.append(buffer,size);
	}
	close(fd);
}

bool fileExists(utf8 path){
	bool exists = access(path.c_str(), F_OK) == 0;
	return exists;
}

#endif
