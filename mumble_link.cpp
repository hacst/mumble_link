/* Copyright (C) 2013, Stefan Hacker <dd0t@users.sourceforge.net>

   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution.
   - Neither the name of the Mumble Developers nor the names of its
     contributors may be used to endorse or promote products derived from this
     software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdint.h>
#include <boost/python.hpp>
#include <boost/thread.hpp>
#include <string>

using namespace std;
using namespace boost;

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

bool run_updater = false;
thread t;

void updater() {
	while (run_updater) {
		this_thread::sleep(posix_time::milliseconds(100));
		lm->uiTick++;
	}
}

bool link_open() {
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
	return true;
}

void stop() {
	run_updater = false;
	t.join();
}

void start() {
	if (run_updater) return;

	run_updater = true;
	t = thread(updater);
}


bool setup(const wstring& name, const wstring& description) {
	if (!link_open())
		return false;

	memset(lm, 0, sizeof(LinkedMem));
	wcsncpy(lm->name, name.c_str(), 256);
	wcsncpy(lm->description, description.c_str(), 2048);
	lm->uiVersion = 2;
	
	start();
	return true;
}

void set_name(const wstring& name) {
	wcsncpy(lm->name, name.c_str(), 256);
}

void set_description(const wstring& description) {
	wcsncpy(lm->description, description.c_str(), 2048);
}

void set_version(const uint32_t version) {
	lm->uiVersion = version;
}

void set_tick(const uint32_t tick) {
	lm->uiTick = tick;
}

uint32_t get_tick() {
	return lm->uiTick;
}

void tick() {
	++lm->uiTick;
}

wstring get_name() {
	return lm->name;
}

wstring get_description() {
	return lm->description;
}

uint32_t get_version() {
	return lm->uiVersion;
}


void set_context(const string& context) {
	memcpy(lm->context, context.c_str(), context.size());
	lm->context_len = context.size();
}

void set_identity(const wstring& identity) {
	wcsncpy(lm->identity, identity.c_str(), 256);
}

string get_context() {
	return string((char*)lm->context, lm->context_len);
}

wstring get_identity() {
	return lm->identity;
}

void set_camera_front(const float x, const float y, const float z) {
	lm->fCameraFront[0] = x;
	lm->fCameraFront[1] = y;
	lm->fCameraFront[2] = z;
}

void set_camera_top(const float x, const float y, const float z) {
	lm->fCameraTop[0] = x;
	lm->fCameraTop[1] = y;
	lm->fCameraTop[2] = z;
}

void set_camera_position(const float x, const float y, const float z) {
	lm->fCameraPosition[0] = x;
	lm->fCameraPosition[1] = y;
	lm->fCameraPosition[2] = z;
}

boost::python::tuple get_camera_front() {
	return boost::python::make_tuple(lm->fCameraFront[0], lm->fCameraFront[1], lm->fCameraFront[2]);
}

boost::python::tuple get_camera_top() {
	return boost::python::make_tuple(lm->fCameraTop[0], lm->fCameraTop[1], lm->fCameraTop[2]);
}

boost::python::tuple get_camera_position() {
	return boost::python::make_tuple(lm->fCameraPosition[0], lm->fCameraPosition[1], lm->fCameraPosition[2]);
}


void set_avatar_front(const float x, const float y, const float z) {
	lm->fAvatarFront[0] = x;
	lm->fAvatarFront[1] = y;
	lm->fAvatarFront[2] = z;
}

void set_avatar_top(const float x, const float y, const float z) {
	lm->fAvatarTop[0] = x;
	lm->fAvatarTop[1] = y;
	lm->fAvatarTop[2] = z;
}

void set_avatar_position(const float x, const float y, const float z) {
	lm->fAvatarPosition[0] = x;
	lm->fAvatarPosition[1] = y;
	lm->fAvatarPosition[2] = z;
}

boost::python::tuple get_avatar_top() {
	return boost::python::make_tuple(lm->fAvatarFront[0], lm->fAvatarFront[1], lm->fAvatarFront[2]);
}

boost::python::tuple get_avatar_front() {
	return boost::python::make_tuple(lm->fAvatarTop[0], lm->fAvatarTop[1], lm->fAvatarTop[2]);
}

boost::python::tuple get_avatar_position() {
	return boost::python::make_tuple(lm->fAvatarPosition[0], lm->fAvatarPosition[1], lm->fAvatarPosition[2]);
}

BOOST_PYTHON_MODULE(mumble_link)
{
	python::def("open", link_open, "Open the shared memory interface without changing it. Make sure open succeeded before taking any other action.");

	python::def("setup", setup, "Setup as plugin with given data. Start auto ticking. Make sure setup succeeded before taking any other action.");

	python::def("start", start, "Start auto ticking every 100ms");
	python::def("stop", stop, "Stop auto ticking.");
	
	python::def("set_name", set_name, "Set plugin name (only takes effect on re-link).");
	python::def("set_description", set_description, "Set plugin description (only takes effect on re-link).");
	python::def("set_version", set_version, "Set plugin version. 0 to unlink, 2 to link.");
	python::def("set_tick", set_tick, "Manually set tick value.");
	python::def("tick", tick, "Performs a tick by incrementing tick value.");

	python::def("get_name", get_name, "Returns the plugin name.");
	python::def("get_description", get_description, "Returns the plugin description.");
	python::def("get_version", get_version, "Returns the plugin version.");
	python::def("get_tick", get_tick, "Returns the current tick value.");
	
	python::def("set_camera_top", set_camera_top, "Sets the top vector for the camera.");
	python::def("set_camera_front", set_camera_front, "Sets the front vector for the camera.");
	python::def("set_camera_position", set_camera_position, "Sets the position vector for the camera.");

	python::def("get_camera_top", get_camera_top, "Returns the camera top vector as a x,y,z tuple.");
	python::def("get_camera_front", get_camera_front, "Returns the camera front vector as a x,y,z tuple.");
	python::def("get_camera_position", get_camera_position, "Returns the camera position vector as a x,y,z tuple.");

	python::def("set_avatar_top", set_avatar_top, "Sets the top vector for the avatar.");
	python::def("set_avatar_front", set_avatar_front, "Sets the front vector for the avatar.");
	python::def("set_avatar_position", set_avatar_position, "Sets the position vector for the avatar.");

	python::def("get_avatar_top", get_avatar_top, "Returns the avatar top vector as a x,y,z tuple.");
	python::def("get_avatar_front", get_avatar_front, "Returns the avatar front vector as a x,y,z tuple.");
	python::def("get_avatar_position", get_avatar_position, "Returns the avatar position vector as a x,y,z tuple.");

	python::def("set_context", set_context, "Set the plugin context. Will be prefixed by name inside Mumble. May contain zero values.");
	python::def("set_identity", set_identity, "Set the plugin identity.");

	python::def("get_context", get_context, "Returns the plugin context. May contain zero values.");
	python::def("get_identity", get_identity, "Returns the plugin identity.");
}
