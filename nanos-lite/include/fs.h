#ifndef __FS_H__
#define __FS_H__

#ifndef SEEK_SET
enum {SEEK_SET, SEEK_CUR, SEEK_END};
#endif

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);
int _fs_start(int fd);
int _fs_end(int fd);
int _fs_size(int fd);
int _fs_offset(int fd);

#endif
