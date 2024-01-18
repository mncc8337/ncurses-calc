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

#define KEY_RETURN 10
#define KEY_SPACE 32
#define KEY_ESCAPE 27
#define KEY_BACKDEL 263

#define KEY_W 119
#define KEY_A 97
#define KEY_S 115
#define KEY_D 100

int num_len(int num) {
    int cnt = 1;
    while((num /= 10) > 0) cnt++;
    return cnt;
}

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
void config_button(int i, int startx, int starty, int width, int height, string text) {
    wborder(button[i], ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(button[i]);
    delwin(button[i]);

    button[i] = newwin(height, width, starty, startx);
    prop[i] = {startx, starty, width, height, prop[i].color, text};
}
void destroy_button(int i) {
    wborder(button[i], ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
    wrefresh(button[i]);
    delwin(button[i]);
    button.erase(button.begin() + i);
}

int button_num = 4;
int selected = 0;

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

void clamp(int& val, int min, int max) {
    if(val < min) val = min;
    if(val > max) val = max;
}

string num1 = "";
string num2 = "";
string ans;

enum OPERATOR {
    ADD = 1,
    SUB = 2,
    MUL = 3,
    DIV = 4
};

string get_curr_op(int op) {
    switch(op) {
        case ADD:
            return "ADD";
            break;
        case SUB:
            return "SUB";
            break;
        case MUL:
            return "MUL";
            break;
        case DIV:
            return "DIV";
            break;
        case -2:
            return "RESULT";
        default:
            return "NONE";
    }
}

string* selecting_num;
char op = -1;

int main(int argc, char** argv) {
    selecting_num = &num1;

    initscr();

    start_color();
    use_default_colors();
    init_pair(1, -2, -1); // normal
    init_pair(2, 1, -1); // red
    init_pair(3, 4, -1); // blue

    wbkgd(stdscr, COLOR_PAIR(1));

    keypad(stdscr, TRUE);

    int buttonw = -1;
    int buttonh = -1;
    create_button(0, 0, 0, 0, "+");
    create_button(0, 0, 0, 0, "-");
    create_button(0, 0, 0, 0, "*");
    create_button(0, 0, 0, 0, "/");

    int selectx = 0;
    int selecty = 0;

    int oselectx = 1;
    int oselecty = 1;

    int posx = 0;

    int wwidth = 0;
    int wheight = 0;
    int startx, starty;

    int owwidth = -1;
    int owheight = -1;

    string title = "CALC ver 0.0.1";
    string mode = "NONE";
    string empty(1, ' ');
    
    int chr = '-';
    bool running = true;
    while(running) {
        wrefresh(stdscr);

        getmaxyx(stdscr, wheight, wwidth);
        if(wwidth < 20 or wheight < 18) {
            string msg = "term size too small";
            string dim = to_string(wwidth) + 'x' + to_string(wheight);
            clear();
            mvprintw(wheight/2, (wwidth-msg.length())/2, msg.c_str());
            mvprintw(wheight/2+1, (wwidth-dim.length())/2, dim.c_str());
            getch();
            continue;
        }

        // buttons
        if(owwidth != wwidth or owheight != wheight) {
            clear();

            buttonw = wwidth/2-2;
            buttonh = (wheight - 3)/5;
            startx = (wwidth - buttonw*2)/2;
            starty = (wheight-4-buttonh*2)/2;
            config_button(0, startx, starty+4, buttonw, buttonh, "+");
            config_button(1, startx, starty+4+buttonh, buttonw, buttonh, "-");
            config_button(2, startx+buttonw, starty+4, buttonw, buttonh, "*");
            config_button(3, startx+buttonw, starty+4+buttonh, buttonw, buttonh, "/");

            // redraw prev result
            if(op == -2) {
                attron(COLOR_PAIR(2) | A_BOLD);
                mvprintw(starty+3, startx + (buttonw*2 - ans.length()), ans.c_str());
                attroff(COLOR_PAIR(2) | A_BOLD);
            }

            // redraw deleted input chars
            int offset = 0;
            if(op != -1 and op != -2) {
                offset = num1.length() - posx;
                if(offset < 0) offset = 0;
            }
            attron(COLOR_PAIR(3));
            mvprintw(starty+2, startx + offset, selecting_num->c_str());
            attroff(COLOR_PAIR(3));

            wrefresh(stdscr);
            redraw_button();

            empty = string(buttonw*2, ' ');
            redraw_button();
        }
        owwidth = wwidth; owheight = wheight;

        clamp(selectx, 0, 1);
        clamp(selecty, 0, 1);
        if(oselectx != selectx or oselecty != selecty) {
            for(int i = 0; i < 4; i++) prop[i].color = 1;
            prop[selectx * 2 + selecty].color = 2;
            redraw_button();
        }
        oselectx = selectx;
        oselecty = selecty;


        // draw title
        string t(wwidth, ' ');
        std::copy(title.begin(), title.end(), t.begin());
        std::copy(mode.begin(), mode.end(), t.end() - mode.length());

        attron(COLOR_PAIR(2) | A_BOLD | A_REVERSE);
        mvprintw(0, 0, t.c_str());
        attroff(COLOR_PAIR(2) | A_BOLD | A_REVERSE);

        // draw num1
        if(op != -1 and op != -2) {
            int offset = (posx - num1.length());
            if(offset < 0) offset = 0;

            string n1 = empty;
            std::copy(num1.begin(), num1.end(), n1.begin() + offset);

            attron(COLOR_PAIR(1) | A_DIM);
            mvprintw(starty+1, startx, n1.c_str());
            attroff(COLOR_PAIR(1) | A_DIM);
        }

        // draw num2
        auto attr = COLOR_PAIR(3);
        int offset = 0;
        if(op != -1 and op != -2) {
            offset = num1.length() - posx;
            if(offset < 0) offset = 0;
        }
        string in = empty;
        if(op == -2) {
            op = -1;
            attr = COLOR_PAIR(1) | A_DIM;
            offset = num1.length() - num2.length();
            if(offset < 0) offset = 0;
            std::copy(num2.begin(), num2.end(), in.begin()+offset);
            num1 = "";
        }
        else
            std::copy(selecting_num->begin(), selecting_num->end(), in.begin()+offset);

        attron(attr);
        mvprintw(starty+2, startx, in.c_str());
        chr = mvgetch(starty+2, startx + posx + offset);
        attroff(attr);

        // delete typed char
        mvaddch(starty+2, startx + posx + offset, ' ');

        // clear result
        mvprintw(starty+3, startx, empty.c_str());

        if(op == -1)    
            mvprintw(starty+1, startx, empty.c_str());

        int ord = chr - 48;
        if(ord >= 0 and ord <= 9 and posx < buttonw*2 and (ord != 0 or posx != 1 or (*selecting_num)[0] != '0')) {
            *selecting_num += tochr(ord);
            posx++;
        }
    
        else if(chr == KEY_ESCAPE)
            running = false;
        else if(chr == KEY_BACKDEL and posx > 0) {
            posx--;
            selecting_num->erase(selecting_num->end()-1, selecting_num->end());
        }
        else if(chr == KEY_SPACE or chr == KEY_RETURN) {
            if(op == -1 and num1 != "") {
                op = selectx * 2 + selecty + 1;
                mode = get_curr_op(op);
                posx = 0;
                num2 = "";
                selecting_num = &num2;
            }
            else if(num2 != "" and num1 != "") {
                ans = "not implemented";
                string mod = "";
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
                    case DIV:
                        ans = div(num1, num2, mod);
                        mod = '+' + mod;
                        break;
                }

                mode = "RESULT";

                // avoid overflow
                if(ans.length() + mod.length() > buttonw*2) {
                    int deg = ans.length() + mod.length() - buttonw*2;
                    ans = ans.substr(0, buttonw*2);
                    while(ans.length() + mod.length() + num_len(deg) + 1 > buttonw*2) {
                        deg++;
                        ans = ans.substr(0, ans.length()-1);
                    }

                    ans += 'e' + to_string(deg);
                }
                ans += mod;

                string a = empty;
                std::copy(ans.begin(), ans.end(), a.end() - ans.length());
                attron(COLOR_PAIR(2) | A_BOLD);
                mvprintw(starty+3, startx, a.c_str());
                attroff(COLOR_PAIR(2) | A_BOLD);

                posx = 0;
                op = -2;
                selecting_num = &num1;
            }
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

    for(int i = 0; i < button_num; i++) delwin(button[i]);
    endwin();
    return 0;
}
