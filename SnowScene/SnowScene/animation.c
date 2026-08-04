/******************************************************************************
 *
 * Animation v2.0 (15/07/2026)
 *
 * This template provides a basic FPS-limited render loop for an animated scene.
 *
 ******************************************************************************/

#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>
#include <time.h>


 /******************************************************************************
  * Animation & Timing Setup
  ******************************************************************************/

  // Target frame rate (number of Frames Per Second).
#define TARGET_FPS 60				

// Ideal time each frame should be displayed for (in milliseconds).
const unsigned int FRAME_TIME = 1000 / TARGET_FPS;

// Frame time in fractional seconds.
// Note: This is calculated to accurately reflect the truncated integer value of
// FRAME_TIME, which is used for timing, rather than the more accurate fractional
// value we'd get if we simply calculated "FRAME_TIME_SEC = 1.0f / TARGET_FPS".
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;

// Time we started preparing the current frame (in milliseconds since GLUT was initialized).
unsigned int frameStartTime = 0;

/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/

 // Define all character keys used for input (add any new key definitions here).
 // Note: USE ONLY LOWERCASE CHARACTERS HERE. The keyboard handler provided converts all
 // characters typed by the user to lowercase, so the SHIFT key is ignored.

#define KEY_EXIT			27 // Escape key.

int renderFillEnabled = 1;

/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void idle(void);

/******************************************************************************
 * Animation-Specific Function Prototypes (add your own here)
 ******************************************************************************/

void drawCircle(float centreX, float centreY, float radius);
void drawSolidCircle(float centreX, float centreY, float radius);

void main(int argc, char **argv);
void init(void);
void think(void);

/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/

#define GROUND_POINTS 20
float groundY[GROUND_POINTS];

/******************************************************************************
 * Entry Point (don't put anything except the main function here)
 ******************************************************************************/

void main(int argc, char **argv)
{
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 800);
	glutCreateWindow("Animation");

	// Set up the scene.
	init();

	// Disable key repeat (keyPressed or specialKeyPressed will only be called once when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutIdleFunc(idle);

	// Record when we started rendering the very first frame (which should happen after we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);

	// Enter the main drawing loop (this will never return).
	glutMainLoop();
}

/******************************************************************************
 * GLUT Callbacks (don't add any other functions here)
 ******************************************************************************/

 /*
	 Called when GLUT wants us to (re)draw the current animation frame.

	 Note: This function must not do anything to update the state of our simulated
	 world. Animation (moving or rotating things, responding to keyboard input,
	 etc.) should only be performed within the think() function provided below.
 */
