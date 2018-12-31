#include "dragons.h"

char* skills[STAT_NUMBER] = {"Str", "Dex", "Con", "Int", "Wis", "Cha"};

int check_save(char name[])
{
    DIR *dir;
    struct dirent *ent;
    int i = 0;

    if((dir = opendir("./Games")) != NULL){
        while((ent = readdir(dir)) != NULL){
            if(!strcmp(name, ent->d_name)){i = 1; break;}
        }
        closedir(dir);
    }

    return i;
}

char* select_game()
{
    SAVE_FILE *list = (SAVE_FILE*)malloc(sizeof(SAVE_FILE)), *aux;
    DIR *dir;
    struct dirent *ent;
    char *send = (char*)malloc(sizeof(char)*(NAME_SIZE+1));
    int c = 0, pos = 0, key, end = 1;

    aux = list;
    if((dir = opendir("./Games")) != NULL){
        while((ent = readdir(dir)) != NULL){
            if(!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")) continue;
            aux->next = (SAVE_FILE*)malloc(sizeof(SAVE_FILE));
            aux = aux->next;
            strcpy(aux->name, ent->d_name);
            ++c;
        }
        closedir(dir);
    }
    else{
        printw("\nSomething went wrong.\n");
        WAIT
        exit(-500);
    }

    if(!c){
        CLEAR
        printw("\n\nNo games to load, starting new game.");
        WAIT
        return NULL;
    }

    do{
        CLEAR
        aux = list;
        attron(A_UNDERLINE);
        printw("Select saved game:");
        attroff(A_UNDERLINE);
        for(int i = 0; i < c; ++i){
            aux = aux->next;
            if(i == pos) attron(A_STANDOUT);
            printw("\n%s", aux->name);
            if(i == pos){attroff(A_STANDOUT); printw(" <");}
        }
        if(pos == c) attron(A_STANDOUT);
        printw("\n\nNew Game");
        if(pos == c){attroff(A_STANDOUT); printw(" <");}

        do{
            key = getch();
        }while(key != KEY_UP && key != KEY_DOWN && key != 10);

        switch(key){
            case KEY_UP:{
                if(!pos) pos = c;
                else --pos;
                break;
            }
            case KEY_DOWN:{
                if(pos == c) pos = 0;
                else ++pos;
                break;
            }
            case 10:{
                end = 0;
                break;
            }
        }
    }while(end);

    if(pos == c){
        return NULL;
    }

    aux = list->next;
    for(int i = 0; i < pos; ++i){
        aux = aux->next;
    }

    strcpy(send, "Games/");
    strcat(send, aux->name);

    while(list != NULL){
        aux = list;
        list = list->next;
        free(aux);
    }
    return send;
}




int power(int base, int exp)
{
    if(exp > 0) return base*power(base, exp-1);
    else return 1;
}

PLAYER* new_player_list(GAME game)
{
    PLAYER *aux, *list = (PLAYER*)malloc(sizeof(PLAYER));
    CLASS *aux_c;
    RACE *aux_r;
    int j = 0, end, pos;
    aux = list;

    for(int i = 0; i < game.play_num; ++i, j = 0){
        CLEAR

        aux->next = (PLAYER*)malloc(sizeof(PLAYER));
        aux = aux->next;
        aux_c = game.class_info;
        aux_r = game.race_info;

        printw("\nEnter the name of player %d: ", i+1);
        echo();
        getstr(aux->name);
        noecho();

        aux->kol = GOLD_START;
        //aux->status = BLEED; aux->lvl = aux->buff = aux->exp = 0;
        aux->lvl = aux->buff = aux->status = aux->exp = 0;

        for(int k = 0; k < STAT_NUMBER; k++){
            aux->stats[k] = 13-k;
        }

        end = j = 1;
        do{
            CLEAR
            printw("\nChoose a race:\n");
            list_race_info(aux_r, j);
            move(game.win[X]-1, game.win[Y]-1);
            refresh();
            do{
                pos = getch();
            }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN);

            switch(pos){
                case KEY_UP:{
                    if(j == 1) j = game.race_num;
                    else --j;
                    break;
                }
                case KEY_DOWN:{
                    if(j == game.race_num) j = 1;
                    else ++j;
                    break;
                }
                default:{
                    end = 0;
                    break;
                }
            }
        }while(end);

        for(int k = 0; k < j; ++k){
            aux_r = aux_r->next;
        }
        for(int k = 0; k < STAT_NUMBER; ++k){
            aux->stats[k] += aux_r->base_stats[k];
        }
        aux->race = aux_r;

        end = j = 1;
        do{
            CLEAR
            printw("\nChoose a class:\n");
            list_class_info(aux_c, j);
            move(game.win[X]-1, game.win[Y]-1);
            refresh();
            do{
                pos = getch();
            }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN);

            switch(pos){
                case KEY_UP:{
                    if(j == 1) j = game.class_num;
                    else --j;
                    break;
                }
                case KEY_DOWN:{
                    if(j == game.class_num) j = 1;
                    else ++j;
                    break;
                }
                default:{
                    end = 0;
                    break;
                }
            }
        }while(end);

        for(int k = 0; k < j; ++k){
            aux_c = aux_c->next;
        }
        for(int k = 0; k < STAT_NUMBER; ++k){
            aux->stats[k] += aux_c->base_stats[k];
        }
        aux->max_hp = aux->hp = aux_c->base_hp;
        aux->max_ap = aux->ap = aux_c->base_ap;
        aux->ac = aux_c->base_ac;
        aux->classe = aux_c;

        aux->st = (STATUS*)malloc(sizeof(STATUS));
        aux->st->next = NULL;
        aux->bf = (STATUS*)malloc(sizeof(STATUS));
        aux->bf->next = NULL;

        aux->inv = (INV*)malloc(sizeof(INV));
        aux->inv->next = NULL;
        new_inventory(aux, game.item);
        for(int k = 0; k < aux_c->start_gear_size; ++k){
            add_item(aux, game.weapon_armor, 2, aux_c->start_gear[2*k], aux_c->start_gear[2*k+1]);
        }
        aux->next = NULL;

        pos = 0; end = 1;
        do{
            CLEAR
            printw("\nSelect stat you wish to improve:\n");
            for(int i = 0; i < STAT_NUMBER; ++i){
                if(i == pos) attron(A_STANDOUT);
                printw("\n%s", skills[i]);
                if(i == pos){attroff(A_STANDOUT); printw(" <");}
            }
            printw("\n");

            do{
                j = getch();
            }while(j != KEY_UP && j != KEY_DOWN && j != 10);

            switch(j){
                case KEY_UP:{
                    if(!pos) pos = STAT_NUMBER-1;
                    else --pos;
                    break;
                }
                case KEY_DOWN:{
                    if(pos == STAT_NUMBER-1) pos = 0;
                    else ++pos;
                    break;
                }
                case 10:{
                    end = 0;
                    break;
                }
            }

        }while(end);
        ++aux->stats[pos];
      }
      return list;
}

void list_players(PLAYER *list)
{
    int i = 0, c, x, y;
    float quo;

    while(list->next != NULL){
        ++i;
        list = list->next;

        if(i < 5) y = 0;
        else if(i < 9) y = 48;
        else if(i < 13) y = 96;
        else y = 96+48;

        switch (i) {
            case 1:
            case 5:
            case 9:
            case 13:{x = 5; break;}
            case 2:
            case 6:
            case 10:
            case 14:{x = 13; break;}
            case 3:
            case 7:
            case 11:
            case 15:{x = 21; break;}
            case 4:
            case 8:
            case 12:
            case 16:{x = 29; break;}
        }

        mvprintw(x, y, "Player %d:", i); ++x;
        mvprintw(x, y, "Name: %s", list->name); ++x;
        mvprintw(x, y, "Race: %s, Class: %s\n", list->race->name, list->classe->name); ++x;
        mvprintw(x, y, "HP: ");
        quo = (float)list->hp/(float)list->max_hp;
        if(quo <= 0) c = 1;
        else if(quo > 0 && quo <= 0.3) c = 2;
        else if(quo > 0.3 && quo <= 0.7) c = 3;
        else c = 4;
        attron(COLOR_PAIR(c));
        printw("%d/%d", list->hp, list->max_hp);
        attroff(COLOR_PAIR(c));
        printw(", AP: ");
        quo = (float)list->ap/(float)list->max_ap;
        if(quo == 0) c = 1;
        else if(quo > 0 && quo <= 0.3) c = 2;
        else if(quo > 0.3 && quo <= 0.7) c = 3;
        else c = 4;
        attron(COLOR_PAIR(c));
        printw("%d/%d", list->ap, list->max_ap); ++x;
        attroff(COLOR_PAIR(c));
        printw(", AC: %d", list->ac);
        print_status(list, x, y); x += 2;
        mvprintw(x, y, "Level: %d, Exp: %d, Gold: %d", list->lvl, list->exp, list->kol);
    }
}

void print_status(PLAYER *pla, int x, int y)
{
    mvprintw(x, y, "Debuffs: ");
    if(!pla->status) printw("None");
    else if(pla->status & DEAD){attron(COLOR_PAIR(3)); printw("Dead\n"); attroff(COLOR_PAIR(3));}
    else{
        attron(COLOR_PAIR(1));
        if(pla->status & UNCON)  printw("Uncons, ");
        if(pla->status & BLEED)  printw("Bleed, ");
        if(pla->status & POISON) printw("Poisoned, ");
        if(pla->status & FIRE)   printw("Fire, ");
        if(pla->status & STONED) printw("Stoned, ");
        if(pla->status & SILEN)  printw("Silenced, ");
        if(pla->status & BLIND)  printw("Blind, ");
        if(pla->status & BESERK) printw("Beserk, ");
        if(pla->status & TIKI)   printw("Tiki, ");
        if(pla->status & COLD)   printw("Cold, ");
        attroff(COLOR_PAIR(1));
    }
    ++x;
    mvprintw(x, y, "Buffs: ");
    if(!pla->buff) printw("None");
    else{
        attron(COLOR_PAIR(4));
        if(pla->buff & REGEN)   printw("Regen, ");
        if(pla->buff & GIANT)   printw("Giant, ");
        if(pla->buff & PROTECT) printw("Protect, ");
        if(pla->buff & INVIS)   printw("Invisible, ");
        if(pla->buff & FAST)    printw("Fast, ");
        attroff(COLOR_PAIR(4));
    }


}

