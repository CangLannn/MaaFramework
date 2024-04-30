#include "UI/Term.h"

#ifdef _WIN32

#include <Windows.h>

static inline void enter()
{
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    SetConsoleMode(hStdin, mode);
}

static inline void reset()
{
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hStdin, &mode);

    mode |= (ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
    SetConsoleMode(hStdin, mode);
}

#else

#include <termios.h>
#include <unistd.h>

static inline void enter()
{
    termios tty;
    tcgetattr(STDIN_FILENO, &tty);

    tty.c_lflag &= ~(ICANON | ECHO);

    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

static inline void reset()
{
    termios tty;
    tcgetattr(STDIN_FILENO, &tty);

    tty.c_lflag |= (ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

#endif

namespace maa::cli::ui
{

void enter_raw()
{
    enter();
}

void leave_raw()
{
    reset();
}

}
