#include "tDemo.h"
#include "resource.h"
#include "tDemoList.h"
#include <gl/glew.h>
#include <gl/wglew.h>

void tDemo::Start()
{
	// initializations common to all demos
	InitDemo(); // creates objects, intializes subsystem (SDL)

	// "game loop"
	// change later, to handle input and display controllers, and timed looping
	SDL_Event event;
	while (!m_done)
	{
		while(SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
				// send stop signal to myself in another thread
				_beginthreadex(NULL, 0, StopDemoThread, dynamic_cast<void*>(m_parentListBox), 0, NULL); // start demo in another thread
            }
            else if (event.type == SDL_KEYDOWN)
            {
                if (event.key.keysym.sym == SDLK_ESCAPE)
                {
					// send stop signal to myself in another thread
                    _beginthreadex(NULL, 0, StopDemoThread, dynamic_cast<void*>(m_parentListBox), 0, NULL); // stop demo in another thread
                }
            }
        }
        SDL_GL_SwapWindow(m_window);
	}

	// now quitting, call destructors
	EndDemo();
}

void tDemo::InitDemo()
{
	// construct necessary objects common to all Demos

	// Initialize subsystem
	InitSubsystem();

	// Initialize GL how you'd like it
	glClearColor(1,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
	
	SDL_GL_SwapWindow(m_window);
	glClear(GL_COLOR_BUFFER_BIT); // call clear again so we can get rid of that flickering -- sets back buffer to clear color
}

void tDemo::InitSubsystem()
{
	// initialize SDL
    SDL_Init(SDL_INIT_VIDEO);

	// try for a 4.3 context
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);

	// create the window
    m_window = SDL_CreateWindow(m_title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          m_width, m_height, SDL_WINDOW_OPENGL);
	
	// Create an OpenGL context associated with the window.
    m_context = SDL_GL_CreateContext(m_window);

	//set up glew, now that a context has been created with sdl createcontext
	glewExperimental = GL_TRUE;
	GLenum err = glewInit(); //now intialize glew
}

void tDemo::EndDemo()
{
	// close out the subsystem
	SDL_GL_DeleteContext(m_context);
	// Close and destroy the window
    SDL_DestroyWindow(m_window);
	SDL_Quit(); // when done
}

unsigned __stdcall tDemo::StopDemoThread(void* list)
{
	tDemoList* pDemoList = static_cast<tDemoList*>(list);
	pDemoList->SendStopSignal(pDemoList->GetCurrentEntry());
	return 0;
}

tDemo::~tDemo()
{
	// destruct all necessary objects
}