CLASS* class_list(int *class_num)
{
    FILE *fp = fopen("Settings/Classes.dnd", "r");
    CLASS *aux, *list = (CLASS*)malloc(sizeof(CLASS));
    int j = 0, stat;
    aux = list;

    fscanf(fp, "%d", class_num);

    for(int i = 0; i < *class_num; ++i, j = 0){
        aux->next = (CLASS*)malloc(sizeof(CLASS));
        aux = aux->next;

        for(int k = 0; k < STAT_NUMBER; k++){
            aux->base_stats[k] = 0;
        }

        while(fgetc(fp)!=':');
        fgetc(fp);
        while((aux->name[j] = fgetc(fp)) != '\n'){
            ++j;
        }
        aux->name[j] = '\0';

        do{
            fscanf(fp, " %d", &stat);
            fscanf(fp, " %d", &(aux->base_stats[stat]));
        }while(fgetc(fp) != '\n');

        fscanf(fp, "%d, %d, %d", &(aux->base_hp), &(aux->base_ap), &(aux->base_ac));
        fgetc(fp);
        j = 0;
        do{
            fscanf(fp, "%d %d", &(aux->start_gear[2*j]), &(aux->start_gear[2*j+1]));
            j++;
        }while(fgetc(fp) != '\n');
        aux->start_gear_size = j;
        aux->next = NULL;
        aux->pos = i+1;
    }

    fclose(fp);

    return list;
}

void list_class_info(CLASS *list, int pos)
{
    int i = 0;
    while(list->next != NULL){
        ++i;
        list = list->next;
        if(i == pos) attron(A_STANDOUT);
        printw("\nClass: %s", list->name);
        if(i == pos){attroff(A_STANDOUT); printw(" <");}
        printw("\nBase Stats: St = %d, De = %d, Co = %d, ", list->base_stats[0], list->base_stats[1], list->base_stats[2]);
        printw("In = %d, Wi = %d, Ch = %d\n", list->base_stats[3], list->base_stats[4], list->base_stats[5]);
        printw("Base HP = %d, Base AP = %d\n\n", list->base_hp, list->base_ap);
    }
}

RACE* race_list(int *race_num)
{
    FILE *fp = fopen("Settings/Races.dnd", "r");
    RACE *aux, *list = (RACE*)malloc(sizeof(RACE));
    int j = 0, stat;
    aux = list;

    fscanf(fp, "%d", race_num);

    for(int i = 0; i < *race_num; ++i, j = 0){
        aux->next = (RACE*)malloc(sizeof(RACE));
        aux = aux->next;
        while(fgetc(fp) != ':');
        fgetc(fp);
        while((aux->name[j] = fgetc(fp)) != '\n'){
            ++j;
        }
        aux->name[j] = '\0';

        do{
            fscanf(fp, " %d", &stat);
            fscanf(fp, " %d", &(aux->base_stats[stat]));
        }while(fgetc(fp) != '\n');

        j = 0;
        while(fgetc(fp) != ':');
        fgetc(fp);
        while((aux->special[j] = fgetc(fp)) != '\n'){
            ++j;
        }
        aux->special[j] = '\0';
        aux->next = NULL;
        aux->pos = i+1;
    }

    fclose(fp);
    return list;
}

void list_race_info(RACE *list, int pos)
{
    int i = 0;
    while(list->next != NULL){
        ++i;
        list = list->next;
        if(i == pos) attron(A_STANDOUT);
        printw("\nRace: %s", list->name);
        if(i == pos){attroff(A_STANDOUT); printw(" <");}
        printw("\nBase Stats: St = %d, De = %d, Co = %d, ", list->base_stats[0], list->base_stats[1], list->base_stats[2]);
        printw("In = %d, Wi = %d, Ch = %d\n", list->base_stats[3], list->base_stats[4], list->base_stats[5]);
        printw("Special Ability: %s\n\n", list->special);
    }
}

ITEM* item_list(int *item_num, int choice)
{
    FILE *fp;
    ITEM *aux, *list;
    int j = 0;

    if(choice == 1) fp = fopen("Settings/Items.dnd", "r");
    else if(choice == 2) fp = fopen("Settings/W&A.dnd", "r");
    else if(choice == 3) fp = fopen("Settings/Specials.dnd", "r");

    aux = list = (ITEM*)malloc(sizeof(ITEM));
    fscanf(fp, "%d", item_num);

    for(int i = 0; i < *item_num; ++i, j = 0){
        aux->next = (ITEM*)malloc(sizeof(ITEM));
        aux = aux->next;
        aux->id = i+1;

        while(fgetc(fp) != ' ');
        while((aux->name[j] = fgetc(fp)) != '\n'){
            ++j;
        }

        aux->name[j] = '\0';
        aux->next = NULL;
    }

    return list;
}

int list_items(ITEM *list, int pos, int q)
{
    int i = 0;

    if(q != 0){
        if(!pos) attron(A_STANDOUT);
        printw("\nPrevious page ");
        if(!pos){attroff(A_STANDOUT); printw(" <");}
    }
    else addch('\n');

    for(int k = 0; k < q; ++k){
        for(int j = 0; j < 45; ++j) list = list->next;
    }

    for(int k = 0; k < 45 && list->next != NULL; ++k){
        ++i;
        list = list->next;
        if(i == pos) attron(A_STANDOUT);
        printw("\nItem ID %d: %s", list->id, list->name);
        if(i == pos){attroff(A_STANDOUT); printw(" <");}
    }


    if(list->next != NULL){
        ++i;
        if(i == pos) attron(A_STANDOUT);
        printw("\nNext page ");
        if(i == pos){attroff(A_STANDOUT); printw(" <");}
        return i;
    }
    else return i;
}

void new_inventory(PLAYER *player, ITEM *list)
{
    add_item(player, list, 1, 1, 6);
    add_item(player, list, 1, 2, 10);
    add_item(player, list, 1, 3, 1);
}

void save_game(GAME game, int first)
{
    FILE *fp;
    INV *aux;
    STATUS *aux_s;

    /*if(first){
        fp = fopen("Games/Current.dnd", "r");
        if(fp != NULL){
            printw("\nPrevious game found. Do you wish to save it for later (y)? ");
            scanf("%c", &c);
            WAIT
            fclose(fp);
            if(c == 'y' || c == 'Y'){
                sprintf(move, "mv Games/Current.dnd Games/%d.dnd", rand());
                system(move);
            }
        }
    }*/

    fp = fopen(game.game_name, "w");

    fprintf(fp, "%d %d %d\n", game.round, game.play_num, game.warn_count);

    for(int i = 0; i < game.play_num; ++i){
        game.player_list = game.player_list->next;
        fprintf(fp, "\n\nName: %s\n", game.player_list->name);
        fprintf(fp, "%d %d %ld %ld\n", game.player_list->race->pos, game.player_list->classe->pos, game.player_list->status, game.player_list->buff);
        aux_s = game.player_list->st;
        while(aux_s->next != NULL){
            aux_s = aux_s->next;
            fprintf(fp, " %d", aux_s->cooldown);
        }
        aux_s = game.player_list->bf;
        while(aux_s->next != NULL){
            aux_s = aux_s->next;
            fprintf(fp, " %d", aux_s->cooldown);
        }
        fputc('\n', fp);
        fprintf(fp, "%d %d %d %d %d\n", game.player_list->hp, game.player_list->max_hp, game.player_list->ap, game.player_list->max_ap,  game.player_list->ac);
        fprintf(fp, "%d %d %d\n", game.player_list->lvl, game.player_list->exp, game.player_list->kol);
        for(int k = 0; k < STAT_NUMBER; ++k){
            fprintf(fp, "%d ", game.player_list->stats[k]);
        }
        fputc('\n', fp);
        aux = game.player_list->inv;
        while(aux->next != NULL){
            aux = aux->next;
            if(aux->next == NULL) fprintf(fp, "%d %d %d", aux->type, aux->item->id, aux->quantity);
            else fprintf(fp, "%d %d %d ", aux->type, aux->item->id, aux->quantity);
        }
        fprintf(fp, "\n");
    }

    while(game.warning->next != NULL){
        game.warning = game.warning->next;
        fprintf(fp, "\nName: %s\n", game.warning->name);
        fprintf(fp, "%d\n", game.warning->id);
    }

    fclose(fp);
}

