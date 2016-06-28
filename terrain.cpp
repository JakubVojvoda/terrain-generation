/*
 * LoD terrain generation and displaying
 * Svoboda Petr, Vojvoda Jakub
 * 
 * 2014
 *
 */

#include "pgr.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "landscape.h"

#include "heightmap.h"
#include "trianglemesh.h"

#define WIDTH MAP_SIZE
#define HEIGHT MAP_SIZE

using namespace std;

// Camera
// http://zaachi.blog.zive.cz/2008/08/pohyb-kamery-v-opengl/

#define DEG_TO_RAD (3.14159/180.0)
double uhelPohledu = 45;
unsigned oldX = 800/2,oldY = 600/2;
bool clicked = false;
glm::vec3 camera(-20,2,-26);
glm::vec3 oldcamera = camera;
glm::vec3 direction(0.69,0.02,0.719);
float stepSize = 3;
glm::vec3 cam_velocity(0,0,0);

glm::mat4 projectionMatrix = glm::perspective(
    45.0f,
    800.f/600.f,
    0.1f,
    2000.0f
);
glm::mat4 Model      = glm::mat4(1.0f);
glm::mat4 MVP;
glm::mat4 View;

// Data
Landscape land;
bool draw_triangles = false;
float **heightmap;

int width, height;


// Shaders
GLuint VS, FS, Prog;
GLuint positionAttrib, colorUniform, mvpUniform;
GLuint colorAttrib;

void checkErrors() {

    int code = glGetError();

    if (code == GL_NO_ERROR)
        return;

    cout << getGlErrorString(code) << endl;
}

void onInit()
{
    VS = compileShader(GL_VERTEX_SHADER, loadFile("terrain.vert").c_str());
    FS = compileShader(GL_FRAGMENT_SHADER, loadFile("terrain.frag").c_str());
    Prog = linkShader(2, VS, FS);

    positionAttrib = glGetAttribLocation(Prog, "position");
    colorAttrib = glGetAttribLocation(Prog, "color");
    mvpUniform = glGetUniformLocation(Prog, "mvp");

    HeightMap* terrain_map = new HeightMap(WIDTH, HEIGHT);
    heightmap = terrain_map->generateHeightMap(0.05, 20.0);
    terrain_map->interpolate(INTERPOLATION_COS);
    terrain_map->smooth(8, 1);
    terrain_map->addPerlinNoise(0.05, 0.25, 6, INTERPOLATION_COS);

    heightmap = terrain_map->enlargeHeightMap(4);

    land.Init(heightmap);
    delete terrain_map;
}

void onWindowRedraw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glUseProgram(Prog);

    View       = glm::lookAt(
        camera,
        camera + direction,
        glm::vec3(0,1,0)
    );

    MVP        = projectionMatrix * View * Model;
    glUniformMatrix4fv(mvpUniform, 1, GL_FALSE, glm::value_ptr(MVP));

    if ( oldcamera != camera ) 
    {
        land.Reset();
        land.updateDistance();
        land.splitBasedOnDistance();
        oldcamera = camera;
    }

    land.renderBasedOnDistance();

    SDL_GL_SwapBuffers();
}

void onWindowResized(int w, int h)
{
    width = w; height = h;
    glViewport(0, 0, w, h);
}

void onKeyDown(SDLKey key, Uint16 /* mod*/)
{
    switch(key) {
        case SDLK_ESCAPE : quit(); break;

        case SDLK_q: draw_triangles = !draw_triangles;break;
        case SDLK_w :
            cam_velocity = direction * stepSize;
            break;
        case SDLK_s:
            cam_velocity =  -direction * stepSize;
            break;
        case SDLK_d:
            cam_velocity = glm::vec3( -direction.z * stepSize,0,direction.x * stepSize);
            break;
        case SDLK_a:
            cam_velocity = glm::vec3( direction.z * stepSize,0,-direction.x * stepSize);
            break;
            default : break;
    }
}

void onKeyUp(SDLKey key, Uint16 /*mod*/)
{
    switch(key)
    {
        case SDLK_w:
        case SDLK_s:
        case SDLK_a:
        case SDLK_d:
            cam_velocity = glm::vec3(0,0,0);
            break;
        default: break;
    }
}

void onMouseMove(unsigned x, unsigned y, int /*xrel*/, int /*yrel*/, Uint8 /*buttons*/)
{
    if ( clicked )
    {
        if (x > oldX)
        {
            uhelPohledu += (x - oldX);
            if (uhelPohledu > 360) uhelPohledu -= 360;
            direction.x = cos(uhelPohledu*DEG_TO_RAD);
            direction.z = sin(uhelPohledu*DEG_TO_RAD);
            oldX = x;
        }
        if (x < oldX)
        {
            uhelPohledu -= ( oldX - x);
            if ( uhelPohledu < 0 ) uhelPohledu += 360;
            direction.x = cos(uhelPohledu*DEG_TO_RAD);
            direction.z = sin(uhelPohledu*DEG_TO_RAD);
            oldX = x;
        }
        if (y > oldY)
        {
            if (direction.y > -2)
                direction.y -= 0.02;
            oldY = y;
        }
        if (y < oldY)
        {
            if (direction.y < 2)
                direction.y += 0.02;
            oldY = y;
        }
    }
}

void onMouseDown(Uint8 button, unsigned x, unsigned y)
{
    if ( button == SDL_BUTTON_LEFT )
    {
        oldX = x;
        oldY = y;
        clicked = true;
    }
}

void onMouseUp(Uint8 button, unsigned /*x*/, unsigned /*y*/)
{
    if ( button == SDL_BUTTON_LEFT )
    {
        clicked = false;
    }
}

////////////////////////////////////////////////////////////////////////////////

int main(int /*argc*/, char ** /*argv*/)
{
    try {
        if(SDL_Init(SDL_INIT_VIDEO) < 0) throw SDL_Exception();

        atexit(SDL_Quit);
        init(800, 600, 24, 16, 0);
        mainLoop(40);

    } catch(exception & ex) {
        cerr << "ERROR : " << ex.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
