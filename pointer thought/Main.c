#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>

#define WIDTH 750
#define HEIGHT 750

int last_frame_time = 0;

int init();
void setup();
void input();
void update();
void render();
void destroy();


void bringForward(struct display * display);

int game_is_running;
SDL_Window* window;
SDL_Renderer* renderer;

float focal_length = 4;

struct coordinate {
    int x;
    int y;
};

struct points_2 {
    int p1;
    int p2;
};

struct coordinate3D {
    float x;
    float y;
    float z;
};

struct edge {
    struct points_2 points;
};

struct display {
    int meshAmount;
    struct mesh*** meshPointer;
    struct mesh*** meshStart;
    struct mesh** meshes;
};

struct mesh {
    int vertexAmount;
    int edgeAmount;
    struct edge** edgeStart;
    struct edge** edgePointer;
    struct coordinate3D* coords;
    struct coordinate* coords2D;
    struct edge* edges;

};

struct display* display;

// cube
struct coordinate3D points[8] = {
      {-0.25, 0, 1} ,
      {-0.25, -0.5, 1},
      {0.25, 0, 1},
      {0.25, -0.5, 1},
      {-0.25, 0, 2} ,
      {-0.25, -0.5, 2},
      {0.25, 0, 2},
      {0.25, -0.5, 2}
};

// pyramid
struct coordinate3D points2[5] = {
    {-0.5, -0.5, 0},
    {0.5, -0.5, 0},
    {-0.5, -0.5, 1},
    {0.5, -0.5, 1},
    {0, 0.5, 0.5}
};

// pyramid
struct edge edges2[8] = {
    {0, 1},
    {0, 2},
    {3, 2},
    {3, 1},
    {4, 0},
    {4, 1},
    {4, 2},
    {4, 3}
};

// cube
struct edge edges[12] = {
    {0, 1},
    {0, 2},
    {3, 1},
    {3, 2},
    {4, 5},
    {4, 6},
    {7, 5},
    {7, 6},
    {0, 4},
    {1, 5},
    {2, 6},
    {3, 7},
};

struct mesh* createMesh(struct coordinate3D * points, int pointCount, struct edge * edges, int edgeCount) {
    struct mesh* mesh = malloc(sizeof(struct mesh));
    mesh->edgeAmount = edgeCount;
    mesh->edges = edges;
    mesh->vertexAmount = pointCount;
    mesh->coords = points;
    mesh->coords2D = malloc(sizeof(struct coordinate*));
    mesh->edgePointer = &mesh->edges;
    mesh->edgeStart = mesh->edgePointer;
    return mesh;
}

struct display* createDisplay() {
    struct display* display = malloc(sizeof(struct display));
    display->meshAmount = 0;
    display->meshPointer = &display->meshes;
    display->meshStart = display->meshPointer;
    return display;
}

void addMesh(struct display* display, struct mesh* mesh) {
    display->meshPointer = display->meshStart;
    display->meshPointer += display->meshAmount;
    *display->meshPointer = mesh;
    display->meshPointer = display->meshStart;
    display->meshAmount++;
}

void addMeshes(struct display* display, struct mesh* meshes[], int meshCount) {
    for (int i = 0; i < meshCount; i++) {
        struct mesh* mesh = meshes[i];
        addMesh(display, mesh);
    }
}

void bringForward(struct display* display) {

    for (int j = 0; j < display->meshAmount; j++) {
        display->meshPointer = display->meshStart;
        display->meshPointer += j;
        struct mesh* mesh = *display->meshPointer;
        
        struct coordinate* coord_ptr2D = mesh->coords2D;
        
        for (int i = 0; i < mesh->vertexAmount; i++) {
            float x_forward = ((mesh->coords[i].x * focal_length) / (mesh->coords[i].z + focal_length));
            float y_forward = (((mesh->coords[i].y * focal_length) / (mesh->coords[i].z + focal_length)))*-1;
            struct coordinate forwarded = { x_forward * WIDTH / 2 + WIDTH / 2, y_forward * HEIGHT / 2 + HEIGHT / 2};
            *coord_ptr2D = forwarded;
            coord_ptr2D++;
        }
    }
}

int main() {
    struct mesh* mesh = createMesh(&points, 8, &edges, 12);
    struct mesh* mesh2 = createMesh(&points2, 5, &edges2, 8);
    struct mesh* meshes[2] = { mesh, mesh2 };

    display = createDisplay();

    addMeshes(display, meshes, 2);
    
    game_is_running = init();

    setup();

    while (game_is_running) {
        int start_time = SDL_GetTicks();
        input();
        update();
        render();
    }

    destroy();

    return 0;
}

void input() {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
        case SDL_QUIT:
            game_is_running = 0;
            break;
        case SDL_KEYDOWN:
            switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    game_is_running = 0;
                    break;

                case SDLK_UP:
                    focal_length += 0.1;
                    break;
                case SDLK_DOWN:
                    focal_length -= 0.1;
                    break;
            }
    }
}

void setup() {

}

void update() {
    float deltaTime = (SDL_GetTicks() - last_frame_time) / 1000.0;
    last_frame_time = SDL_GetTicks();

    int meshIndex = 0;
    struct mesh* mesh = *(display->meshStart + meshIndex);


    for (int i = 0; i < mesh->vertexAmount; i++) {

        mesh->coords[i].x -= 0.1 * deltaTime;
    }

}

void renderMeshes(struct display * display) {
    bringForward(display);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    
    // render points
    for (int j = 0; j < display->meshAmount; j++) {
        display->meshPointer = display->meshStart;
        display->meshPointer += j;
        struct mesh* mesh = *display->meshPointer;

        struct coordinate* coord_ptr = mesh->coords2D;
        for (int i = 0; i < mesh->vertexAmount; i++) {
            int x = coord_ptr->x;
            int y = coord_ptr->y;
            SDL_RenderDrawPoint(renderer, x, y);
            coord_ptr++;
        }
    }
    

    
    // render edges
    for (int j = 0; j < display->meshAmount; j++) {
        display->meshPointer = display->meshStart;
        display->meshPointer += j;
        struct mesh* mesh = *display->meshPointer;

        struct coordinate* coord_ptr = mesh->coords2D;
        for (int i = 0; i < mesh->edgeAmount; i++) {
            struct edge* edge = &mesh->edges[i];

            coord_ptr += edge->points.p1;
            struct coordinate p1 = *coord_ptr;
            coord_ptr = mesh->coords2D;

            coord_ptr += edge->points.p2;
            struct coordinate p2 = *coord_ptr;
            coord_ptr = mesh->coords2D;

            SDL_RenderDrawLine(renderer, p1.x, p1.y, p2.x, p2.y);
        }
    }
    
    
    
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    renderMeshes(display);

    SDL_RenderPresent(renderer);

}

void destroy() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fprintf(stderr, "Error: SDL Video System failed.");
        return 0;
    }


    window = SDL_CreateWindow("3d renderer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, NULL);
    if (!window) {
        fprintf(stderr, "Error: Could not create SDL Window.");
        return 0;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fprintf(stderr, "Error: Could not create SDL Renderer.");
        return 0;
    }
    return 1;

}