GAME load_game()
{
    GAME game;
    FILE *fp;
    PLAYER *aux;
    ITEM *aux_w;
    int pos_r, pos_c, bf, st, j = 0;
    char *file_name;

    file_name = select_game();

    if(file_name == NULL){
        game = new_game();
    }
    else{
        fp = fopen(file_name, "r");
        strcpy(game.game_name, file_name);
        free(file_name);

        game.item = item_list(&(game.item_num), 1);
        game.weapon_armor = item_list(&(game.weapon_num), 2);
        game.special = item_list(&(game.special_num), 3);
        game.race_info = race_list(&(game.race_num));
        game.class_info = class_list(&(game.class_num));
        game.warning = (ITEM*)malloc(sizeof(ITEM));
        game.warning->next = NULL;

        aux = game.player_list = (PLAYER*)malloc(sizeof(PLAYER));

        fscanf(fp, "%d %d %d", &(game.round), &(game.play_num), &(game.warn_count));

        for(int i = 0; i < game.play_num; ++i, j = 0){
            aux->next = (PLAYER*)malloc(sizeof(PLAYER));
            aux = aux->next;
            aux->next = NULL;
            aux->inv = (INV*)malloc(sizeof(INV));
            aux->inv->next = NULL;
            aux->classe = game.class_info;
            aux->race = game.race_info;
            aux->st = (STATUS*)malloc(sizeof(STATUS));
            aux->st->next = NULL;
            aux->bf = (STATUS*)malloc(sizeof(STATUS));
            aux->bf->next = NULL;
            aux->status = aux->buff = 0;
            while(fgetc(fp)!=':'){}
            fgetc(fp);
            while((aux->name[j] = fgetc(fp)) != '\n'){

                ++j;
            }
            aux->name[j] = '\0';
            fscanf(fp, "%d %d %d %d", &pos_r, &pos_c, &st, &bf);
            fgetc(fp);

            for(int k = 0; k < pos_c; ++k){
                aux->classe = aux->classe->next;
            }
            for(int k = 0; k < pos_r; ++k){
                aux->race = aux->race->next;
            }
            j = pos_r = 0;
            while(fgetc(fp) != '\n'){
                switch(pos_r){
                    case 0:{
                        fscanf(fp, "%d", &pos_c);
                        do{
                            if(st & power(2, j)){
                                inflict_debuff(aux, 0, power(2, j), pos_c);
                                ++j; break;
                            }
                            else ++j;
                        }while(j < MAX_DB);
                        if(j == MAX_DB){
                            j = 0;
                            pos_r = 1;
                            do{
                                if(bf & power(2, j)){

                                    inflict_buff(aux, 0, power(2, j), pos_c);
                                    ++j; break;
                                }
                                else ++j;
                            }while(j < MAX_BF);
                        }
                        break;
                    }
                    case 1:{
                        fscanf(fp, "%d", &pos_c);
                        do{
                            if(bf & power(2, j)){
                                inflict_buff(aux, 0, power(2, j), pos_c);
                                ++j; break;
                            }
                            else ++j;
                        }while(j < MAX_BF);
                        break;
                    }
                }
            }
            aux->status = st;

            fscanf(fp, "%d %d %d %d %d", &(aux->hp), &(aux->max_hp), &(aux->ap), &(aux->max_ap), &(aux->ac));
            fgetc(fp);
            fscanf(fp, "%d %d %d", &(aux->lvl), &(aux->exp), &(aux->kol));
            fgetc(fp);
            for(int k = 0; k < STAT_NUMBER; ++k){
                fscanf(fp, " %d", &(aux->stats[k]));
            }

            aux->inv = (INV*)malloc(sizeof(INV));
            aux->inv->next = NULL;
            fgetc(fp);

            do{
                fscanf(fp, "%d %d %d", &pos_r, &pos_c, &j);
                switch (pos_r) {
                    case 1:{add_item(aux, game.item, 1, pos_c, j); break;}
                    case 2:{add_item(aux, game.weapon_armor, 2, pos_c, j); break;}
                    case 3:{break; /*later*/}
                }
            }while(fgetc(fp) != '\n');

        }
        aux_w = game.warning; j = 0;
        for(int i = 0; i < game.warn_count; ++i, j = 0){
            aux_w->next = (ITEM*)malloc(sizeof(ITEM));
            aux_w = aux_w->next;
            while(fgetc(fp) != ':');
            fgetc(fp);
            while((aux_w->name[j] = fgetc(fp)) != '\n'){
                ++j;
            }
            aux_w->name[j] = '\0';
            fscanf(fp, "%d", &(aux_w->id));
            aux_w->next = NULL;
        }
    }
    fclose(fp);
    return game;
}

GAME new_game()
{
    GAME game;
    int end = 1, end2, pos, key;
    char name[NAME_SIZE+1];

    game.item = item_list(&(game.item_num), 1);
    game.weapon_armor = item_list(&(game.weapon_num), 2);
    game.special = item_list(&(game.special_num), 3);
    game.race_info = race_list(&(game.race_num));
    game.class_info = class_list(&(game.class_num));
    game.warning = (ITEM*)malloc(sizeof(ITEM));
    game.warning->next = NULL;
    game.play_num = game.round = 1; game.warn_count = 0;

    do{
        CLEAR
        printw("\nName of the new game: ");
        echo();
        getstr(name);
        noecho();

        strcat(name, ".dnd");

        if(check_save(name)){
            pos = 0; end2 = 1;
            do{
                CLEAR
                printw("\nSaved game already exists. Overwrite?");

                if(!pos) attron(A_STANDOUT);
                printw("\n\nYes");
                if(!pos){attroff(A_STANDOUT); printw(" <");}
                if(pos) attron(A_STANDOUT);
                printw("\nNo");
                if(pos){attroff(A_STANDOUT); printw(" <");}
                printw("\n");

                do{
                    key = getch();
                }while(key != KEY_UP && key != KEY_DOWN && key != 10);

                if(key == KEY_UP || key == KEY_DOWN) pos = pos^0b1;
                else end2 = 0;
            }while(end2);
            if(!pos) end = 0;
        }
        else end = 0;
    }while(end);

    strcpy(game.game_name, "Games/");
    strcat(game.game_name, name);

    end = 1;
    do{
        CLEAR
        printw("\nInput the number of players (max = %d): < %02d >", MAX_PLAYERS, game.play_num);
        move(game.win[X]-1, game.win[Y]-1);
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_LEFT && pos != KEY_RIGHT);

        switch (pos) {
            case KEY_LEFT:{
                if(game.play_num == 1) game.play_num = MAX_PLAYERS;
                else --game.play_num;
                break;
            }
            case KEY_RIGHT:{
                if(game.play_num == MAX_PLAYERS) game.play_num = 1;
                else ++game.play_num;
                break;
            }
            default:{
                end = 0;
                break;
            }
        }

    }while(end);

    game.player_list = new_player_list(game);

    save_game(game, 1);

    return game;
}





void free_player_list(PLAYER *list)
{
    if(list->next != NULL) free_player_list(list->next);
    if(list->inv != NULL) free_inv_list(list->inv);
    if(list->st != NULL)free_status_list(list->st);
    if(list->bf != NULL)free_status_list(list->bf);
    free(list);
}

void free_class_list(CLASS *list)
{
    if(list->next != NULL) free_class_list(list->next);
    free(list);
}

void free_race_list(RACE *list)
{
    if(list->next != NULL) free_race_list(list->next);
    free(list);
}

void free_item_list(ITEM *list)
{
    if(list->next != NULL) free_item_list(list->next);
    free(list);
}

void free_inv_list(INV *list)
{
    if(list->next != NULL) free_inv_list(list->next);
    free(list);
}

void free_status_list(STATUS *list)
{
    if(list->next != NULL) free_status_list(list->next);
    free(list);
}





void game_show()
{

    int end = 1, k = 0, pos;
    GAME game;

    draw_dragon();
    printf("\n\n\t\t\t\tPress enter to continue!");
    getchar();
    system("clear");

    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    getmaxyx(stdscr, game.win[X], game.win[Y]);

    do{
        CLEAR

        if(!k) attron(A_STANDOUT);
        printw("\nNew Game");
        if(!k) {attroff(A_STANDOUT); printw(" <");}
        if(k) attron(A_STANDOUT);
        printw("\nLoad Game");
        if(k) {attroff(A_STANDOUT); printw(" <");}
        printw("\n");
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN);
        clear();
        if(pos == KEY_UP || pos == KEY_DOWN){
            ++k;
            k = 0b1 & k;
        }
        else{
            if(k){
                game = load_game();
                end = 0;
            }
            else{
                game = new_game();
                end = 0;
            }
        }
    }while(end);
    end = 1;
    k = 0;
    do{
        CLEAR
        noecho();
        attron(COLOR_PAIR(2));
        printw("Round %d\n\n", game.round);
        attroff(COLOR_PAIR(2));
        list_players(game.player_list);
        attron(A_UNDERLINE);
        mvprintw(36, 0, "\nWhat to do now?\n");
        attroff(A_UNDERLINE);
        print_opt(k);
        print_warnings(game);
        refresh();
        do{
            pos = getch();
        }while(pos != '\n' && pos != KEY_UP && pos != KEY_DOWN);
        switch (pos) {
            case KEY_UP:{
                if(k == 0) k = SELEC_MAX-1;
                else --k;
                break;
            }
            case KEY_DOWN:{
                if(k == SELEC_MAX-1) k = 0;
                else ++k;
                break;
            }
            case 10:{
                switch (k) {
                    case 0:{damage_ap(game); break;}
                    case 1:{print_stats(game); break;}
                    case 2:{list_inventory(game); break;}
                    case 3:{add_inv(game); break;}
                    case 4:{rem_inv(game); break;}
                    case 5:{give_player(game); break;}
                    case 6:{quest_end(game); break;}
                    case 7:{clean(game); break;}
                    case 8:{status_ailment(game); break;}
                    case 9:{
                        ++game.round;
                        if(!end_round(game, &(game.warn_count))){
                            kill_game(game);
                            end = 0;
                        }
                        break;
                    }
                    case 10:{revive(game); break;}
                    case 11:{ac_change(game); break;}
                    case 12:{dice_roll(); break;}
                    case 13:{add_warning(game, &(game.warn_count)); break;}
                    case 14:{remove_warn(game, &(game.warn_count)); break;}
                    case 15:{change_class(game); break;}
                    case 16:{end = 0; save_game(game, 0); break;}
                }
                if(end) save_game(game, 0);
                break;
            }
        }
    }while(end);
    free_player_list(game.player_list);
    free_class_list(game.class_info);
    free_race_list(game.race_info);
    free_item_list(game.item);
    free_item_list(game.weapon_armor);
    free_item_list(game.special);
    free_item_list(game.warning);

    CLEAR_f
    endwin();
}

