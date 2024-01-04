// a drawing program using ncurses
// arrow to move cursor
// `1` to choose background color
// `2` to `8` to choose other color
// `esc` to quit

#include <ncurses.h>

#include <string>
using std::string;
using std::to_string;

#define KEY_SPACE char(32)
#define KEY_ESCAPE char(27)

int main(int argc, char** argv) {
    initscr();
    keypad(stdscr, TRUE);
    start_color();
    use_default_colors();
    init_pair(1, -2, -1);

    // generate default color
    for(int i = 0; i <= 7; i++)
        init_pair(i + 2, i, i);

    wbkgd(stdscr, COLOR_PAIR(1));

    int posx = 1;
    int posy = 1;
    int current_color = 1;

    char chr = ' ';
    bool running = true;
    while(running) {
        attron(COLOR_PAIR(current_color));
        mvaddch(posy, posx, ' ');
        attroff(COLOR_PAIR(current_color));
        refresh();

        chr = getch();
        // delete typed char
        attron(COLOR_PAIR(1));
        mvaddch(posy, posx + 1, ' ');
        attroff(COLOR_PAIR(1));
        
        // choose color
        int clr = chr - 48;
        if(clr >= 1 and clr <= 9) current_color = clr;
        else {
            switch(chr) {
            case (char)KEY_UP:
                --posy;
                break;
            case (char)KEY_DOWN:
                ++posy;
                break;
            case (char)KEY_LEFT:
                --posx;
                break;
            case (char)KEY_RIGHT:
                ++posx;
                break;
            case KEY_ESCAPE:
                running = false;
                break;
            }
        }
    }

    endwin();
    return 0;
}
