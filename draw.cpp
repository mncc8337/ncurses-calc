// a drawing program
// use mouse to move cursor, hold button1 to draw
// <1> to choose eraser
// <2> to <8> to choose color
// <9> to clear
// `esc` to quit

#include <ncurses.h>

#include <cstring>
#include <string>
using std::string;
using std::to_string;

#define KEY_SPACE 32
#define KEY_ESCAPE 27

int main(int argc, char** argv) {
    initscr();
    noecho();
    cbreak();

    keypad(stdscr, TRUE);

    start_color();
    use_default_colors();
    init_pair(1, -2, -1);
    // generate default color
    for(int i = 0; i <= 7; i++)
        init_pair(i + 2, i, i);

    // clear screen
    wbkgd(stdscr, COLOR_PAIR(1));

    // mouse event
    mousemask(REPORT_MOUSE_POSITION | BUTTON1_PRESSED | BUTTON1_RELEASED, NULL);
    mouseinterval(0); // remove mouse interval
    printf("\033[?1003h\n"); // enable mouse movement report
    MEVENT event;
    bool mouse_pressed = false;

    int posx = 1;
    int posy = 1;
    int current_color = 2;

    int width, height;

    char debugmsg[512] = "cursed painter";

    int chr = ' ';
    bool running = true;
    while(running) {
        getmaxyx(stdscr, height, width);

        // print UI
        attron(COLOR_PAIR(1));
        mvprintw(1, 1, debugmsg);
        mvprintw(height-1, width - 10, (to_string(posx) + "  ").c_str());
        mvprintw(height-1, width - 5, (to_string(posy) + "  ").c_str());
        attroff(COLOR_PAIR(1));

        if(mouse_pressed) {
            attron(COLOR_PAIR(current_color));
            mvaddch(posy, posx, ' ');
            attroff(COLOR_PAIR(current_color));
        }
        else move(posy, posx);

        refresh();

        chr = getch();

        // choose color
        int clr = chr - 48;
        if(clr >= 1 and clr <= 9) {
            current_color = clr;
            sprintf(debugmsg, "set to color %d", clr);
        }
        else if(clr == 0) {
            clear();
            strncpy(debugmsg, "cleared", 512);
        }
    
        switch(chr) {
            case KEY_MOUSE:
            if(getmouse(&event) == OK) {
                if(event.bstate & BUTTON1_PRESSED)
                    mouse_pressed = true;
                if(event.bstate & BUTTON1_RELEASED)
                    mouse_pressed = false;

                posx = event.x;
                posy = event.y;
            }
            break;

            case KEY_ESCAPE:
            running = false;
            break;

            default:
            // delete typed char
            attron(COLOR_PAIR(1));
            mvaddch(posy, posx + 1, ' ');
            attroff(COLOR_PAIR(1));
        }
    }

    endwin();
    printf("\033[?1003l\n"); // disable mouse movement report
    return 0;
}