void damage_ap(GAME game)
{
    int end = 1, k = 1, pos, pla, hp[2] = {0}, ap[2] = {0};
    PLAYER *aux;

    pla = 1;
    do{
        CLEAR
        aux = game.player_list;
        for(int k = 1; k <= game.play_num; ++k){
            aux = aux->next;
            if(k == pla) attron(A_STANDOUT);
            printw("\nPlayer %d: %s", k, aux->name);
            if(k == pla){attroff(A_STANDOUT); printw(" <");}
        }
        addch('\n');
        move(game.win[X]-2, game.win[Y]-2);
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != 27);
        switch (pos) {
            case KEY_UP:{
                if(pla == 1) pla = game.play_num;
                else --pla;
                break;
            }
            case KEY_DOWN:{
                if(pla == game.play_num) pla = 1;
                else ++pla;
                break;
            }
            case 27:{return;break;}
            default:{end = 0; break;}
        }
    }while(end);

    aux = game.player_list;
    for(int i = 0; i < pla; ++i){
        aux = aux->next;
    }

    if(aux->status & DEAD){
        printw("\nPlayer %d is dead, can't do action.\n", pla);
        WAIT
        return;
    }

    end = 1;
    pla = k = 0;
    do{
        CLEAR
        printw("Change HP and AP:\n\n");
        switch (k) {
            case 0b00:{printw("HP: -%d%d AP: -%d%d", hp[1], hp[0], ap[1], ap[0]); break;}
            case 0b01:{printw("HP: -%d%d AP: +%d%d", hp[1], hp[0], ap[1], ap[0]); break;}
            case 0b10:{printw("HP: +%d%d AP: -%d%d", hp[1], hp[0], ap[1], ap[0]); break;}
            case 0b11:{printw("HP: +%d%d AP: +%d%d", hp[1], hp[0], ap[1], ap[0]); break;}
        }
        switch (pla) {
            case 0:{move(3, 4); addch('|'); move(5, 4); addch('|'); break;}
            case 1:{move(3, 5); addch('|'); move(5, 5); addch('|'); break;}
            case 2:{move(3, 6); addch('|'); move(5, 6); addch('|'); break;}
            case 3:{move(3, 12); addch('|'); move(5, 12); addch('|'); break;}
            case 4:{move(3, 13); addch('|'); move(5, 13); addch('|'); break;}
            case 5:{move(3, 14); addch('|'); move(5, 14); addch('|'); break;}
        }
        move(game.win[X]-1, game.win[Y]-1);
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != 27 && pos != KEY_UP && pos != KEY_DOWN && pos != KEY_LEFT && pos != KEY_RIGHT);
        switch (pos) {
            case KEY_UP:{
                switch (pla) {
                    case 0:{if(k & 0b10) k -= 0b10; else k += 0b10; break;}
                    case 1:{if(hp[1] == 9) hp[1] = 0; else ++hp[1]; break;}
                    case 2:{if(hp[0] == 9) hp[0] = 0; else ++hp[0]; break;}
                    case 3:{if(k & 0b01) k -= 0b01; else k += 0b01; break;}
                    case 4:{if(ap[1] == 9) ap[1] = 0; else ++ap[1]; break;}
                    case 5:{if(ap[0] == 9) ap[0] = 0; else ++ap[0]; break;}
                } break;
            }
            case KEY_DOWN:{
                switch (pla) {
                    case 0:{if(k & 0b10) k -= 0b10; else k += 0b10; break;}
                    case 1:{if(hp[1] == 0) hp[1] = 9; else --hp[1]; break;}
                    case 2:{if(hp[0] == 0) hp[0] = 9; else --hp[0]; break;}
                    case 3:{if(k & 0b01) k -= 0b01; else k += 0b01; break;}
                    case 4:{if(ap[1] == 0) ap[1] = 9; else --ap[1]; break;}
                    case 5:{if(ap[0] == 0) ap[0] = 9; else --ap[0]; break;}
                } break;
            }
            case KEY_LEFT:{if(pla == 0) pla = 5; else --pla; break;}
            case KEY_RIGHT:{if(pla == 5) pla = 0; else ++pla; break;}
            case 27:{return;break;}
            default:{end = 0; break;}
        }
    }while(end);

    if(!(k & 0b01)){ap[0] = -ap[0]; ap[1] = -ap[1];}
    if(!(k & 0b10)){hp[0] = -hp[0]; hp[1] = -hp[1];}

    if(aux->hp + (hp[0] + 10*hp[1]) <= -5){inflict_death(aux); return;}
    else if(aux->hp + (hp[0] + 10*hp[1]) <= 0 && !(aux->status & UNCON)){aux->hp += hp[0] + 10*hp[1]; aux->status += UNCON;}
    else if(aux->hp + (hp[0] + 10*hp[1]) > aux->max_hp) aux->hp = aux->max_hp;
    else aux->hp += hp[0] + 10*hp[1];

    if(aux->ap + (ap[0] + 10*ap[1]) < 0){printw("\nPlayer doesn't have enough AP to perform action.\n"); WAIT}
    else if(aux->ap + (ap[0] + 10*ap[1]) > aux->max_ap) aux->ap = aux->max_ap;
    else aux->ap += ap[0] + 10*ap[1];
}

void add_inv(GAME game)
{
    PLAYER *aux;
    int end, pos, id, qua, type, max, pla, q[2] = {0};

    pla = end = 1;
    do{
        CLEAR
        aux = game.player_list;
        for(int k = 1; k <= game.play_num; ++k){
            aux = aux->next;
            if(k == pla) attron(A_STANDOUT);
            printw("\nPlayer %d: %s", k, aux->name);
            if(k == pla){attroff(A_STANDOUT); printw(" <");}
        }
        addch('\n');
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != 27);
        switch (pos) {
            case KEY_UP:{
                if(pla == 1) pla = game.play_num;
                else --pla;
                break;
            }
            case KEY_DOWN:{
                if(pla == game.play_num) pla = 1;
                else ++pla;
                break;
            }
            case 27:{return;break;}
            default:{end = 0; break;}
        }
    }while(end);
    aux = game.player_list;
    for(int i = 0; i < pla; ++i){
        aux = aux->next;
    }

    type = end = 1;
    do{
        CLEAR
        printw("\nChoose an item type:\n");
        if(type == 1) attron(A_STANDOUT);
        printw("\nNormal Items");
        if(type == 1){attroff(A_STANDOUT); printw(" <");}
        if(type == 2) attron(A_STANDOUT);
        printw("\nEquipment & Weapons");
        if(type == 2){attroff(A_STANDOUT); printw(" <");}
        if(type == 3) attron(A_STANDOUT);
        printw("\nSpecial Items");
        if(type == 3){attroff(A_STANDOUT); printw(" <");}
        addch('\n');
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != 27);

        switch (pos) {
            case KEY_UP:{if(type == 1) type = 3; else --type; break;}
            case KEY_DOWN:{if(type == 3) type = 1; else ++type; break;}
            case 27:{return;break;}
            default:{end = 0; break;}
        }
    }while(end);

    id = end = 1;
    pla = 0;
    do{
        CLEAR
        printw("\nChoose the item:\n");
        switch (type) {
            case 1:{max = list_items(game.item, id, pla); break;}
            case 2:{max = list_items(game.weapon_armor, id, pla); break;}
            case 3:{max = list_items(game.special, id, pla); break;}
        }
        addch('\n');
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != 27);

        switch (pos) {
            case KEY_UP:{if((id == 1 && !pla) || !id) id = max; else --id; break;}
            case KEY_DOWN:{
                if(id == max){
                    if(pla) id = 0;
                    else id = 1;
                }
                else ++id;
                break;
            }
            case 27:{return;break;}
            default:{
                if(id == 0){--pla; id = 1;}
                else if(id == 46){++pla; id = 1;}
                else{end = 0; id += pla*45;}
                break;
            }
        }
    }while(end);

    end = 1;
    max = 0;
    do{
        CLEAR
        printw("\nItem quantity: %d%d", q[0], q[1]);
        move(2, 15+max); addch('|'); move(4, 15+max); addch('|');
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != KEY_LEFT && pos != KEY_RIGHT && pos != 27);

        if(pos == KEY_LEFT || pos == KEY_RIGHT) max = (max + 1) & 0b1;
        else{
            switch (pos) {
                case KEY_UP:{if(q[max] == 9) q[max] = 0; else ++q[max]; break;}
                case KEY_DOWN:{if(q[max] == 0) q[max] = 9; else --q[max]; break;}
                case 27:{return;break;}
                default:{end = 0; break;}
            }
        }

    }while(end);

    qua = q[1] + 10*q[0];
    if(qua == 0) return;

    switch (type) {
        case 1:{add_item(aux, game.item, type, id, qua); break;}
        case 2:{add_item(aux, game.weapon_armor, type, id, qua); break;}
        case 3:{add_item(aux, game.special, type, id, qua); break;}
    }
}

void add_item(PLAYER* aux, ITEM* list_i, int type, int id, int qua)
{
    INV *aux_i, *new;

    aux_i = aux->inv;
    while(aux_i->next != NULL){
        if(aux_i->next->type != type || aux_i->next->item->id < id){
            aux_i = aux_i->next;
            continue;
        }
        else if(aux_i->next->type > type) break;
        else if(aux_i->next->item->id == id){
            aux_i->next->quantity += qua;
            return;
        }
        else break;
    }
    for(int i = 0; i < id; ++i){
        list_i = list_i->next;
    }
    new = (INV*)malloc(sizeof(INV));
    new->item = list_i;
    new->quantity = qua;
    new->type = type;
    new->next = aux_i->next;
    aux_i->next = new;
}

