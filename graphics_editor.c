// Edited through VS Code
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define ROWS 30
#define COLS 60
#define MAX_OBJECTS 100

/* ───────────────────────────── Canvas ───────────────────────────── */

char canvas[ROWS][COLS];

void init_canvas(void) {
    for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
            canvas[r][c] = '_';
}

void display_canvas(void) {
    printf("\n  ");
    for (int c = 0; c < COLS; c++) printf("%d", c % 10);
    printf("\n");
    for (int r = 0; r < ROWS; r++) {
        printf("%2d|", r);
        for (int c = 0; c < COLS; c++) printf("%c", canvas[r][c]);
        printf("|\n");
    }
    printf("\n");
}

static void set_pixel(int r, int c, char ch) {
    if (r >= 0 && r < ROWS && c >= 0 && c < COLS)
        canvas[r][c] = ch;
}

/* ───────────────────────────── Object store ─────────────────────── */

typedef enum { OBJ_CIRCLE, OBJ_RECTANGLE, OBJ_LINE, OBJ_TRIANGLE } ObjType;

typedef struct {
    int active;
    ObjType type;
    /* circle  : cx, cy, radius                        */
    /* rect    : x1, y1 (top-left), x2, y2 (bottom-right) */
    /* line    : x1, y1, x2, y2                        */
    /* triangle: three vertices stored as x1y1 x2y2 x3y3 */
    int p[6];
} Object;

Object objects[MAX_OBJECTS];
int obj_count = 0;

/* ───────────────────── Drawing primitives ────────────────────────── */

/* Bresenham line */
static void draw_line_raw(int r1, int c1, int r2, int c2, char ch) {
    int dr = abs(r2 - r1), dc = abs(c2 - c1);
    int sr = (r1 < r2) ? 1 : -1;
    int sc = (c1 < c2) ? 1 : -1;
    int err = dr - dc;
    while (1) {
        set_pixel(r1, c1, ch);
        if (r1 == r2 && c1 == c2) break;
        int e2 = 2 * err;
        if (e2 > -dc) { err -= dc; r1 += sr; }
        if (e2 <  dr) { err += dr; c1 += sc; }
    }
}

/* Midpoint circle */
static void draw_circle_raw(int cr, int cc, int radius, char ch) {
    int x = 0, y = radius, d = 1 - radius;
    while (x <= y) {
        set_pixel(cr + x, cc + y, ch); set_pixel(cr - x, cc + y, ch);
        set_pixel(cr + x, cc - y, ch); set_pixel(cr - x, cc - y, ch);
        set_pixel(cr + y, cc + x, ch); set_pixel(cr - y, cc + x, ch);
        set_pixel(cr + y, cc - x, ch); set_pixel(cr - y, cc - x, ch);
        if (d < 0) d += 2 * x + 3;
        else { d += 2 * (x - y) + 5; y--; }
        x++;
    }
}

static void draw_rect_raw(int r1, int c1, int r2, int c2, char ch) {
    draw_line_raw(r1, c1, r1, c2, ch);
    draw_line_raw(r2, c1, r2, c2, ch);
    draw_line_raw(r1, c1, r2, c1, ch);
    draw_line_raw(r1, c2, r2, c2, ch);
}

static void draw_triangle_raw(int r1,int c1,int r2,int c2,int r3,int c3,char ch) {
    draw_line_raw(r1,c1,r2,c2,ch);
    draw_line_raw(r2,c2,r3,c3,ch);
    draw_line_raw(r3,c3,r1,c1,ch);
}

/* ───────────────────── Redraw entire canvas ─────────────────────── */

void redraw_canvas(void) {
    init_canvas();
    for (int i = 0; i < obj_count; i++) {
        if (!objects[i].active) continue;
        int *p = objects[i].p;
        switch (objects[i].type) {
            case OBJ_CIRCLE:    draw_circle_raw(p[0],p[1],p[2],'*');           break;
            case OBJ_RECTANGLE: draw_rect_raw(p[0],p[1],p[2],p[3],'*');        break;
            case OBJ_LINE:      draw_line_raw(p[0],p[1],p[2],p[3],'*');        break;
            case OBJ_TRIANGLE:  draw_triangle_raw(p[0],p[1],p[2],p[3],p[4],p[5],'*'); break;
        }
    }
}

/* ────────────────────── Add helpers ────────────────────────────── */

static int new_slot(void) {
    if (obj_count >= MAX_OBJECTS) { printf("Object limit reached!\n"); return -1; }
    return obj_count++;
}

void add_circle(void) {
    int cr, cc, r;
    printf("Circle centre row: "); scanf("%d",&cr);
    printf("Circle centre col: "); scanf("%d",&cc);
    printf("Radius           : "); scanf("%d",&r);
    int i = new_slot(); if (i<0) return;
    objects[i] = (Object){1, OBJ_CIRCLE, {cr,cc,r}};
    redraw_canvas();
    printf("Circle added (id %d).\n", i);
}

void add_rectangle(void) {
    int r1,c1,r2,c2;
    printf("Top-left  row col : "); scanf("%d %d",&r1,&c1);
    printf("Bot-right row col : "); scanf("%d %d",&r2,&c2);
    int i = new_slot(); if (i<0) return;
    objects[i] = (Object){1, OBJ_RECTANGLE, {r1,c1,r2,c2}};
    redraw_canvas();
    printf("Rectangle added (id %d).\n", i);
}

void add_line(void) {
    int r1,c1,r2,c2;
    printf("Start row col : "); scanf("%d %d",&r1,&c1);
    printf("End   row col : "); scanf("%d %d",&r2,&c2);
    int i = new_slot(); if (i<0) return;
    objects[i] = (Object){1, OBJ_LINE, {r1,c1,r2,c2}};
    redraw_canvas();
    printf("Line added (id %d).\n", i);
}

