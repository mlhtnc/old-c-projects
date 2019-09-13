#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <wchar.h>

/*reads from keypress, does not echo*/
wchar_t getch(void)
{
	struct termios oldattr, newattr;
	wchar_t ch;
	tcgetattr(STDIN_FILENO, &oldattr);
	newattr=oldattr;
	newattr.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
	ch=getwchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);

	return ch;
}

/*reads from keypress, echoes*/
wchar_t getche(void)
{
	struct termios oldattr, newattr;
	wchar_t ch;
	tcgetattr(STDIN_FILENO, &oldattr);
	newattr=oldattr;
	newattr.c_lflag &= ~(ICANON);
	tcsetattr(STDIN_FILENO, TCSANOW, &newattr);
	ch=getwchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldattr);

	return ch;
}