void rem_inv(GAME game)
{
    INV *aux_i, *freed;
    PLAYER *aux;
    int end, pos, pla, max;

    pla = end = 1;
    do{
        CLEAR
        aux = game.player_list;
        for(int k = 1; k <= game.play_num; ++k){
            aux = aux->next;
            if(k == pla) attron(A_STANDOUT);
            printw("\nPlayer %d: %s", k, aux->name);
            if(k == pla){attroff(A_STANDOUT); printw(" <");}
        }
        addch('\n');
        move(game.win[X]-2, game.win[Y]-2);
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != 27);
        switch (pos) {
            case KEY_UP:{
                if(pla == 1) pla = game.play_num;
                else --pla;
                break;
            }
            case KEY_DOWN:{
                if(pla == game.play_num) pla = 1;
                else ++pla;
                break;
            }
            case 27:{return;break;}
            default:{end = 0; break;}
        }
    }while(end);
    aux = game.player_list;
    for(int i = 0; i < pla; ++i){
        aux = aux->next;
    }

    pla = end = 1;
    do{
        max = 0;
        aux_i = aux->inv;
        CLEAR
        printw("Select item:\n");
        while(aux_i->next != NULL){
            ++max;
            aux_i = aux_i->next;
            if(max == pla) attron(A_STANDOUT);
            printw("\nItem: %s x%d (#%d_%d)", aux_i->item->name, aux_i->quantity, aux_i->item->id, aux_i->type);
            if(max == pla){attroff(A_STANDOUT); printw(" <");}
        }
        addch('\n');
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != 27);

        switch (pos) {
            case KEY_UP:{if(pla == 1) pla = max; else --pla; break;}
            case KEY_DOWN:{if(pla == max) pla = 1; else ++pla; break;}
            case 27:{return;break;}
            default:{end = 0; break;}
        }
    }while(end);
    aux_i = aux->inv;
    for(int i = 0; i < pla-1; ++i){
        aux_i = aux_i->next;
    }

    end = 1;
    pla = 0;
    do{
        CLEAR
        printw("\nSelect quantity: %03d", pla);
        move(2, 17); printw("|||"); move(4, 17); printw("|||");
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != 27);

        switch (pos) {
            case KEY_UP:{if(pla == aux_i->next->quantity) pla = 0; else ++pla; break;}
            case KEY_DOWN:{if(pla == 0) pla = aux_i->next->quantity; else --pla; break;}
            case 27:{return;break;}
            default:{end = 0; break;}
        }
    }while(end);

    if(pla == 0) return;
    else if(pla == aux_i->next->quantity){
        freed = aux_i->next;
        aux_i->next = freed->next;
        free(freed);
    }
    else aux_i->next->quantity -= pla;

}

void quest_end(GAME game)
{
    int pos, end, xp[4]={0}, kol[4]={0}, x, k, pla;

    end = 1;
    pla = 0;
    do{
        CLEAR
        printw("Input Exp and Gold:\n\n");

        printw("Exp: %d%d%d%d Gold: %d%d%d%d", xp[0], xp[1], xp[2], xp[3], kol[0], kol[1], kol[2], kol[3]); // 3 5

        if(pla < 4){move(3, 5+pla); addch('|'); move(5, 5+pla); addch('|');}
        else{move(3, 12+pla); addch('|'); move(5, 12+pla); addch('|');}

        move(game.win[X]-1, game.win[Y]-1);
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != 27 && pos != KEY_UP && pos != KEY_DOWN && pos != KEY_LEFT && pos != KEY_RIGHT);
        switch (pos) {
            case KEY_UP:{
                if(pla < 4){
                    if(xp[pla] == 9) xp[pla] = 0; else ++xp[pla];
                }
                else{
                    if(kol[pla-4] == 9) kol[pla-4] = 0; else ++kol[pla-4];
                }
                break;
            }
            case KEY_DOWN:{
                if(pla < 4){
                    if(xp[pla] == 0) xp[pla] = 9; else --xp[pla];
                }
                else{
                    if(kol[pla-4] == 0) kol[pla-4] = 9; else --kol[pla-4];
                }
                break;
            }
            case KEY_LEFT:{if(pla == 0) pla = 7; else --pla; break;}
            case KEY_RIGHT:{if(pla == 7) pla = 0; else ++pla; break;}
            case 27:{return;break;}
            default:{end = 0; break;}
        }
    }while(end);

    x = 1000*xp[0] + 100*xp[1] + 10*xp[2] + xp[3];
    k = 1000*kol[0] + 100*kol[1] + 10*kol[2] + kol[3];

    for(int i = 1; i <= game.play_num; ++i){
        add_xp(game.player_list, i, x, k, i);
    }
}

void give_player(GAME game)
{
    PLAYER *aux;
    int pos, end, xp[4]={0}, kol[4]={0}, sig, x, k, pla, player;

    pla = end = 1; sig = 0;
    do{
        CLEAR
        aux = game.player_list;
        for(int k = 1; k <= game.play_num; ++k){
            aux = aux->next;
            if(k == pla) attron(A_STANDOUT);
            printw("\nPlayer %d: %s", k, aux->name);
            if(k == pla){attroff(A_STANDOUT); printw(" <");}
        }
        addch('\n');
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != 27);
        switch (pos) {
            case KEY_UP:{
                if(pla == 1) pla = game.play_num;
                else --pla;
                break;
            }
            case KEY_DOWN:{
                if(pla == game.play_num) pla = 1;
                else ++pla;
                break;
            }
            case 27:{return;break;}
            default:{end = 0; break;}
        }
    }while(end);
    aux = game.player_list;
    for(int i = 0; i < pla; ++i){
        aux = aux->next;
    }
    player = pla;
    end = 1;
    pla = 0;
    do{
        CLEAR
        printw("Input Exp and Gold:\n\n");

        switch (sig) {
            case 0b00:{printw("Exp: +%d%d%d%d Gold: +%d%d%d%d", xp[0], xp[1], xp[2], xp[3], kol[0], kol[1], kol[2], kol[3]); break;}
            case 0b01:{printw("Exp: +%d%d%d%d Gold: -%d%d%d%d", xp[0], xp[1], xp[2], xp[3], kol[0], kol[1], kol[2], kol[3]); break;}
            case 0b10:{printw("Exp: -%d%d%d%d Gold: +%d%d%d%d", xp[0], xp[1], xp[2], xp[3], kol[0], kol[1], kol[2], kol[3]); break;}
            case 0b11:{printw("Exp: -%d%d%d%d Gold: -%d%d%d%d", xp[0], xp[1], xp[2], xp[3], kol[0], kol[1], kol[2], kol[3]); break;}
        }

        if(pla < 5){move(3, 5+pla); addch('|'); move(5, 5+pla); addch('|');}
        else{move(3, 12+pla); addch('|'); move(5, 12+pla); addch('|');}

        move(game.win[X]-1, game.win[Y]-1);
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != KEY_LEFT && pos != KEY_RIGHT);
        switch (pos) {
            case KEY_UP:{
                switch (pla) {
                    case 0:{if(sig & 0b10) sig -= 0b10; else sig += 0b10; break;}
                    case 5:{if(sig & 0b01) sig -= 0b01; else sig += 0b01; break;}
                    default:{
                        if(pla < 5){
                            if(xp[pla-1] == 9) xp[pla-1] = 0; else ++xp[pla-1];
                        }
                        else{
                            if(kol[pla-6] == 9) kol[pla-6] = 0; else ++kol[pla-6];
                        }
                    }
                }
                break;
            }
            case KEY_DOWN:{
                switch (pla) {
                    case 0:{if(sig & 0b10) sig -= 0b10; else sig += 0b10; break;}
                    case 5:{if(sig & 0b01) sig -= 0b01; else sig += 0b01; break;}
                    default:{
                        if(pla < 5){
                            if(xp[pla-1] == 0) xp[pla-1] = 9; else --xp[pla-1];
                        }
                        else{
                            if(kol[pla-6] == 0) kol[pla-6] = 9; else --kol[pla-6];
                        }
                    }
                }
                break;
            }
            case KEY_LEFT:{if(pla == 0) pla = 9; else --pla; break;}
            case KEY_RIGHT:{if(pla == 9) pla = 0; else ++pla; break;}
            default:{end = 0; break;}
        }
    }while(end);

    x = 1000*xp[0] + 100*xp[1] + 10*xp[2] + xp[3];
    k = 1000*kol[0] + 100*kol[1] + 10*kol[2] + kol[3];
    if(sig & 0b01) k = -1*k;
    if(sig & 0b10) x = -1*x;

    add_xp(aux, 0, x, k, player);
}

void add_xp(PLAYER *list, int pla, int xp, int kol, int real)
{
    int end = 1;

    for(int i = 0; i < pla; ++i){
        list = list->next;
    }

    if(list->status & DEAD || list->status & UNCON){
        printw("Player %d is dead or uncons, can't give EXP or Gold.\n", pla);
        WAIT
        return;
    }

    list->exp += xp;
    if(!strcmp(list->race->special, "Gold Fingers")) list->kol += kol*3/2;
    else list->kol += kol;

    do{
        if(list->exp >= level_exp(list->lvl)){
            ++list->lvl;
            lvl_up(list, real, list->lvl);
        }
        else end = 0;
    }while(end);
}

int level_exp(int level)
{
    if(level == 0) return 512;
    else return level_exp(level-1) + pow(2, level+9);
}

void lvl_up(PLAYER *p, int l, int level)
{
    int hp, ap, key, pos, end = 1;

    hp = (rand()%p->classe->base_hp)+1;
    ap = (rand()%p->classe->base_ap)+1;

    pos = 0;
    do{
        CLEAR
        printw("\nPlayer %s leveled up! Gained %d HP and %d AP.\n", p->name, hp, ap);
        printw("\nSelect stat you wish to improve:\n");
        for(int i = 0; i < STAT_NUMBER; ++i){
            if(i == pos) attron(A_STANDOUT);
            printw("\n%s", skills[i]);
            if(i == pos){attroff(A_STANDOUT); printw(" <");}
        }
        printw("\n");

        do{
            key = getch();
        }while(key != KEY_UP && key != KEY_DOWN && key != 10);

        switch(key){
            case KEY_UP:{
                if(!pos) pos = STAT_NUMBER-1;
                else --pos;
                break;
            }
            case KEY_DOWN:{
                if(pos == STAT_NUMBER-1) pos = 0;
                else ++pos;
                break;
            }
            case 10:{
                end = 0;
                break;
            }
        }

    }while(end);

    p->max_hp += hp;
    p->max_ap += ap;
    p->hp = p->max_hp;
    p->ap = p->max_ap;
    ++p->stats[pos];

    p->status = 0;
}

