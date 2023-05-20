#include <common.h>

void main_loop();
void hal_init();

int
main(int argc, const char* argv[]) {
	Log("Argc:%d\n",argc);
	// Log("Argv[0]:%s\n",argv[0]);
	Log("game start!");

  hal_init();
	main_loop();

	return 0;
}
