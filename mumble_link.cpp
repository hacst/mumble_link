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

bool run_updater = true;
thread t;

void updater() {
	while (run_updater) {
		this_thread::sleep(posix_time::milliseconds(100));
		lm->uiTick++;
	}
}
	
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

	memset(lm, 0, sizeof(LinkedMem));
	wcsncpy(lm->name, name.c_str(), 256);
	wcsncpy(lm->description, description.c_str(), 2048);
	lm->uiVersion = 2;
	t = thread(updater);
	return true;
}

void stop() {
	run_updater = false;
	t.join();
}

void set_context(const string& context) {
	memcpy(lm->context, context.c_str(), context.size());
	lm->context_len = context.size();
}

void set_identity(const wstring& identity) {
	wcsncpy(lm->identity, identity.c_str(), 256);
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

BOOST_PYTHON_MODULE(mumble_link)
{
	python::def("setup", setup);
	python::def("stop", stop);
	
	python::def("set_camera_top", set_camera_position);
	python::def("set_camera_front", set_camera_front);
	python::def("set_camera_position", set_camera_position);

	python::def("set_avatar_top", set_avatar_position);
	python::def("set_avatar_front", set_avatar_front);
	python::def("set_avatar_position", set_avatar_position);

	python::def("set_context", set_context);
	python::def("set_identity", set_identity);
}
