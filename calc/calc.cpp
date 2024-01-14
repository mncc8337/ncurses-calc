// a drawing program
// use mouse to move cursor, hold button1 to draw
// <1> to choose eraser
// <2> to <8> to choose color
// <9> to clear
// <esc> to quit

#include <ncurses.h>

#include <cstring>
#include <string>
using std::string;
using std::to_string;

#include <iostream>
using std::cout;

#include <vector>
using std::vector;

#include "calc.h"

#define KEY_ENTER 10
#define KEY_SPACE 32
#define KEY_ESCAPE 27

#define KEY_W 119
#define KEY_A 97
#define KEY_S 115
#define KEY_D 100

struct button_prop {
    int x;
    int y;
    int width;
    int height;
    int color;
    string text;
};

vector<WINDOW*> button;
vector<button_prop> prop;

int create_button(int startx, int starty, int width, int height, string text) {
    button.push_back(newwin(height, width, starty, startx));
    prop.push_back({startx, starty, width, height, 1, text});

    return button.size()-1;
}
void destroy_button(uint i) {
    wborder(button[i], ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(button[i]);
    delwin(button[i]);
    button.erase(button.begin() + i);
}

uint button_num = 4;
uint selected = 0;

void redraw_button() {
    for(int i = 0; i < button_num; i++) {
        box(button[i], 0, 0);
        mvwprintw(button[i],
                  prop[i].height/2, (prop[i].width - prop[i].text.length())/2,
                  prop[i].text.c_str());
        wbkgd(button[i], COLOR_PAIR(prop[i].color));
        wrefresh(button[i]);
    }
}

void submit() {
    
}

void clamp(int& val, int min, int max) {
    if(val < min) val = min;
    if(val > max) val = max;
}

const int LIMIT = 512;
string num1 = "";
string num2 = "";

enum OPERATOR {
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4
};

string* selecting_num;
char op = -1;

int main(int argc, char** argv) {
    selecting_num = &num1;

    initscr();

    start_color();
    use_default_colors();
    init_pair(1, -2, -1); // normal
    init_pair(1, -1, -2); // invert

    wbkgd(stdscr, COLOR_PAIR(1));

    keypad(stdscr, TRUE);

    int buttonw = 25;
    int buttonh = 3;
    create_button(0, 2, buttonw, buttonh, "+");
    create_button(0, 2+buttonh, buttonw, buttonh, "-");
    create_button(0+buttonw, 2, buttonw, buttonh, "*");
    create_button(0+buttonw, 2+buttonh, buttonw, buttonh, "/");

    int selectx = 0;
    int selecty = 0;

    int posx = 0;
    int posy = 0;
    
    int chr = ' ';
    bool running = true;
    while(running) {
        clamp(selectx, 0, 1);
        clamp(selecty, 0, 1);
        for(int i = 0; i < 4; i++)
            prop[i].color = 1;
        prop[selectx * 2 + selecty].color = 2;

        refresh();
        redraw_button();

        move(posy, posx);

        chr = getch();

        int ord = chr - 48;
        if(ord >= 0 and ord <= 9 and posx < LIMIT) {
            *selecting_num += tochr(ord);
            posx++;
        }
    
        if(chr == KEY_ESCAPE)
            running = false;
        else if(chr == KEY_SPACE or chr == KEY_ENTER) {
            for(int i = 0; i < LIMIT; i++) mvaddch(1, i, ' ');
            if(op == -1) {
                op = selectx * 2 + selecty + 1;
                posx = 0;
                selecting_num = &num2;
            }
            else {
                string ans = "not implemented";
                switch(op) {
                    case ADD:
                        ans = add(num1, num2);
                        break;
                    case SUB:
                        ans = sub(num1, num2);
                        break;
                    case MUL:
                        ans = mul(num1, num2);
                        break;
                }
                mvprintw(1, (50 - ans.length()), ans.c_str());

                posx = 0;
                op = -1;
                num1 = ""; num2 = "";
                selecting_num = &num1;
            }
            for(int i = 0; i < LIMIT; i++) mvaddch(0, i, ' ');
        }
        else if(chr == KEY_UP or chr == KEY_W)
            selecty -= 1;
        else if(chr == KEY_DOWN or chr == KEY_S)
            selecty += 1;
        else if(chr == KEY_LEFT or chr == KEY_A)
            selectx -= 1;
        else if(chr == KEY_RIGHT or chr == KEY_D)
            selectx += 1;
    }

    endwin();
    return 0;
}
