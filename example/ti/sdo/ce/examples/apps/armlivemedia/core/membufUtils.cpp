#include "membufUtils.h"
#include "constDefines.h"

unsigned char *buffer_addr[MAX_BUFFER];
unsigned char *sdi0_buffer_addr[MAX_BUFFER];
unsigned char *sdi1_buffer_addr[MAX_BUFFER];

MemBufUtils::MemBufUtils() : m_fdmem(-1) {

}

MemBufUtils::~MemBufUtils() {
	close();
}

bool MemBufUtils::open() {
	if ((m_fdmem = ::open("/dev/mem", O_RDWR|O_SYNC)) == -1) {
	   perror("open /dev/mem fail.");
	   return false;
	}

	buffer_addr[0] = (unsigned char*)mmap(0, MAP_SIZE, PROT_READ|PROT_WRITE,
			MAP_SHARED, m_fdmem, VGA_BASE_ADDR & ~4096);
	if (buffer_addr[0] == MAP_FAILED) {
		printf("mmap fail.\n");
		return false;
	}
	
	memset(buffer_addr[0], 0xFF, MAP_SIZE);

#ifdef CAMERA_SDI
	sdi0_buffer_addr[0] = buffer_addr[0] + CaptureVGASIZE;
	sdi1_buffer_addr[0] = sdi0_buffer_addr[0] + CaptureSDI0SIZE;
#endif
	return true;
}

bool MemBufUtils::close() {
	if (m_fdmem != - 1) {
		::munmap(buffer_addr[0], MAP_SIZE);
		::close(m_fdmem);
		m_fdmem = -1;
	}
	return true;
}
