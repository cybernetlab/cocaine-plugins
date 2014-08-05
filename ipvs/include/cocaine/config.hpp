#define COCAINE_VERSION_MAJOR   0
#define COCAINE_VERSION_MINOR   11
#define COCAINE_VERSION_RELEASE 0

#define COCAINE_MAKE_VERSION(major, minor, release) \
	((major) * 10000 + (minor) * 100 + (release))

#define COCAINE_VERSION \
	COCAINE_MAKE_VERSION(COCAINE_VERSION_MAJOR, COCAINE_VERSION_MINOR, COCAINE_VERSION_RELEASE)