void add_triangle(void) {
    int r1,c1,r2,c2,r3,c3;
    printf("Vertex 1 row col : "); scanf("%d %d",&r1,&c1);
    printf("Vertex 2 row col : "); scanf("%d %d",&r2,&c2);
    printf("Vertex 3 row col : "); scanf("%d %d",&r3,&c3);
    int i = new_slot(); if (i<0) return;
    objects[i] = (Object){1, OBJ_TRIANGLE, {r1,c1,r2,c2,r3,c3}};
    redraw_canvas();
    printf("Triangle added (id %d).\n", i);
}

/* ────────────────────── List / Delete / Modify ─────────────────── */

static const char *type_name(ObjType t) {
    switch(t) {
        case OBJ_CIRCLE:    return "Circle";
        case OBJ_RECTANGLE: return "Rectangle";
        case OBJ_LINE:      return "Line";
        case OBJ_TRIANGLE:  return "Triangle";
    }
    return "Unknown";
}

void list_objects(void) {
    int found = 0;
    printf("\n%-4s %-10s  Parameters\n", "ID", "Type");
    printf("-------------------------------------------\n");
    for (int i = 0; i < obj_count; i++) {
        if (!objects[i].active) continue;
        found = 1;
        int *p = objects[i].p;
        printf("%-4d %-10s  ", i, type_name(objects[i].type));
        switch (objects[i].type) {
            case OBJ_CIRCLE:
                printf("centre(%d,%d) r=%d", p[0],p[1],p[2]); break;
            case OBJ_RECTANGLE:
                printf("(%d,%d)-(%d,%d)", p[0],p[1],p[2],p[3]); break;
            case OBJ_LINE:
                printf("(%d,%d)->(%d,%d)", p[0],p[1],p[2],p[3]); break;
            case OBJ_TRIANGLE:
                printf("(%d,%d) (%d,%d) (%d,%d)",
                       p[0],p[1],p[2],p[3],p[4],p[5]); break;
        }
        printf("\n");
    }
    if (!found) printf("  (no objects)\n");
    printf("\n");
}

void delete_object(void) {
    list_objects();
    int id;
    printf("Enter object ID to delete (-1 to cancel): "); scanf("%d",&id);
    if (id < 0 || id >= obj_count || !objects[id].active) {
        printf("Invalid ID.\n"); return;
    }
    objects[id].active = 0;
    redraw_canvas();
    printf("Object %d deleted.\n", id);
}

void modify_object(void) {
    list_objects();
    int id;
    printf("Enter object ID to modify (-1 to cancel): "); scanf("%d",&id);
    if (id < 0 || id >= obj_count || !objects[id].active) {
        printf("Invalid ID.\n"); return;
    }
    objects[id].active = 0; /* temporarily remove */
    printf("Re-enter parameters for %s:\n", type_name(objects[id].type));
    ObjType t = objects[id].type;
    objects[id] = (Object){0}; /* zero out */
    objects[id].type = t;

    int *p = objects[id].p;
    switch (t) {
        case OBJ_CIRCLE:
            printf("Centre row col: "); scanf("%d %d",&p[0],&p[1]);
            printf("Radius        : "); scanf("%d",&p[2]);
            break;
        case OBJ_RECTANGLE:
            printf("Top-left  row col: "); scanf("%d %d",&p[0],&p[1]);
            printf("Bot-right row col: "); scanf("%d %d",&p[2],&p[3]);
            break;
        case OBJ_LINE:
            printf("Start row col: "); scanf("%d %d",&p[0],&p[1]);
            printf("End   row col: "); scanf("%d %d",&p[2],&p[3]);
            break;
        case OBJ_TRIANGLE:
            printf("Vertex 1 row col: "); scanf("%d %d",&p[0],&p[1]);
            printf("Vertex 2 row col: "); scanf("%d %d",&p[2],&p[3]);
            printf("Vertex 3 row col: "); scanf("%d %d",&p[4],&p[5]);
            break;
    }
    objects[id].active = 1;
    redraw_canvas();
    printf("Object %d updated.\n", id);
}

/* ────────────────────────────── Main menu ──────────────────────── */

void print_menu(void) {
    printf("╔══════════════════════════════╗\n");
    printf("║    2D Graphics Editor (C)    ║\n");
    printf("╠══════════════════════════════╣\n");
    printf("║  1. Display canvas           ║\n");
    printf("║  2. Add circle               ║\n");
    printf("║  3. Add rectangle            ║\n");
    printf("║  4. Add line                 ║\n");
    printf("║  5. Add triangle             ║\n");
    printf("║  6. List objects             ║\n");
    printf("║  7. Delete object            ║\n");
    printf("║  8. Modify object            ║\n");
    printf("║  9. Clear canvas             ║\n");
    printf("║  0. Exit                     ║\n");
    printf("╚══════════════════════════════╝\n");
    printf("Choice: ");
}

int main(void) {
    init_canvas();
    int choice;
    do {
        print_menu();
        if (scanf("%d",&choice) != 1) { while(getchar()!='\n'); continue; }
        switch (choice) {
            case 1: display_canvas();  break;
            case 2: add_circle();      break;
            case 3: add_rectangle();   break;
            case 4: add_line();        break;
            case 5: add_triangle();    break;
            case 6: list_objects();    break;
            case 7: delete_object();   break;
            case 8: modify_object();   break;
            case 9:
                init_canvas();
                obj_count = 0;
                printf("Canvas cleared.\n");
                break;
            case 0: printf("Goodbye!\n"); break;
            default: printf("Invalid choice.\n");
        }
    } while (choice != 0);
    return 0;
}
