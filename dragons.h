/* compile with gcc -o dnd *.c -I. -lm -ggdb3 -lncurses */

#ifndef DRAGONS
#define DRAGONS

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>
#include <dirent.h>

#define LOG {printw("\nIN FILE %s || IN LINE %d", __FILE__, __LINE__); fflush(stdout);}

#define CLEAR   {clear(); printw("****  Dungeons & Dragons  ****\n\n");}
#define CLEAR_f {clear();}
#define WAIT    {while(getch()!='\n');}
#define NOT_VALID {printw("\n Not a valid input.\n");}

#define X  0
#define Y  1
#define MAX_PLAYERS 16
#define SELEC_MAX   17
#define ITEMS       1
#define WEP_AR      2
#define NAME_SIZE   50
#define ITEM_SIZE   20
#define STAT_NUMBER 6
#define ST 0
#define DE 1
#define CO 2
#define IN 3
#define WI 4
#define CH 5

#define HP_D    -5
#define HP_U    0
#define MAX_AC  9
#define MAX_DB  11
#define MAX_BF  5
#define NONE    0b000000000

// DEBUFFS
#define DEAD    0b00000000001
#define UNCON   0b00000000010
#define POISON  0b00000000100
#define BLEED   0b00000001000
#define BLIND   0b00000010000
#define SILEN   0b00000100000
#define STONED  0b00001000000
#define BESERK  0b00010000000
#define FIRE    0b00100000000
#define COLD    0b01000000000
#define TIKI    0b10000000000

#define TIKI_ROUND 50

// BUFFS
#define REGEN   0b000000001
#define GIANT   0b000000010
#define PROTECT 0b000000100
#define FAST    0b000001000
#define INVIS   0b000100000

// BUFF AND DEBUFF DAMAGE
#define FIRE_DAM 3
#define POISON_DAM 2
#define BLEED_DAM 1
#define REGEN_H 1

#define GOLD_START 30

typedef struct SAVE_FILE{
    char name[NAME_SIZE+1];
    struct SAVE_FILE *next;
}SAVE_FILE;

typedef struct ITEM{
    char name[NAME_SIZE+1];
    int id;
    struct ITEM *next;
}ITEM;

typedef struct INV{
    int quantity;
    ITEM *item;
    int type;
    struct INV *next;
}INV;

typedef struct STATUS{
    int cooldown;
    unsigned int status;
    struct STATUS *next;
}STATUS;

typedef struct RACE{
  char name[ITEM_SIZE+1];
  int base_stats[STAT_NUMBER];
  char special[NAME_SIZE+1];
  struct RACE *next;
  int pos;
}RACE;

typedef struct CLASS{
  char name[ITEM_SIZE+1];
  int base_stats[STAT_NUMBER];
  int base_hp, base_ap, base_ac;
  int start_gear[10];
  int start_gear_size;
  struct CLASS *next;
  int pos;
}CLASS;

typedef struct PLAYER{
  int hp, ap, ac;
  int max_hp, max_ap;
  int kol, exp, lvl;
  unsigned long int status, buff;
  RACE *race;
  CLASS *classe;
  INV *inv;
  STATUS *st, *bf;
  char name[NAME_SIZE + 1];
  int stats[STAT_NUMBER];
  struct PLAYER *next;
}PLAYER;

typedef struct GAME{
  int round, play_num, class_num, race_num;
  int item_num, weapon_num, special_num, warn_count;
  int win[2];
  FILE *log;
  char game_name[NAME_SIZE + 1];
  PLAYER *player_list;
  CLASS *class_info;
  RACE *race_info;
  ITEM *item, *weapon_armor, *special, *warning;
}GAME;

int check_save(char name[]);
char* select_game();

int power(int base, int exp);
PLAYER* new_player_list(GAME game);
void list_players(PLAYER *list);
void print_status(PLAYER *pla, int x, int y);
CLASS* class_list(int *class_num);
void list_class_info(CLASS *list, int pos);
RACE* race_list(int *race_num);
void list_race_info(RACE *list, int pos);
ITEM* item_list(int *item_num, int choice);
int list_items(ITEM *list, int pos, int q);
void new_inventory(PLAYER *player, ITEM *list);
void save_game(GAME game, int first);
GAME load_game();
GAME new_game();

void free_player_list(PLAYER *list);
void free_class_list(CLASS *list);
void free_race_list(RACE *list);
void free_item_list(ITEM *list);
void free_inv_list(INV *list);
void free_status_list(STATUS *list);

void game_show();
void damage_ap(GAME game);
void add_inv(GAME game);
void add_item(PLAYER* aux, ITEM* list, int type, int id, int qua);
void rem_inv(GAME game);
void quest_end(GAME game);
void give_player(GAME game);
void add_xp(PLAYER *list, int pla, int xp, int kol, int real);
int level_exp(int level);
void lvl_up(PLAYER *p, int l, int level);
void clean(GAME game);
void esuna(PLAYER *list, int pla);
void status_ailment(GAME game);
void inflict_debuff(PLAYER *list, int pla, int status, int cool);
void inflict_buff(PLAYER *list, int pla, int status, int cool);
void inflict_death(PLAYER *pla);
void list_inventory(GAME game);
int end_round(GAME game, int *count);
void revive(GAME game);
void ac_change(GAME game);
void change_item(PLAYER *pla, GAME game);
void dice_roll();
void add_warning(GAME game, int *count);
void remove_warn(GAME game, int *count);
void change_class(GAME game);
void kill_game(GAME game);

void print_opt(int pos);
void print_debuff(int pos);
void print_buff(int pos);
void print_stats(GAME game);
void print_warnings(GAME game);

void draw_dragon();

#endif
