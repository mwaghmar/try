/*
 * boxes.c:
 * Simple mouse-driven ASCII-art drawing program.
 *
 * Copyright (c) 2002 Chris Lightfoot. All rights reserved.
 * Email: chris@ex-parrot.com; WWW: http://www.ex-parrot.com/~chris/
 *
 */

#include <curses.h>
#include <stdlib.h>

/* Flags for the lines in various directions. */
#define L_UP    1
#define L_DOWN  2
#define L_LEFT  4
#define L_RIGHT 8

struct cell {
    /* Directions to which this cell is connected by lines. */
    unsigned int lines;
    /* Does this cell have a character? */
    char c;
};

struct cell **cells_alloc(const int w, const int h) {
    struct cell **C, **p, *q;
    C = malloc(h * sizeof(struct cell*) + w * h * sizeof(struct cell));
    for (p = C, q = (struct cell*)((char*)C + h * sizeof(*C)); p < C + h; ++p, q += w)
        *p = q;
    return C;
}

int min(const int a, const int b) {
    return (a > b) ? b : a;
}

int max(const int a, const int b) {
    return (a > b) ? a : b;
}

void vertical_line(struct cell **d, const int x, const int y1, const int y2) {
    int Y1, Y2, y;
    if (y1 == y2)
        return;
    else if (y1 > y2) {
        Y1 = y2;
        Y2 = y1;
    } else {
        Y1 = y1;
        Y2 = y2;
    }

    d[Y1][x].lines |= L_DOWN;
    d[Y2][x].lines |= L_UP;
    for (y = Y1 + 1; y < Y2; ++y)
        d[y][x].lines |= L_DOWN | L_UP;
}

void horizontal_line(struct cell **d, const int x1, const int x2, const int y) {
    int X1, X2, x;
    if (x1 == x2)
        return;
    else if (x1 > x2) {
        X1 = x2;
        X2 = x1;
    } else {
        X1 = x1;
        X2 = x2;
    }

    d[y][X1].lines |= L_RIGHT;
    d[y][X2].lines |= L_LEFT;
    for (x = X1 + 1; x < X2; ++x)
        d[y][x].lines |= L_LEFT | L_RIGHT;
}

int get_line_character(const unsigned int l) {
    switch (l) {
        case 0:
            return ' ';
                
        case L_UP:
        case L_DOWN | L_UP:
        case L_DOWN:
            return '|';

        case L_LEFT:
        case L_LEFT | L_RIGHT:
        case L_RIGHT:
            return '-';

        case L_LEFT | L_UP:
            return '\'';

        case L_RIGHT | L_UP:
            return '`';

        case L_LEFT | L_DOWN:
        case L_RIGHT | L_DOWN:
            return '.';

        default:
            return '+';
    }
}

int visual_feedback = 1;

void flash_box(const int x0, const int y0, const int x1, const int y1) {
    int j;
    
/*    if (!visual_feedback)
        return;*/

    for (j = y0; j <= y1; ++j)
        mvchgat(j, x0, -1 /*x1 - x0 + 1*/, A_REVERSE, 0, NULL);
    refresh();

    usleep(500000);
    
    for (j = y0; j <= y1; ++j)
        mvchgat(j, x0, x1 - x0 + 1, A_NORMAL, 0, NULL);
    refresh();
}

void display_drawing(struct cell **d, const int w, const int h) {
    int x, y;
    move(0, 0);
    for (y = 0; y < h; ++y)
        for (x = 0; x < w; ++x)
            if (d[y][x].c)
                addch(d[y][x].c);
            else
                addch(get_line_character(d[y][x].lines));
    refresh();
}

void flash_modelabel(const char *str, struct cell **d) {
    attron(A_REVERSE | A_BOLD);
    move(0, 0);
    addstr(" ");
    addstr(str);
    addstr(" ");
    attroff(A_REVERSE | A_BOLD);
    move(LINES - 1, COLS - 1);
    refresh();
    usleep(500000);
    display_drawing(d, COLS, LINES);
    refresh();
}

