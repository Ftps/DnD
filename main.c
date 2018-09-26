#include "dragons.h"

int main(int argc, char* argv[])
{
    time_t t;

    raw();

    srand((unsigned)time(&t));

    game_show();

  return 0;
}