void display(void)
{
	if (!renderFillEnabled)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	else
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR DRAWING CODE
		
		Separate reusable pieces of drawing code into functions, which you can add
		to the "Animation-Specific Functions" section below.
		
		Remember to add prototypes for any new functions to the "Animation-Specific
		Function Prototypes" section near the top of this template.
	*/

	glClear(GL_COLOR_BUFFER_BIT);

	// Sky
	glBegin(GL_QUADS);
	glColor4f(0.1f, 0.1f, 0.4f, 1.0f);   // dark blue at top
	glVertex2f(0, 800);
	glVertex2f(800, 800);
	glColor4f(0.4f, 0.6f, 0.9f, 1.0f);   // lighter blue at bottom
	glVertex2f(800, 0);
	glVertex2f(0, 0);
	glEnd();

	// Ground
	glBegin(GL_POLYGON);
	// bottom two corners - darker
	glColor3f(0.5f, 0.5f, 0.65f);   
	glVertex2f(0, 0);
	glVertex2f(800, 0);

	//bumpy top edge - brighter white
	for (int i = GROUND_POINTS - 1; i >= 0; i--)
	{
		float x = i * (800.0f / (GROUND_POINTS - 1));
		glColor3f(0.95f, 0.95f, 1.0f);
		glVertex2f(x, groundY[i]);
	}
	glEnd();

	// Snowman
	glColor3f(1.0f, 1.0f, 1.0f);
	drawCircle(400, 210, 70);   // bottom (biggest)
	drawCircle(400, 300, 55);   // middle
	drawCircle(400, 370, 40);   // head (smallest)

	// Eyes
	glColor3f(0.0f, 0.0f, 0.0f);
	drawSolidCircle(385, 380, 6);   // left eye
	drawSolidCircle(415, 380, 6);   // right eye

	// Nose (carrot)
	glColor3f(1.0f, 0.5f, 0.0f);   // orange
	glBegin(GL_TRIANGLES);
	glVertex2f(400, 370);   // base top
	glVertex2f(400, 362);   // base bottom
	glVertex2f(425, 360);   // tip pointing right
	glEnd();

	// Stick arms
	glColor3f(0.4f, 0.2f, 0.0f);   // brown
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	// left arm
	glVertex2f(355, 310);
	glVertex2f(310, 330);
	// left arm branch 1 (forks upward, partway along)
	glVertex2f(340, 317);
	glVertex2f(335, 330);
	// left arm branch 2 (forks downward, further along)
	glVertex2f(325, 324);
	glVertex2f(320, 315);
	
	// right arm
	glVertex2f(445, 310);
	glVertex2f(490, 330);
	// right arm branch 1 (forks upward, partway along)
	glVertex2f(460, 317);
	glVertex2f(465, 330);
	// right arm branch 2 (forks downward, further along)
	glVertex2f(470, 322);
	glVertex2f(478, 312);
	glEnd();

	// Scarf
	glColor3f(0.8f, 0.1f, 0.1f);   // red
	// neck band (dips in the middle to follow the neck)
	glBegin(GL_POLYGON);
	glVertex2f(370, 345);   // top left
	glVertex2f(430, 345);   // top right
	glVertex2f(428, 336);   // bottom right
	glVertex2f(400, 330);   // bottom middle (dips lower)
	glVertex2f(372, 336);   // bottom left
	glEnd();

	// dangling end
	glBegin(GL_QUADS);
	glVertex2f(410, 340);
	glVertex2f(422, 340);
	glVertex2f(422, 305);
	glVertex2f(410, 305);
	glEnd();

	// Buttons
	glColor3f(0.1f, 0.1f, 0.1f);   // dark
	drawSolidCircle(400, 300, 5);
	drawSolidCircle(400, 280, 5);
	drawSolidCircle(400, 320, 5);

	// Hat
	glColor3f(0.1f, 0.1f, 0.1f);   // black
	// brim
	glBegin(GL_QUADS);
	glVertex2f(365, 408);
	glVertex2f(435, 408);
	glVertex2f(435, 400);
	glVertex2f(365, 400);
	glEnd();

	// crown
	glBegin(GL_QUADS);
	glVertex2f(378, 408);
	glVertex2f(422, 408);
	glVertex2f(422, 448);
	glVertex2f(378, 448);
	glEnd();

	// hat band
	glColor3f(0.8f, 0.1f, 0.1f);   // red band
	glBegin(GL_QUADS);
	glVertex2f(378, 416);
	glVertex2f(422, 416);
	glVertex2f(422, 408);
	glVertex2f(378, 408);
	glEnd();

	glutSwapBuffers();

}

/*
	Called when the OpenGL window has been resized.
*/
void reshape(int width, int h)
{
}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is pressed.
*/
void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {
		/*
			TEMPLATE: Add any new character key controls here.

			Rather than using literals (e.g. "d" for diagnostics), create a new KEY_
			definition in the "Keyboard Input Handling Setup" section of this file.
		*/
	case 'l':
		renderFillEnabled = !renderFillEnabled;
		break;
	case KEY_EXIT:
		exit(0);
		break;
	}
}