int find_end_of_text_block(struct cell **d, const int x, const int y, const int w) {
    int i;
    for (i = x; i < w && d[y][i].c; ++i);
    return i;
}

int main(int argc, char *argv[]) {
    struct cell **drawing;
    int i;
    enum mode_t { lines = 0, boxes = 1, text = 2, erase = 3 } mode = lines;
    char *mode_name[] = { "Draw lines", "Draw boxes", "Write text", "Erase" };

    /* Click positions, used for line/box drawing. */
    MEVENT click[2];
    int nclick = 0;

    /* Text cursor location. */
    struct {
        int insert;
        int writing, startx, x, wsx, y;
    } textcursor = {0};
    
    initscr(); cbreak(); noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);

    mousemask(BUTTON1_CLICKED | BUTTON2_CLICKED | BUTTON3_CLICKED, NULL);

    drawing = cells_alloc(COLS, LINES);
    
    while ((i = getch()) != ERR) {
        if (i == KEY_MOUSE) {
            MEVENT ev;
            getmouse(&ev);
            move(ev.y, ev.x);
            refresh();

            if (mode == text) {
                /* Click in text mode; set start-of-writing. */
                textcursor.writing = 1;
                textcursor.startx = textcursor.x = textcursor.wsx = ev.x;
                textcursor.y = ev.y;
            } else if (ev.bstate == BUTTON3_CLICKED && mode == lines) {
                /* Button 3 click starts a new line. */
                nclick = 1;
                click[0] = ev;
            } else {
                /* Any random click in lines/boxes/erase mode. */
                click[nclick++] = ev;
                if (nclick == 2) {
                    nclick = 0;

                    if (mode == lines) {
                        /* We draw two types of corners:
                         *
                         *  .---X
                         *  |       button 1, button 1
                         *  X
                         *
                         *      X
                         *      |   button 1, button 2
                         *  X---'
                         * 
                         */
/*                        if (click[0].x == click[1].x)
                            vertical_line(drawing, click[0].x, click[0].y, click[1].y);
                        else if (click[0].y == click[1].y)
                            horizontal_line(drawing, click[0].x, click[1].x, click[0].y);
                        else */{
                            if (click[1].bstate & BUTTON1_CLICKED) {
                                if (click[0].x < click[1].x) {
                                    horizontal_line(drawing, click[0].x, click[1].x, click[1].y);
                                    vertical_line(drawing, click[0].x, click[0].y, click[1].y);
                                } else {
                                    horizontal_line(drawing, click[1].x, click[0].x, click[0].y);
                                    vertical_line(drawing, click[1].x, click[1].y, click[0].y);
                                }
                            } else if (click[1].bstate & BUTTON2_CLICKED) {
                                if (click[0].x > click[1].x) {
                                    horizontal_line(drawing, click[0].x, click[1].x, click[1].y);
                                    vertical_line(drawing, click[0].x, click[0].y, click[1].y);
                                } else {
                                    horizontal_line(drawing, click[1].x, click[0].x, click[0].y);
                                    vertical_line(drawing, click[1].x, click[1].y, click[0].y);
                                }
                            } else
                                beep();
                        }
                        display_drawing(drawing, COLS, LINES);

                        /* Allow drawing of `polylines'. */
                        click[0] = click[1];
                        nclick = 1;
                        move(click[0].y, click[0].x);
                        refresh();
                    } else if (mode == boxes) {
                        /* Boxes are drawn between the two corners specified. */
                        vertical_line(drawing, click[0].x, click[0].y, click[1].y);
                        vertical_line(drawing, click[1].x, click[0].y, click[1].y);
                        horizontal_line(drawing, click[0].x, click[1].x, click[0].y);
                        horizontal_line(drawing, click[0].x, click[1].x, click[1].y);
                        display_drawing(drawing, COLS, LINES);
                    } else if (mode == erase) {
                        /* Erase the contents of the box between the two
                         * corners. */
                        int i, i0, i1, j, j0, j1;
                        i0 = min(click[0].x, click[1].x);
                        i1 = max(click[0].x, click[1].x);
                        j0 = min(click[0].y, click[1].y);
                        j1 = max(click[0].y, click[1].y);
                        flash_box(i0, j0, i1, j1);
                        for (j = j0; j <= j1; ++j)
                            for (i = i0; i <= i1; ++i)
                                drawing[j][i].lines = drawing[j][i].c = 0;
                        display_drawing(drawing, COLS, LINES);
                    }
                }
            }
        } else if (mode != text || !textcursor.writing) {
            /* Key pressed in non-writing mode; maybe switch mode. */
            enum mode_t mm;
            mm = mode;
            switch (i) {
                case 'L':
                case 'l':
                    mode = lines;
                    break;

                case 'B':
                case 'b':
                    mode = boxes;
                    break;

                case 'T':
                case 't':
                    mode = text;
                    break;

                case 'E':
                case 'e':
                    mode = erase;
                    break;

                case ' ':
                    nclick = 0;
                    break;
            }
            if (mm != mode) {
                flash_modelabel(mode_name[mode], drawing);
                nclick = 0;
            }
        } else {
            /* Key pressed in writing mode; say something, probably. */
            switch (i) {
                case KEY_LEFT:
                    if (textcursor.x > textcursor.startx)
                        --textcursor.x;
                    else
                        beep();
                    break;

                case KEY_RIGHT:
                    if (textcursor.x < find_end_of_text_block(drawing, textcursor.x, textcursor.y, COLS) - 1)
                        ++textcursor.x;
                    else
                        beep();
                    break;

                case KEY_UP:
                    if (textcursor.y > 0)
                        --textcursor.y;
                    else
                        beep();
                    break;

                case KEY_DOWN:
                    if (textcursor.y < LINES - 1)
                        ++textcursor.y;
                    else
                        beep();
                    break;

                case KEY_HOME:
                    textcursor.x = textcursor.startx;
                    break;

                case KEY_END: {
                    /* Try to go to the end of the current bit of text. */
                    int I;
                    for (I = textcursor.x; I < COLS && drawing[textcursor.y][I].c; ++I);
                    --I;
                    textcursor.x = I;
                    break;
                }

                case KEY_IC:
                    textcursor.insert = !textcursor.insert;
                    flash_modelabel(textcursor.insert ? "Insert" : "Replace", drawing);
                    break;

                case KEY_BACKSPACE:
                    if (textcursor.x > textcursor.startx) {
                        --textcursor.x;
                        if (textcursor.insert) {
                            /* Move characters to left. */
                            int i0, i1;
                            i0 = find_end_of_text_block(drawing, textcursor.x, textcursor.y, COLS);
                            for (i1 = textcursor.x; i1 < i0; ++i1)
                                drawing[textcursor.y][i1].c = drawing[textcursor.y][i1 + 1].c;
                        } else
                            drawing[textcursor.y][textcursor.x].c = ' ';
                    }
                    break;

                case KEY_ENTER:
                    textcursor.writing = 0;
                    break;

                default:
                    if (isprint(i)) {
                        if (textcursor.insert) {
                            /* Move characters to right. */
                            int i0, i1;
                            i0 = find_end_of_text_block(drawing, textcursor.x, textcursor.y, COLS);
                            for (i1 = min(i0 + 1, COLS); i1 > textcursor.x; --i1)
                                drawing[textcursor.y][i1].c = drawing[textcursor.y][i1 - 1].c;
                        }
                        drawing[textcursor.y][textcursor.x].c = i;
                        if (i == ' ')
                            textcursor.wsx = textcursor.x;
                        ++textcursor.x;
                    }
                    break;
            }

            
            display_drawing(drawing, COLS, LINES);
            move(textcursor.y, textcursor.x);
            refresh();
        }
    }
}