void clean(GAME game)
{

    PLAYER *aux;
    int pla, end, pos;

    pla = end = 1;
    do{
        CLEAR
        aux = game.player_list;
        for(int k = 1; k <= game.play_num; ++k){
            aux = aux->next;
            if(k == pla) attron(A_STANDOUT);
            printw("\nPlayer %d: %s", k, aux->name);
            if(k == pla){attroff(A_STANDOUT); printw(" <");}
        }
        addch('\n');
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != 27);
        switch (pos) {
            case KEY_UP:{
                if(pla == 1) pla = game.play_num;
                else --pla;
                break;
            }
            case KEY_DOWN:{
                if(pla == game.play_num) pla = 1;
                else ++pla;
                break;
            }
            case 27:{return;break;}
            default:{end = 0; break;}
        }
    }while(end);

    esuna(game.player_list, pla);
}

void esuna(PLAYER *list, int pla)
{
    STATUS *aux, *freed;

    for(int i = 0; i < pla; ++i){
        list = list->next;
    }
    if(list->status & DEAD){
        printw("Player %d is dead, can't use esuna!\n", pla);
        WAIT
        return;
    }

    for(int i = 2; i < MAX_DB-1; ++i){
        if(list->status & (0b1 << i)){
            list->status -= 0b1 << i;
            aux = list->st;
            while(aux->next != NULL){
                if(aux->next->status & (0b1 << i)){
                    freed = aux->next;
                    aux->next = freed->next;
                    free(freed);
                    break;
                }
            }
        }
    }

}

void status_ailment(GAME game)
{

    PLAYER *aux;
    int pla, end, pos, st, cool, type, max, q[2] = {0};

    pla = end = 1;
    do{
        CLEAR
        aux = game.player_list;
        for(int k = 1; k <= game.play_num; ++k){
            aux = aux->next;
            if(k == pla) attron(A_STANDOUT);
            printw("\nPlayer %d: %s", k, aux->name);
            if(k == pla){attroff(A_STANDOUT); printw(" <");}
        }
        addch('\n');
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != 27);
        switch (pos) {
            case KEY_UP:{
                if(pla == 1) pla = game.play_num;
                else --pla;
                break;
            }
            case KEY_DOWN:{
                if(pla == game.play_num) pla = 1;
                else ++pla;
                break;
            }
            case 27:{return;break;}
            default:{end = 0; break;}
        }
    }while(end);


    end = 1; type = 0;
    do{
        CLEAR
        printw("\nChoose status ailment type:\n");
        if(!type) attron(A_STANDOUT);
        printw("\nDebuff");
        if(!type){attroff(A_STANDOUT); printw(" <");}
        if(type) attron(A_STANDOUT);
        printw("\nBuff");
        if(type){attroff(A_STANDOUT); printw(" <");}
        addch('\n');
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN);

        switch (pos) {
            case KEY_UP:
            case KEY_DOWN:{type = (type+1) & 0b1; break;}
            default:{end = 0; break;}
        }
    }while(end);

    end = 1; st = 0;
    do{
        if(!type){
            max = MAX_DB-2;
            print_debuff(st);
        }
        if(type){
            max = MAX_BF-1;
            print_buff(st);
        }
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN);

        switch (pos) {
            case KEY_UP:{if(st == 0) st = max; else --st; break;}
            case KEY_DOWN:{if(st == max) st = 0; else ++st; break;}
            default:{end = 0; break;}
        }

    }while(end);

    end = 1; max = 0;
    do{
        CLEAR
        printw("\nItem quantity: %d%d", q[0], q[1]);
        move(2, 15+max); addch('|'); move(4, 15+max); addch('|');
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != KEY_LEFT && pos != KEY_RIGHT);

        if(pos == KEY_LEFT || pos == KEY_RIGHT) max = (max + 1) & 0b1;
        else{
            switch (pos) {
                case KEY_UP:{if(q[max] == 9) q[max] = 0; else ++q[max]; break;}
                case KEY_DOWN:{if(q[max] == 0) q[max] = 9; else --q[max]; break;}
                default:{end = 0; break;}
            }
        }

    }while(end);

    cool = q[1] + q[0]*10;

    switch (type) {
        case 0:{inflict_debuff(game.player_list, pla, 0b1<<st, cool); break;}
        case 1:{inflict_buff(game.player_list, pla, 0b1<<st, cool); break;}
    }

}

void inflict_debuff(PLAYER *list, int pla, int status, int cool)
{
    STATUS *aux, *to_free;

    for(int i = 0; i < pla; ++i){
        list = list->next;
    }

    aux = list->st;

    if(list->status & DEAD){
        printw("\nPlayer is dead, can't inflict debuff.\n");
        WAIT
        return;
    }

    if(status & DEAD) inflict_death(list);
    else if(status & UNCON){
        if(list->status & UNCON) return;
        else{
            list->status += UNCON;
            list->hp = 0;
        }
    }
    else if(status & list->status){
        list->status -= status;
        while(aux->next != NULL){
            if(aux->next->status & status){
                to_free = aux->next;
                aux->next = aux->next->next;
                free(to_free);
                return;
            }
            aux = aux->next;
        }

    }
    else{
        list->status += status;
        to_free = (STATUS*)malloc(sizeof(STATUS));
        to_free->status = status;
        to_free->cooldown = cool;

        while(aux->next != NULL){
            if(aux->next->status > status) break;
            else aux = aux->next;
        }

        to_free->next = aux->next;
        aux->next = to_free;
    }

}

void inflict_buff(PLAYER *list, int pla, int status, int cool)
{
    STATUS *aux, *to_free;

    for(int i = 0; i < pla; ++i){
        list = list->next;
    }

    aux = list->bf;

    if(list->status & DEAD){
        printw("\nPlayer is dead, can't inflict debuff.\n");
        WAIT
        return;
    }

    if(status & list->buff){
        list->buff -= status;
        while(aux->next != NULL){
            if(aux->next->status & status){
                to_free = aux->next;
                aux->next = aux->next->next;
                free(to_free);
                return;
            }
        }

    }
    else{
        list->buff += status;
        to_free = (STATUS*)malloc(sizeof(STATUS));
        to_free->status = status;
        to_free->cooldown = cool;
        while(aux->next != NULL){
            if(aux->next->status > status) break;
            else aux = aux->next;
        }
        to_free->next = aux->next;
        aux->next = to_free;
    }
}

void inflict_death(PLAYER *pla)
{
    if(pla->st->next != NULL){
        free_status_list(pla->st->next);
        pla->st->next = NULL;
    }
    if(pla->bf->next != NULL){
        free_status_list(pla->bf->next);
        pla->bf->next = NULL;
    }
    pla->status = DEAD;
    pla->buff = NONE;
    pla->hp = HP_D;
}

void list_inventory(GAME game)
{
    INV *aux_i;
    PLAYER *aux;
    int end = 1, pos, pla = 1;

    do{
        CLEAR
        aux = game.player_list;
        for(int k = 1; k <= game.play_num; ++k){
            aux = aux->next;
            if(k == pla) attron(A_STANDOUT);
            printw("\nPlayer %d: %s", k, aux->name);
            if(k == pla){attroff(A_STANDOUT); printw(" <");}
        }
        addch('\n');
        move(game.win[X]-2, game.win[Y]-2);
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != 27);
        switch (pos) {
            case KEY_UP:{
                if(pla == 1) pla = game.play_num;
                else --pla;
                break;
            }
            case KEY_DOWN:{
                if(pla == game.play_num) pla = 1;
                else ++pla;
                break;
            }
            case 27:{return;break;}
            default:{end = 0; break;}
        }
    }while(end);

    aux = game.player_list;
    for(int i = 0; i < pla; ++i){
        aux = aux->next;
    }

    CLEAR
    printw("\nInventory of Player %s:\n\n", aux->name);

    aux_i = aux->inv;
    while(aux_i->next != NULL){
        aux_i = aux_i->next;
        printw("Item: %s x%d (#%d_%d)\n", aux_i->item->name, aux_i->quantity, aux_i->item->id, aux_i->type);
    }
    WAIT
}

