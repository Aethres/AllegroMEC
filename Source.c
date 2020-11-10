#include <stdio.h>
#include <math.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <ctype.h>
#pragma warning(disable : 4996)

typedef struct
{
    double x, y;
}coordinates;

typedef struct {

    coordinates coordinates;
    double radius;
}circle;

coordinates points[100];

void getPoints() {

    FILE* file;
    char string[100];
    
    int i = 0;
    int chooser = 0;

    if ((file = fopen("Coordinates.txt", "r")) == NULL) {
        printf("\nCant open file\n");
        return;
    }

    fgets(&string, sizeof(string), file);
  
    char* ptr = &string;
    while (*ptr) {
        if (isdigit(*ptr) || *ptr == '-') {
            int val = strtol(ptr, &ptr, 10);
            if (chooser == 0) {
                points[i].x = val;
                chooser = 1;
            }
            else {
                points[i].y = val;
                chooser = 0;
            }
            i++;
        }
        else {
            ptr++;
        }
    }

    fclose(file);
}

double distance(coordinates pointA, coordinates pointB) {

    return sqrt(pow(pointA.x - pointB.x, 2) + pow(pointA.y - pointB.y, 2));

}

int is_valid_circle(circle circle, coordinates points[], int length) {

    for (int i = 0; i < length; i++) {

        if (distance(circle.coordinates, points[i]) > circle.radius)
            return 0;
    }

    return 1;

}


circle getMEC() {

    int pointsLength = sizeof(points) / sizeof(points[0]);
    int firstCircle = 1;


    circle tmpcircle = { 0, 0, 0 };
    circle mec = { 0, 0, 0 };

    for (int i = 0; i < pointsLength; i++) {
        for (int j = 1; j < pointsLength; j++) {

            tmpcircle.coordinates.x = (points[i].x + points[j].x) / 2.0;
            tmpcircle.coordinates.y = (points[i].y + points[j].y) / 2.0;
            tmpcircle.radius = distance(tmpcircle.coordinates, points[i]);

            if (is_valid_circle(tmpcircle, points, pointsLength) && (tmpcircle.radius <= mec.radius || firstCircle == 1)) {
                mec = tmpcircle;
                firstCircle = 0;
            }
        }
    }



    for (int i = 0; i < pointsLength; i++) {
        for (int j = 1; j < pointsLength; j++) {
            for (int k = 0; k < pointsLength; k++) {

                tmpcircle.coordinates.x = (points[i].x + points[j].x + points[k].x) / 3.0;
                tmpcircle.coordinates.y = (points[i].y + points[j].y + points[k].x) / 3.0;
                tmpcircle.radius = distance(tmpcircle.coordinates, points[i]);

                if (is_valid_circle(tmpcircle, points, pointsLength) && (tmpcircle.radius <= mec.radius || firstCircle == 1)) {
                    mec = tmpcircle;
                    firstCircle = 0;
                }
            }


        }
    }
    printf("%f %f %f", mec.coordinates.x, mec.coordinates.y, mec.radius);
    return mec;

}

void must_init(bool test, const char* description){
    if (test) return;

    printf("couldn't initialize %s\n", description);
    exit(1);
}

int main()
{

    getPoints();
    getMEC();

    must_init(al_init(), "allegro");
    must_init(al_install_keyboard(), "keyboard");
    must_init(al_init_primitives_addon(), "primitives");

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    must_init(timer, "timer");

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    must_init(queue, "queue");

    al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
    al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
    al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);

    ALLEGRO_DISPLAY* disp = al_create_display(800, 800);
    must_init(disp, "display");

    ALLEGRO_FONT* font = al_create_builtin_font();
    must_init(font, "font");

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(disp));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;

    ALLEGRO_VERTEX v[] = {
    {.x = 210, .y = 320, .z = 0, .color = al_map_rgb_f(1, 0, 0) },
    {.x = 330, .y = 320, .z = 0, .color = al_map_rgb_f(0, 1, 0) },
    {.x = 210, .y = 420, .z = 0, .color = al_map_rgb_f(0, 0, 1) },
    {.x = 330, .y = 420, .z = 0, .color = al_map_rgb_f(1, 1, 0) },
    };

    al_start_timer(timer);
    while(1){

        al_wait_for_event(queue, &event);

        switch(event.type){
        case ALLEGRO_EVENT_TIMER:
            // game logic goes here.
            redraw = true;
            break;

        case ALLEGRO_EVENT_KEY_DOWN:
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
            break;
        }

        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue)){

            al_clear_to_color(al_map_rgb(0, 0, 0));
            al_draw_text(font, al_map_rgb(255, 255, 255), 0, 0, 0, "Hello world!");
            al_draw_filled_triangle(35, 350, 85, 375, 35, 400, al_map_rgb_f(0, 1, 0));
            al_draw_filled_rectangle(240, 260, 340, 340, al_map_rgba_f(0, 0, 0.5, 0.5));
            al_draw_circle(450, 370, 30, al_map_rgb_f(1, 0, 1), 2);
            al_draw_line(440, 110, 460, 210, al_map_rgb_f(1, 0, 0), 1);
            al_draw_line(500, 220, 570, 200, al_map_rgb_f(1, 1, 0), 1);
            al_draw_prim(v, NULL, NULL, 0, 4, ALLEGRO_PRIM_TRIANGLE_STRIP);
            al_flip_display();

            redraw = false;
        }
    }

    al_destroy_font(font);
    al_destroy_display(disp);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);


    return 0;
}