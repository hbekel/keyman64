#ifdef linux
#undef linux
#define linux !defined(__CYGWIN__)
#endif

#define windows defined(WIN32) || defined(__CYGWIN__)

#define mac defined(__APPLE__)

#define posix linux || mac