int end_round(GAME game, int *count)
{
    int h, k = 0, c;
    STATUS *aux;
    INV *aux_i;
    ITEM *freed;
    PLAYER *list = game.player_list;

    while(list->next != NULL){
        list = list->next;
        if(list->status & DEAD) continue;
        ++k;
        if(list->status & FIRE) list->hp -= FIRE_DAM;
        if(list->status & POISON) list->hp -= POISON_DAM;
        if(list->status & BLEED) list->hp -= BLEED_DAM;
        if(list->buff & REGEN){
            if(list->hp >= list->max_hp - REGEN_H) list->hp = list->max_hp;
            else list->hp += REGEN_H;
        }

        if(list->hp <= HP_D){inflict_death(list); --k; continue;}
        else if(list->hp <= HP_U && !(list->status & UNCON)) list->status += UNCON;
        else if(list->hp > HP_U  && list->status & UNCON) list->status -= UNCON;

        if(!(list->status & UNCON) && !(list->status & COLD)){
            h = (rand()%2 + 1);
            if(list->ap >= list->max_ap - h) list->ap = list->max_ap;
            else list->ap += h;
        }
        h = 0;
        aux = list->st;
        while(aux->next != NULL){
            if(aux->next->status & TIKI) h = 1;
            if(aux->next->cooldown > 0){
                --aux->next->cooldown;
                if(aux->next->status & TIKI){h = 2; aux = aux->next; continue;}
                if(aux->next->cooldown == 0){
                    inflict_debuff(list, 0, aux->next->status, 0);
                    continue;
                }
            }
            aux = aux->next;
        }
        aux = list->bf;
        while(aux->next != NULL){
            if(aux->next->cooldown > 0){

                --aux->next->cooldown;
                if(aux->next->cooldown == 0){
                    inflict_buff(list, 0, aux->next->status, 0);
                    continue;
                }
            }
            aux = aux->next;
        }
        if(h == 1){
            h = rand()%20 + 1;
            if(h > 16){
                h = 1;
                aux = list->st;
                while(aux->next != NULL){
                    aux = aux->next;
                    if(aux->status & TIKI){
                        aux->cooldown = TIKI_ROUND;
                        h = 0;
                        break;
                    }
                }
                if(h){
                    CLEAR
                    printw("\nSomething went wrong\n");
                    refresh();
                    exit(-1);
                }
                change_item(list, game);
            }
        }
        else if(h == 0){
            aux_i = list->inv;
            while(aux_i->next != NULL){
                aux_i = aux_i->next;
                if(aux_i->type == 3 && aux_i->item->id == 3){h = 1; break;}
            }
            if(h) inflict_debuff(list, 0, TIKI, TIKI_ROUND);
        }
        if(!(list->status & UNCON)){
            aux_i = list->inv;
            while(aux_i->next != NULL){
                aux_i = aux_i->next;
                if(aux_i->type == 3 && aux_i->item->id == 4){
                    if((rand()%100 + 1) == 100){
                        CLEAR
                        printw("\nThe pendant strikes again...");
                        WAIT
                        inflict_debuff(list, 0, UNCON, 1);
                        break;
                    }
                }
            }
        }

    }

    CLEAR
    h = 0;
    while(game.warning->next != NULL){
        c = 0;
        if(game.warning->next->id == 1){
            ++h; c = 1;
            printw("\nWARNING: %s", game.warning->next->name);
            freed = game.warning->next;
            game.warning->next = freed->next;
            free(freed);
            --*count;
        }
        else if(game.warning->next->id > 1) --game.warning->next->id;
        if(!c) game.warning = game.warning->next;
    }
    if(h){refresh(); WAIT}
    return k;
}

void revive(GAME game)
{
    PLAYER *aux;
    int end = 1, pos, pla = 1;

    do{
        CLEAR
        aux = game.player_list;
        for(int k = 1; k <= game.play_num; ++k){
            aux = aux->next;
            if(k == pla) attron(A_STANDOUT);
            printw("\nPlayer %d: %s", k, aux->name);
            if(k == pla){attroff(A_STANDOUT); printw(" <");}
        }
        addch('\n');
        move(game.win[X]-2, game.win[Y]-2);
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != 27);
        switch (pos) {
            case KEY_UP:{
                if(pla == 1) pla = game.play_num;
                else --pla;
                break;
            }
            case KEY_DOWN:{
                if(pla == game.play_num) pla = 1;
                else ++pla;
                break;
            }
            case 27:{return;break;}
            default:{end = 0; break;}
        }
    }while(end);

    aux = game.player_list;
    for(int i = 0; i < pla; ++i){
        aux = aux->next;
    }

    if(!(aux->status & DEAD)){
        printw("\nPlayer is not dead, can't revive!\n");
        WAIT
        return;
    }

    aux->status -= DEAD;
    aux->hp = aux->max_hp;
    aux->ap = aux->max_ap;

}

void ac_change(GAME game)
{
    PLAYER *aux;
    int end = 1, pos, pla = 1;

    do{
        CLEAR
        aux = game.player_list;
        for(int k = 1; k <= game.play_num; ++k){
            aux = aux->next;
            if(k == pla) attron(A_STANDOUT);
            printw("\nPlayer %d: %s", k, aux->name);
            if(k == pla){attroff(A_STANDOUT); printw(" <");}
        }
        addch('\n');
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != 27);
        switch (pos) {
            case KEY_UP:{
                if(pla == 1) pla = game.play_num;
                else --pla;
                break;
            }
            case KEY_DOWN:{
                if(pla == game.play_num) pla = 1;
                else ++pla;
                break;
            }
            case 27:{return;break;}
            default:{end = 0; break;}
        }
    }while(end);
    aux = game.player_list;
    for(int i = 0; i < pla; ++i){
        aux = aux->next;
    }

    pla = end = 1;
    do{
        CLEAR
        printw("\nInput the new AC\n: < %d >", pla);
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_LEFT && pos != KEY_RIGHT && pos != 27);

        switch (pos) {
            case KEY_LEFT:{
                if(pla == 1) pla = MAX_AC;
                else --pla;
                break;
            }
            case KEY_RIGHT:{
                if(pla == MAX_AC) pla = 1;
                else ++pla;
                break;
            }
            case 27:{return;break;}
            default:{
                end = 0;
                break;
            }
        }

    }while(end);
    aux->ac = pla;
}

void change_item(PLAYER *pla, GAME game)
{
    INV *aux = pla->inv;
    ITEM *aux_i;
    int count = 0;

    while(aux->next != NULL){
        aux = aux->next;
        if(aux->type == 1) ++count;
        else break;
    }

    if(!count){
        CLEAR
        printw("\nTIKI\n\nNo items to change.");
        WAIT
        return;
    }

    count = (rand()%count)+1;
    aux = pla->inv;
    for(int i = 0; i < count; ++i) aux = aux->next;

    count = (rand()% game.item_num)+1;
    aux_i = game.item;
    for(int i = 0; i < count; ++i) aux_i = aux_i->next;

    CLEAR
    printw("\nTIKI\n\nPlayer %s suffered an item change: %s to %s (qua = %d).", pla->name, aux->item->name, aux_i->name, aux->quantity);
    WAIT

    aux->item = aux_i;

}

void dice_roll()
{

    int end, roll, q[3] = {0}, pos, dice, k;

    end = 1; roll = k = 0;
    do{
        CLEAR
        printw("\nSelect dice roll: %dd%d%d%d\n", roll, q[0], q[1], q[2]);
        switch (k) {
            case 0:{move(2, 18); addch('|'); move(4, 18); addch('|'); break;}
            case 1:{move(2, 20); addch('|'); move(4, 20); addch('|'); break;}
            case 2:{move(2, 21); addch('|'); move(4, 21); addch('|'); break;}
            case 3:{move(2, 22); addch('|'); move(4, 22); addch('|'); break;}
        }
        refresh();
        do{
            pos = getch();
        }while(pos != 10 && pos != 27 && pos != KEY_UP && pos != KEY_DOWN && pos != KEY_LEFT && pos != KEY_RIGHT);

        switch (pos) {
            case KEY_UP:{
                if(!k){
                    if(roll == 9) roll = 0; else ++roll;
                }
                else{
                    if(q[k-1] == 9) q[k-1] = 0; else ++q[k-1];
                }
                break;
            }
            case KEY_DOWN:{
                if(!k){
                    if(!roll) roll = 9; else --roll;
                }
                else{
                    if(!q[k-1]) q[k-1] = 9; else --q[k-1];
                }
                break;
            }
            case KEY_LEFT:{if(!k) k = 3; else --k; break;}
            case KEY_RIGHT:{if(k == 3) k = 0; else ++k; break;}
            case 27:{return;break;}
            default:{end = 0; break;}
        }

    }while(end);

    dice = q[2] + 10*q[1] + 100*q[0];

    k = 0;
    if(dice && roll) for(int i = 0; i < roll; ++i) k += (rand()%dice) + 1;

    CLEAR
    printw("\nRoll: %d\n", k);
    WAIT
}

void add_warning(GAME game, int *count)
{
    ITEM *aux = (ITEM*)malloc(sizeof(ITEM));
    int end, pos, k, q[3] = {0};

    CLEAR
    echo();
    printw("\nEnter your message: ");
    getstr(aux->name);
    noecho();

    end = 1; k = 0;
    do{
        CLEAR
        printw("\nEnter the number of rounds: %d%d%d", q[0], q[1], q[2]);
        switch (k) {
            case 0:{move(2, 28); addch('|'); move(4, 28); addch('|'); break;}
            case 1:{move(2, 29); addch('|'); move(4, 29); addch('|'); break;}
            case 2:{move(2, 30); addch('|'); move(4, 30); addch('|'); break;}
        }

        do{
            pos = getch();
        }while(pos != 10 && pos != KEY_UP && pos != KEY_DOWN && pos != KEY_LEFT && pos != KEY_RIGHT);

        switch (pos) {
            case KEY_UP:{if(q[k] == 9) q[k] = 0; else ++q[k]; break;}
            case KEY_DOWN:{if(!q[k]) q[k] = 9; else --q[k]; break;}
            case KEY_LEFT:{if(!k) k = 2; else --k; break;}
            case KEY_RIGHT:{if(k == 2) k = 0; else ++k; break;}
            default:{end = 0; break;}
        }

    }while(end);

    aux->id = q[0]*100 + q[1]*10 + q[2];
    ++*count;
    while(game.warning->next != NULL){
        if(game.warning->next->id > aux->id){
            break;
        }
        else{
            game.warning = game.warning->next;
        }
    }

    aux->next = game.warning->next;
    game.warning->next = aux;

}

