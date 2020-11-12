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
}Coordinates;

typedef struct {

    Coordinates coordinates;
    double radius;
}Circle;

struct Camera
{
    int x, y, speedX, speedY;
    double scale;
};

Coordinates points[100];

int pointsSize = 0;

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
                pointsSize++;
            }
            else {
                points[i].y = val;
                chooser = 0;
                i++;
            }
            
            
        }
        else {
            ptr++;
        }
    }


    fclose(file);
}

double distance(Coordinates pointA, Coordinates pointB) {

    return sqrt(pow(pointA.x - pointB.x, 2) + pow(pointA.y - pointB.y, 2));

}

int is_valid_circle(Circle circle, Coordinates points[], int length) {

    for (int i = 0; i < length; i++) {

        if (distance(circle.coordinates, points[i]) > circle.radius)
            return 0;
    }

    return 1;

}

Circle getMEC() {

    int pointsLength = pointsSize;
    int firstCircle = 1;


    Circle tmpcircle = { 0, 0, 0 };
    Circle mec = { 0, 0, 0 };

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
    int windowX = 600;
    int windowY = 600;
    
    
    getPoints();
    Circle circle = getMEC();

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

    

    windowX = al_get_display_width(disp);
    windowY = al_get_display_height(disp);

    printf("\nwindowX: %d windowY: %d\n", windowX, windowY);

    struct Camera camera = { windowX/2, windowY/2 , 0, 0, 10};

    int xLength = windowX/2;
    int yLength = windowY/2;
    int xNegativeLength = xLength;
    int yNegativeLength = yLength;
    int fifthLine = 0;
    int littleLineSize = 5;

    char str[10];

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
            camera.x += camera.speedX;
            camera.y += camera.speedY;
            xLength -= camera.speedX;
            yLength -= camera.speedY;
            xNegativeLength += camera.speedX;
            yNegativeLength += camera.speedY;

            redraw = true;
            break;

        case ALLEGRO_EVENT_KEY_DOWN:
            if (event.keyboard.keycode == ALLEGRO_KEY_UP)
                camera.speedY+= 10;
            if (event.keyboard.keycode == ALLEGRO_KEY_DOWN)
                camera.speedY-= 10;
            if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)
                camera.speedX+= 10;
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
                camera.speedX-= 10;



            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                done = true;
            
            break;
                
        case ALLEGRO_EVENT_KEY_UP:
            if (event.keyboard.keycode == ALLEGRO_KEY_UP)
                camera.speedY-= 10;
            if (event.keyboard.keycode == ALLEGRO_KEY_DOWN)
                camera.speedY+= 10;
            if (event.keyboard.keycode == ALLEGRO_KEY_LEFT)
                camera.speedX-= 10;
            if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT)
                camera.speedX+= 10;
            break;
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            done = true;
            break;
        }


        if(done)
            break;

        if(redraw && al_is_event_queue_empty(queue)){

            al_clear_to_color(al_map_rgb(200, 200, 200));
            al_draw_text(font, al_map_rgb(0, 0, 0), 0, 0, 0, "You can move the camera with arrow keys");
            
            al_draw_circle(circle.coordinates.x * camera.scale + camera.x, -circle.coordinates.y * camera.scale + camera.y, circle.radius * camera.scale, al_map_rgb_f(1, 0, 1), 2);
            
            for (int i = 0; i < pointsSize; i++)
            {
                al_draw_filled_circle(points[i].x * camera.scale + camera.x, -points[i].y * camera.scale + camera.y, 0.3 * camera.scale, al_map_rgb_f(0, 0, 128));
            }

            al_draw_line(0, 0 + camera.y, windowX, 0 + camera.y, al_map_rgb_f(0, 0, 0), 2);
            al_draw_line(0 + camera.x, 0, 0 + camera.x, windowY, al_map_rgb_f(0, 0, 0), 2);

            fifthLine = 0;
            for (int i = 1; i < xLength / camera.scale; i++)
            {   
                if (fifthLine == 5) {
                    littleLineSize = 10;
                    fifthLine = 0;
                    sprintf(str, "%d", i - 1);
                    al_draw_text(font, al_map_rgb(255, 0, 0), camera.scale * i + camera.x, littleLineSize * 2 + camera.y, ALLEGRO_ALIGN_CENTRE, str);
                }
                else {
                    littleLineSize = 5;
                }
                al_draw_line(camera.scale * i + camera.x, -littleLineSize + camera.y, camera.scale * i + camera.x, littleLineSize + camera.y, al_map_rgb_f(0, 0, 0), 2);
                fifthLine++;
            }

            fifthLine = 0;
            for (int i = 1; i < xNegativeLength / camera.scale; i++)
            {
                if (fifthLine == 5) {
                    littleLineSize = 10;
                    fifthLine = 0;
                    sprintf(str, "%d", -(i - 1));
                    al_draw_text(font, al_map_rgb(255, 0, 0), -(camera.scale * i) + camera.x, littleLineSize * 2 + camera.y, ALLEGRO_ALIGN_CENTRE, str);
                }
                else {
                    littleLineSize = 5;
                }
                al_draw_line(-(camera.scale * i) + camera.x, -littleLineSize + camera.y, -(camera.scale * i) + camera.x, littleLineSize + camera.y, al_map_rgb_f(0, 0, 0), 2);
                fifthLine++;
            }

            fifthLine = 0;
            for (int i = 1; i < yLength / camera.scale; i++)
            {
                if (fifthLine == 5) {
                    littleLineSize = 10;
                    fifthLine = 0;
                    sprintf(str, "%d", -(i - 1));
                    al_draw_text(font, al_map_rgb(255, 0, 0), -littleLineSize * 2 + camera.x, camera.scale * i - 3 + camera.y, ALLEGRO_ALIGN_RIGHT, str);
                }
                else {
                    littleLineSize = 5;
                }
                al_draw_line(-littleLineSize + camera.x, camera.scale * i + camera.y, littleLineSize + camera.x, camera.scale * i + camera.y,  al_map_rgb_f(0, 0, 0), 2);
                fifthLine++;
            }

            fifthLine = 0;
            for (int i = 1; i < yNegativeLength / camera.scale; i++)
            {
                if (fifthLine == 5) {
                    littleLineSize = 10;
                    fifthLine = 0;
                    sprintf(str, "%d", i - 1);
                    al_draw_text(font, al_map_rgb(255, 0, 0), -littleLineSize * 2 + camera.x, -(camera.scale * i) - 3 + camera.y, ALLEGRO_ALIGN_RIGHT, str);
                }
                else {
                    littleLineSize = 5;
                }
                al_draw_line(-littleLineSize + camera.x, -(camera.scale * i) + camera.y, littleLineSize + camera.x, -(camera.scale * i) + camera.y, al_map_rgb_f(0, 0, 0), 2);
                fifthLine++;
            }
            
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