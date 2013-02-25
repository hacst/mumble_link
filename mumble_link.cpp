#include <stdint.h>
#include <boost/python.hpp>
#include <string>

using namespace std;
using namespace boost::python;

#ifdef WIN32
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#endif

struct LinkedMem {
#ifdef WIN32
	UINT32	uiVersion;
	DWORD	uiTick;
#else
	uint32_t uiVersion;
	uint32_t uiTick;
#endif
	float	fAvatarPosition[3];
	float	fAvatarFront[3];
	float	fAvatarTop[3];
	wchar_t	name[256];
	float	fCameraPosition[3];
	float	fCameraFront[3];
	float	fCameraTop[3];
	wchar_t	identity[256];
#ifdef WIN32
	UINT32	context_len;
#else
	uint32_t context_len;
#endif
	unsigned char context[256];
	wchar_t description[2048];
};

LinkedMem *lm = NULL;

LinkedMem tmp;

bool setup(const wstring& name, const wstring& description) {

#ifdef WIN32
	HANDLE hMapObject = OpenFileMappingW(FILE_MAP_ALL_ACCESS, FALSE, L"MumbleLink");
	if (hMapObject == NULL)
		return false;

	lm = (LinkedMem *) MapViewOfFile(hMapObject, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(LinkedMem));
	if (lm == NULL) {
		CloseHandle(hMapObject);
		hMapObject = NULL;
		return false;
	}
#else
	char memname[256];
	snprintf(memname, 256, "/MumbleLink.%d", getuid());

	int shmfd = shm_open(memname, O_RDWR, S_IRUSR | S_IWUSR);

	if (shmfd < 0) {
		return false;
	}

	lm = (LinkedMem *)(mmap(NULL, sizeof(struct LinkedMem), PROT_READ | PROT_WRITE, MAP_SHARED, shmfd,0));

	if (lm == (void *)(-1)) {
		lm = NULL;
		return false;
	}
#endif
	memset(&tmp, 0, sizeof(LinkedMem));
	wcsncpy(tmp.name, name.c_str(), 256);
	wcsncpy(tmp.description, description.c_str(), 2048);
	tmp.uiVersion = 2;

	return true;
}

void set_context(const string& context) {
	memcpy(tmp.context, context.c_str(), context.size());
	tmp.context_len = context.size();
}

void set_identity(const wstring& identity) {
	memcpy(tmp.identity, identity.c_str(), identity.size() * sizeof(wchar_t));
}

void set_camera_front(const float x, const float y, const float z) {
	tmp.fCameraFront[0] = x;
	tmp.fCameraFront[1] = y;
	tmp.fCameraFront[2] = z;
}

void set_camera_top(const float x, const float y, const float z) {
	tmp.fCameraTop[0] = x;
	tmp.fCameraTop[1] = y;
	tmp.fCameraTop[2] = z;
}

void set_camera_position(const float x, const float y, const float z) {
	tmp.fCameraPosition[0] = x;
	tmp.fCameraPosition[1] = y;
	tmp.fCameraPosition[2] = z;
}


void set_avatar_front(const float x, const float y, const float z) {
	tmp.fAvatarFront[0] = x;
	tmp.fAvatarFront[1] = y;
	tmp.fAvatarFront[2] = z;
}

void set_avatar_top(const float x, const float y, const float z) {
	tmp.fAvatarTop[0] = x;
	tmp.fAvatarTop[1] = y;
	tmp.fAvatarTop[2] = z;
}

void set_avatar_position(const float x, const float y, const float z) {
	tmp.fAvatarPosition[0] = x;
	tmp.fAvatarPosition[1] = y;
	tmp.fAvatarPosition[2] = z;
}

bool update() {
	if (! lm)
		return false;

	tmp.uiTick++;
	memcpy(lm, &tmp, sizeof(LinkedMem));
	return true;
}

BOOST_PYTHON_MODULE(mumble_link)
{
	def("setup", setup);
	
	def("set_camera_top", set_camera_position);
	def("set_camera_front", set_camera_front);
	def("set_camera_position", set_camera_position);

	def("set_avatar_top", set_avatar_position);
	def("set_avatar_front", set_avatar_front);
	def("set_avatar_position", set_avatar_position);

	def("set_context", set_context);
	def("set_identity", set_identity);

	def("update", update);
}