/*
	Called by GLUT when it's not rendering a frame.

	Note: We use this to handle animation and timing. You shouldn't need to modify
	this callback at all. Instead, place your animation logic (e.g. moving or rotating
	things) within the think() method provided with this template.
*/
void idle(void)
{
	// Wait until it's time to render the next frame.

	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) - frameStartTime;
	if (frameTimeElapsed < FRAME_TIME)
	{
		// This frame took less time to render than the ideal FRAME_TIME: we'll suspend this thread for the remaining time,
		// so we're not taking up the CPU until we need to render another frame.
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}

	// Begin processing the next frame.

	frameStartTime = glutGet(GLUT_ELAPSED_TIME); // Record when we started work on the new frame.

	think(); // Update our simulated world before the next call to display().

	glutPostRedisplay(); // Tell OpenGL there's a new frame ready to be drawn.
}

/******************************************************************************
 * Animation-Specific Functions (Add your own functions at the end of this section)
 ******************************************************************************/

void drawCircle(float centreX, float centreY, float radius)
{
	glBegin(GL_TRIANGLE_FAN);
	glColor3f(0.95f, 0.95f, 1.0f);
	glVertex2f(centreX, centreY);   // centre point

	for (int angle = 0; angle <= 360; angle += 10)
	{
		float rad = angle * 3.14159f / 180.0f;
		float x = centreX + radius * cos(rad);
		float y = centreY + radius * sin(rad);
		glColor3f(0.5f, 0.5f, 0.65f);
		glVertex2f(x, y);
	}
	glEnd();
}

void drawSolidCircle(float centreX, float centreY, float radius)
{
	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(centreX, centreY);   // centre point

	for (int angle = 0; angle <= 360; angle += 10)
	{
		float rad = angle * 3.14159f / 180.0f;
		float x = centreX + radius * cos(rad);
		float y = centreY + radius * sin(rad);
		glVertex2f(x, y);
	}
	glEnd();
}

/*
	Initialise OpenGL and set up our scene before we begin the render loop.
*/
void init(void)
{
	srand((unsigned int)time(NULL));

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, 800, 0, 800);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	float height = 225;   // starting height
	for (int i = 0; i < GROUND_POINTS; i++)
	{
		groundY[i] = height;
		height += (rand() % 21) - 10;   // drift up or down by -10 to +10
	}
}

/*
	Advance our animation by FRAME_TIME milliseconds.

	Note: Our template's GLUT idle() callback calls this once before each new
	frame is drawn, EXCEPT the very first frame drawn after our application
	starts. Any setup required before the first frame is drawn should be placed
	in init().
*/
void think(void)
{
	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR ANIMATION/SIMULATION CODE

		In this function, we update all the variables that control the animated
		parts of our simulated world. For example: if you have a moving box, this is
		where you update its coordinates to make it move. If you have something that
		spins around, here's where you update its angle.

		NOTHING CAN BE DRAWN IN HERE: you can only update the variables that control
		how everything will be drawn later in display().

		How much do we move or rotate things? Because we use a fixed frame rate, we
		assume there's always FRAME_TIME milliseconds between drawing each frame. So,
		every time think() is called, we need to work out how far things should have
		moved, rotated, or otherwise changed in that period of time.

		Movement example:
		* Let's assume a distance of 1.0 GL units is 1 metre.
		* Let's assume we want something to move 2 metres per second on the x axis
		* Each frame, we'd need to update its position like this:
			x += 2 * (FRAME_TIME / 1000.0f)
		* Note that we have to convert FRAME_TIME to seconds. We can skip this by
		  using a constant defined earlier in this template:
			x += 2 * FRAME_TIME_SEC;

		Rotation example:
		* Let's assume we want something to do one complete 360-degree rotation every
		  second (i.e. 60 Revolutions Per Minute, or RPM).
		* Each frame, we'd need to update our object's angle like this (we'll use the
		  FRAME_TIME_SEC constant as per the example above):
			a += 360 * FRAME_TIME_SEC;

		This works for any type of "per second" change: just multiply the amount you'd
		want to move in a full second by FRAME_TIME_SEC, and add or subtract that
		from whatever variable you're updating.

		You can use this same approach to animate other things like color, opacity,
		brightness of lights, etc.
	*/
}

/**************************************2026*S2****************************************/