void remove_warn(GAME game, int *count)
{
    int end = 1, pos = 1, key;
    ITEM *aux;

    if(game.warning->next == NULL){
        CLEAR
        printw("\n No messages to erase.");
        WAIT
        return;
    }

    do{
        CLEAR
        aux = game.warning;
        printw("\nChoose a message to remove:\n");
        for(int i = 1; aux->next != NULL; ++i){
            aux = aux->next;
            if(i == pos) attron(A_UNDERLINE);
            printw("\n%s", aux->name);
            if(i == pos){attroff(A_UNDERLINE); printw(" <");}
        }
        printw("\n");
        do{
            key = getch();
        }while(key != 10 && key != KEY_UP && key != KEY_DOWN && key != 27);

        switch (key) {
            case KEY_UP:{
                if(pos == 1) pos = *count;
                else --pos;
                break;
            }
            case KEY_DOWN:{
                if(pos == *count) pos = 1;
                else ++pos;
                break;
            }
            case 10:{
                end = 0;
                break;
            }
            case 27:{
                return; break;
            }
        }
    }while(end);

    for(int i = 0; i < pos-1; ++i){
        game.warning = game.warning->next;
    }
    aux = game.warning->next;
    game.warning->next = aux->next;
    --*count;
    free(aux);
}

void change_class(GAME game)
{
    int key, pos = 1, end = 1;
    PLAYER *aux;
    CLASS *aux_c;

    do{
        CLEAR
        aux = game.player_list;
        for(int k = 1; k <= game.play_num; ++k){
            aux = aux->next;
            if(k == pos) attron(A_STANDOUT);
            printw("\nPlayer %d: %s; Class %s", k, aux->name, aux->classe->name);
            if(k == pos){attroff(A_STANDOUT); printw(" <");}
        }
        addch('\n');
        move(game.win[X]-2, game.win[Y]-2);
        refresh();
        do{
            key = getch();
        }while(key != 10 && key != KEY_UP && key != KEY_DOWN && key != 27);
        switch (key) {
            case KEY_UP:{
                if(pos == 1) pos = game.play_num;
                else --pos;
                break;
            }
            case KEY_DOWN:{
                if(pos == game.play_num) pos = 1;
                else ++pos;
                break;
            }
            case 27:{return;break;}
            default:{end = 0; break;}
        }
    }while(end);

    aux = game.player_list;
    for(int i = 0; i < pos; ++i){
        aux = aux->next;
    }

    end = pos = 1;
    do{
        CLEAR
        printw("\nChoose a new class:");
        list_class_info(game.class_info, pos);
        refresh();

        do{
            key = getch();
        }while(key != KEY_UP && key != KEY_DOWN && key != 10 && key != 27);

        switch (key) {
            case KEY_UP:{
                if(pos == 1) pos = game.class_num;
                else --pos;
                break;
            }
            case KEY_DOWN:{
                if(pos == game.class_num) pos = 1;
                else ++pos;
                break;
            }
            case 10:{
                end = 0;
                break;
            }
            case 27:{
                return;
                break;
            }
        }
    }while(end);

    aux_c = game.class_info;
    for(int i = 0; i < pos; ++i){
        aux_c = aux_c->next;
    }

    if(aux_c->pos == aux->classe->pos) return;
    for(int i = 0; i < STAT_NUMBER; ++i){
        aux->stats[i] = aux->stats[i] - aux->classe->base_stats[i] + aux_c->base_stats[i];
    }
    aux->classe = aux_c;
}

void kill_game(GAME game)
{
    char kill_file[NAME_SIZE+7];

    CLEAR
    printw("Round %d\n\n", game.round);
    list_players(game.player_list);
    printw("\nAll players are dead, game over!\n");
    WAIT

    sprintf(kill_file, "rm -r %s", game.game_name);
    system(kill_file);
}




void print_opt(int pos)
{
    if(pos == 0) attron(A_STANDOUT);
    printw("\nHP and AP change");
    if(pos == 0){attroff(A_STANDOUT); printw(" <");}
    if(pos == 1) attron(A_STANDOUT);
    printw("\nShow player stats");
    if(pos == 1){attroff(A_STANDOUT); printw(" <");}
    if(pos == 2) attron(A_STANDOUT);
    printw("\nList inventory");
    if(pos == 2){attroff(A_STANDOUT); printw(" <");}
    if(pos == 3) attron(A_STANDOUT);
    printw("\nAdd to inventory");
    if(pos == 3){attroff(A_STANDOUT); printw(" <");}
    if(pos == 4) attron(A_STANDOUT);
    printw("\nRemove from inventory");
    if(pos == 4){attroff(A_STANDOUT); printw(" <");}
    if(pos == 5) attron(A_STANDOUT);
    printw("\nAdd Exp and Gold");
    if(pos == 5){attroff(A_STANDOUT); printw(" <");}
    if(pos == 6) attron(A_STANDOUT);
    printw("\nReward end of Quest");
    if(pos == 6){attroff(A_STANDOUT); printw(" <");}
    if(pos == 7) attron(A_STANDOUT);
    printw("\nEsuna");
    if(pos == 7){attroff(A_STANDOUT); printw(" <");}
    if(pos == 8) attron(A_STANDOUT);
    printw("\nAdd/Remove Status Ailment");
    if(pos == 8){attroff(A_STANDOUT); printw(" <");}
    if(pos == 9) attron(A_STANDOUT);
    printw("\nEnd Round ");
    if(pos == 9){attroff(A_STANDOUT); printw(" <");}
    if(pos == 10) attron(A_STANDOUT);
    printw("\nRevive Player");
    if(pos == 10){attroff(A_STANDOUT); printw(" <");}
    if(pos == 11) attron(A_STANDOUT);
    printw("\nChange AC");
    if(pos == 11){attroff(A_STANDOUT); printw(" <");}
    if(pos == 12) attron(A_STANDOUT);
    printw("\nRoll dice");
    if(pos == 12){attroff(A_STANDOUT); printw(" <");}
    if(pos == 13) attron(A_STANDOUT);
    printw("\nAdd message");
    if(pos == 13){attroff(A_STANDOUT); printw(" <");}
    if(pos == 14) attron(A_STANDOUT);
    printw("\nRemove message");
    if(pos == 14){attroff(A_STANDOUT); printw(" <");}
    if(pos == 15) attron(A_STANDOUT);
    printw("\nChange class");
    if(pos == 15){attroff(A_STANDOUT); printw(" <");}
    if(pos == 16) attron(A_STANDOUT);
    printw("\nSave and Quit");
    if(pos == 16){attroff(A_STANDOUT); printw(" <");}
}

void print_debuff(int pos)
{
    CLEAR

    printw("\nChoose a debuff:\n");

    if(pos == 0) attron(A_STANDOUT);
    printw("\nDeath");
    if(pos == 0){attroff(A_STANDOUT); printw(" <");}
    if(pos == 1) attron(A_STANDOUT);
    printw("\nUnconscious");
    if(pos == 1){attroff(A_STANDOUT); printw(" <");}
    if(pos == 2) attron(A_STANDOUT);
    printw("\nPoison");
    if(pos == 2){attroff(A_STANDOUT); printw(" <");}
    if(pos == 3) attron(A_STANDOUT);
    printw("\nBleed");
    if(pos == 3){attroff(A_STANDOUT); printw(" <");}
    if(pos == 4) attron(A_STANDOUT);
    printw("\nBlind");
    if(pos == 4){attroff(A_STANDOUT); printw(" <");}
    if(pos == 5) attron(A_STANDOUT);
    printw("\nSilence");
    if(pos == 5){attroff(A_STANDOUT); printw(" <");}
    if(pos == 6) attron(A_STANDOUT);
    printw("\nStoned");
    if(pos == 6){attroff(A_STANDOUT); printw(" <");}
    if(pos == 7) attron(A_STANDOUT);
    printw("\nBeserk");
    if(pos == 7){attroff(A_STANDOUT); printw(" <");}
    if(pos == 8) attron(A_STANDOUT);
    printw("\nFire");
    if(pos == 8){attroff(A_STANDOUT); printw(" <");}
    if(pos == 9) attron(A_STANDOUT);
    printw("\nCold");
    if(pos == 9){attroff(A_STANDOUT); printw(" <");}
    addch('\n');
    refresh();
}

void print_buff(int pos)
{
    CLEAR

    printw("\nChoose a buff:\n");

    if(pos == 0) attron(A_STANDOUT);
    printw("\nRegen");
    if(pos == 0){attroff(A_STANDOUT); printw(" <");}
    if(pos == 1) attron(A_STANDOUT);
    printw("\nGiant");
    if(pos == 1){attroff(A_STANDOUT); printw(" <");}
    if(pos == 2) attron(A_STANDOUT);
    printw("\nProtect");
    if(pos == 2){attroff(A_STANDOUT); printw(" <");}
    if(pos == 3) attron(A_STANDOUT);
    printw("\nFast");
    if(pos == 3){attroff(A_STANDOUT); printw(" <");}
    if(pos == 4) attron(A_STANDOUT);
    printw("\nInvisible");
    if(pos == 4){attroff(A_STANDOUT); printw(" <");}
    addch('\n');
    refresh();
}

void print_stats(GAME game)
{
    int i = 0;
    PLAYER *list;
    CLEAR

    list = game.player_list;
    printw("\nShowing player stats:\n");
    while(list->next != NULL){
        ++i;
        list = list->next;
        printw("\nPlayer %d: %s\n", i,list->name);
        printw("Stats: St = %d, De = %d, Co = %d, ", list->stats[0], list->stats[1], list->stats[2]);
        printw("In = %d, Wi = %d, Ch = %d\n", list->stats[3], list->stats[4], list->stats[5]);

    }
    refresh();
    WAIT
}

void print_warnings(GAME game)
{
    ITEM *aux = game.warning;

    move(37, 60);
    attron(A_UNDERLINE);
    printw("Messages:");
    attroff(A_UNDERLINE);

    for(int i = 0; aux->next != NULL && i < 76; ++i){
        aux = aux->next;
        move(i%19 + 39, 60 + NAME_SIZE*(i/19));
        printw("%s - rounds: %d", aux->name, aux->id);
    }
}




void draw_dragon()
{
    system("clear");
    system("cat Settings/Dragon.asc2ii");
}
