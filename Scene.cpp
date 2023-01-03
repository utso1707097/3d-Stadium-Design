#include <GL/freeglut.h>

#include <cmath>
#include <numbers>

#include "Scene.h"

// Color struct
typedef struct {
	float r, g, b, a;
} Color;

// Material struct
typedef struct {
	Color ambient;
	Color diffuse;
	Color specular;
	float shininess;
} Material;


static const int W_WIDTH = 1280;
static const int W_HEIGHT = 720;

static GLfloat Txval = 0.0f, Tyval = 0.0f, Tzval = 0.0f;
static GLfloat alpha = 0.0f, theta = 0.0f;
static GLfloat axis_x = 0.0f, axis_y = 0.0f, axis_z = 0.0f;
static bool bRotate = false, uRotate = false;

// Fan controls
static GLfloat theta_circle = 0.0f, theta_fan_rotate = 0.0f;
static bool fan_rotate = false;

// Light controls
static bool amb_light = true, dif_light = true, spec_light = true;
static bool light0 = true, light1 = true, light2 = false;

// Camera Controls
static GLfloat eyeX = -0, eyeY = -0, eyeZ = 50;
static GLfloat lookX = -0, lookY = -0, lookZ = 0;
static GLfloat upX = 0.0f, upY = 1.0f, upZ = 0.0f;

static GLfloat yaw_angle = -90.0f, pitch_angle = 0.0f, roll_angle = 0.0f;

// Textures
extern constexpr unsigned int texture_count = 8;
extern unsigned int textures[texture_count];


static constexpr GLfloat cube[8][3] =
{
	{-1.0,	1.0,  1.0},
	{-1.0,	1.0, -1.0},
	{ 1.0,	1.0,  1.0},
	{ 1.0,	1.0, -1.0},
	{ 1.0, -1.0,  1.0},
	{ 1.0, -1.0, -1.0},
	{-1.0, -1.0,  1.0},
	{-1.0, -1.0, -1.0}
};

static constexpr GLuint indices[2][8] =
{
	{0, 1, 2, 3, 4, 5, 6, 7},
	{3, 5, 1, 7, 0, 6, 2, 4}
};

static GLfloat quad_vertices[8][3] =
{
	{-1.0, -1.0, -1.0},
	{ 1.0, -1.0, -1.0},
	{-1.0, -1.0,  1.0},
	{ 1.0, -1.0,  1.0},

	{-1.0,  1.0, -1.0},
	{ 1.0,  1.0, -1.0},
	{-1.0,  1.0,  1.0},
	{ 1.0,  1.0,  1.0}
};

static GLuint quad_indices[6][4] =
{
	{0,2,3,1},
	{0,2,6,4},
	{2,3,7,6},
	{1,3,7,5},
	{1,5,4,0},
	{6,7,5,4}
};

static void get_normal(
	GLfloat x1, GLfloat y1, GLfloat z1,
	GLfloat x2, GLfloat y2, GLfloat z2,
	GLfloat x3, GLfloat y3, GLfloat z3)
{
	GLfloat Ux, Uy, Uz, Vx, Vy, Vz, Nx, Ny, Nz;

	Ux = x2 - x1;
	Uy = y2 - y1;
	Uz = z2 - z1;

	Vx = x3 - x1;
	Vy = y3 - y1;
	Vz = z3 - z1;

	Nx = Uy * Vz - Uz * Vy;
	Ny = Uz * Vx - Ux * Vz;
	Nz = Ux * Vy - Uy * Vx;

	glNormal3f(Nx, Ny, Nz);
}

static void draw_sphere(
	const Material& material, 
	GLdouble radius, GLuint slices, GLuint stacks
)
{
	auto [ar, ag, ab, aa] = material.ambient;
	glMaterialfv(GL_FRONT, GL_AMBIENT, &ar);
	auto [dr, dg, db, da] = material.diffuse;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, &dr);
	auto [sr, sg, sb, sa] = material.specular;
	glMaterialfv(GL_FRONT, GL_SPECULAR, &sr);
	glMaterialfv(GL_FRONT, GL_SHININESS, &material.shininess);

	glPushMatrix();
	glutSolidSphere(radius, slices, stacks);
	glPopMatrix();
}

// Function to draw a torus
static void draw_torus(
	const Material& material,
	GLdouble innerRadius, GLdouble outerRadius, GLint nsides, GLint rings
)
{
	auto [ar, ag, ab, aa] = material.ambient;
	glMaterialfv(GL_FRONT, GL_AMBIENT, &ar);
	auto [dr, dg, db, da] = material.diffuse;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, &dr);
	auto [sr, sg, sb, sa] = material.specular;
	glMaterialfv(GL_FRONT, GL_SPECULAR, &sr);
	glMaterialfv(GL_FRONT, GL_SHININESS, &material.shininess);

	glPushMatrix();
	glRotated(90, 1, 0, 0);
	glutSolidTorus(innerRadius, outerRadius, nsides, rings);
	glPopMatrix();
}

// Function to draw a cube
static void draw_cube(
	const Material& material,
	GLdouble size
)
{
	auto [ar, ag, ab, aa] = material.ambient;
	glMaterialfv(GL_FRONT, GL_AMBIENT, &ar);
	auto [dr, dg, db, da] = material.diffuse;
	glMaterialfv(GL_FRONT, GL_DIFFUSE, &dr);
	auto [sr, sg, sb, sa] = material.specular;
	glMaterialfv(GL_FRONT, GL_SPECULAR, &sr);
	glMaterialfv(GL_FRONT, GL_SHININESS, &material.shininess);

	glPushMatrix();
	glScaled(size, size, size);
	glBegin(GL_QUADS);
	for (GLint i = 0; i < 6; i++)
	{
		get_normal(
			quad_vertices[quad_indices[i][0]][0], quad_vertices[quad_indices[i][0]][1], quad_vertices[quad_indices[i][0]][2],
			quad_vertices[quad_indices[i][1]][0], quad_vertices[quad_indices[i][1]][1], quad_vertices[quad_indices[i][1]][2],
			quad_vertices[quad_indices[i][2]][0], quad_vertices[quad_indices[i][2]][1], quad_vertices[quad_indices[i][2]][2]
		);

		glVertex3fv(&quad_vertices[quad_indices[i][0]][0]); glTexCoord2f(0, 0);
		glVertex3fv(&quad_vertices[quad_indices[i][1]][0]); glTexCoord2f(0, 1);
		glVertex3fv(&quad_vertices[quad_indices[i][2]][0]); glTexCoord2f(1, 1);
		glVertex3fv(&quad_vertices[quad_indices[i][3]][0]); glTexCoord2f(1, 0);

	}
	glEnd();
	glPopMatrix();
}

static void draw_textured_cube()
{
	GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 60 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glBegin(GL_QUADS);
	for (GLint i = 0; i < 6; i++)
	{
		get_normal(
			quad_vertices[quad_indices[i][0]][0], quad_vertices[quad_indices[i][0]][1], quad_vertices[quad_indices[i][0]][2],
			quad_vertices[quad_indices[i][1]][0], quad_vertices[quad_indices[i][1]][1], quad_vertices[quad_indices[i][1]][2],
			quad_vertices[quad_indices[i][2]][0], quad_vertices[quad_indices[i][2]][1], quad_vertices[quad_indices[i][2]][2]
		);

		glVertex3fv(&quad_vertices[quad_indices[i][0]][0]); glTexCoord2f(0, 0);
		glVertex3fv(&quad_vertices[quad_indices[i][1]][0]); glTexCoord2f(0, 1);
		glVertex3fv(&quad_vertices[quad_indices[i][2]][0]); glTexCoord2f(1, 1);
		glVertex3fv(&quad_vertices[quad_indices[i][3]][0]); glTexCoord2f(1, 0);

	}
	glEnd();
}

static void draw_cube(GLfloat r, GLfloat g, GLfloat b)
{
	GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat mat_ambient[] = { r, g, b, 1.0 };
	GLfloat mat_diffuse[] = { r, g, b, 1.0 };
	GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat mat_shininess[] = { 60 };

	glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

	glBegin(GL_QUAD_STRIP);
	for (GLint i = 0; i < 2; i++)
	{
		glVertex3fv(&cube[indices[i][0]][0]);
		glVertex3fv(&cube[indices[i][1]][0]);
		glVertex3fv(&cube[indices[i][2]][0]);
		glVertex3fv(&cube[indices[i][3]][0]);
		glVertex3fv(&cube[indices[i][4]][0]);
		glVertex3fv(&cube[indices[i][5]][0]);
		glVertex3fv(&cube[indices[i][6]][0]);
		glVertex3fv(&cube[indices[i][7]][0]);
	}

	glEnd();
}

void draw_stumps()
{
	Material material = {
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{0.1f, 0.1f, 0.1f},
		0.15f
	};
	
	// Middle stumps
	glPushMatrix();
	glScalef(0.05f, 1.0f, 0.05f);
	glTranslatef(0.0f, 2.5f, 0.0f);
	draw_cube(material, 2.5);
	glPopMatrix();
	
	// Left stumps
	glPushMatrix();
	glScalef(0.05f, 1.0f, 0.05f);
	glTranslatef(-15.0f, 2.5f, 0.0f);
	draw_cube(material, 2.5);
	glPopMatrix();

	// Right stumps
	glPushMatrix();
	glScalef(0.05f, 1.0f, 0.05f);
	glTranslatef(15.0f, 2.5f, 0.0f);
	draw_cube(material, 2.5);
	glPopMatrix();

	// Right Bail
	glPushMatrix();
	glScalef(0.075f, 0.05f, 0.05f);
	glTranslatef(5.0f, 100.0f, 0.0f);
	draw_cube(material, 5.0);
	glPopMatrix();
	
	// Left Bail
	glPushMatrix();
	glScalef(0.075f, 0.05f, 0.05f);
	glTranslatef(-5.0f, 100.0f, 0.0f);
	draw_cube(material, 5.0);
	glPopMatrix();
}

void draw_flood_light()
{
	// flood light base
	Material material = {
		{0.5f, 0.5f, 0.5f},
		{0.5f, 0.5f, 0.5f},
		{0.1f, 0.1f, 0.1f},
		0.15f
	};
	glPushMatrix();
	glScalef(0.05f, 2.0f, 0.05f);
	glTranslatef(0.0f, -40.0f, 0.0f);
	draw_cube(material, 10.0f);
	glPopMatrix();

	// flood light
	material = {
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{0.0f, 0.0f, 0.0f},
		1.0f
	};
	glPushMatrix();
	glScalef(1.0f, 1.0f, 1.0f);
	glTranslatef(0.0f, -60.0f, 0.0f);
	draw_sphere(material, 5.0f, 20, 20);
	glPopMatrix();
}

void draw_scene()
{
	// Draw a green solid sphere
	glPushMatrix();
	glScalef(1.0f, 0.005f, 1.0f);
	glTranslatef(0.0f, 0.0f, 0.0f);
	Material material = { 
		{0.0f, 1.0f, 0.0f},
		{0.0f, 1.0f, 0.0f},
		{0.1f, 0.1f, 0.1f},
		0.05f
	};
	draw_sphere(material, 50, 50, 50);
	glPopMatrix();

	// Draw a white solid torus (ground gallery)
	glPushMatrix();
	glScalef(1.0f, 0.01f, 1.0f);
	glTranslatef(0.0f, 1.0f, 0.0f);
	material = {
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{0.1f, 0.1f, 0.1f},
		0.15f
	};
	draw_torus(material, 0.5, 50 + 0.5, 50, 50);
	glPopMatrix();

	// Draw another white solid torus (first gallery)
	glPushMatrix();
	glScalef(1.0f, 0.01f, 1.0f);
	glTranslatef(0.0f, 25.0f, 0.0f);
	material = {
		{0.8f, 0.8f, 0.8f},
		{0.8f, 0.8f, 0.8f},
		{0.1f, 0.1f, 0.1f},
		0.15f
	};
	draw_torus(material, 0.5, 50 + 1.0, 50, 50);
	glPopMatrix();

	// Draw another white solid torus (second gallery)
	glPushMatrix();
	glScalef(1.0f, 0.01f, 1.0f);
	glTranslatef(0.0f, 50.0f, 0.0f);
	material = {
		{0.6f, 0.6f, 0.6f},
		{0.6f, 0.6f, 0.6f},
		{0.1f, 0.1f, 0.1f},
		0.15f
	};
	draw_torus(material, 0.5, 50 + 1.5, 50, 50);
	glPopMatrix();
	
	// Draw another white solid torus
	glPushMatrix();
	glScalef(1.0f, 0.01f, 1.0f);
	glTranslatef(0.0f, 75.0f, 0.0f);
	material = {
		{0.4f, 0.4f, 0.4f},
		{0.4f, 0.4f, 0.4f},
		{0.1f, 0.1f, 0.1f},
		0.15f
	};
	draw_torus(material, 0.5, 50 + 2.0, 50, 50);
	glPopMatrix();

	// Draw a gray solid cube (pitch)
	glPushMatrix();
	glScalef(1.0f, 0.01f, 2.0f);
	glTranslatef(0.0f, 50.0f, 0.0f);
	material = {
		{0.5f, 0.5f, 0.5f},
		{0.5f, 0.5f, 0.5f},
		{0.1f, 0.1f, 0.1f},
		0.15f
	};
	draw_cube(material, 5.0);
	glPopMatrix();	

	// Pitch Lines
	glPushMatrix();
	glScalef(1.0f, 0.01f, 0.01f);
	glTranslatef(0.0f, 55.0f, 850.0f);
	material = {
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{0.1f, 0.1f, 0.1f},
		0.15f
	};
	draw_cube(material, 5.0);
	glPopMatrix();
	
	glPushMatrix();
	glScalef(1.0f, 0.01f, 0.01f);
	glTranslatef(0.0f, 55.0f, -850.0f);
	draw_cube(material, 5.0);
	glPopMatrix();

	// Draw stumps
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 10.0f);
	draw_stumps();
	glPopMatrix();

	// Draw stumps
	glPushMatrix();
	glTranslatef(0.0f, 0.0f, -10.0f);
	draw_stumps();
	glPopMatrix();

	// Draw Top Left Flood Light
	glPushMatrix();
	glTranslatef(-30.0f, 100.0f, -40.0f);
	draw_flood_light();
	glPopMatrix();

	// Draw Top Right Flood Light
	glPushMatrix();
	glTranslatef(30.0f, 100.0f, -40.0f);
	draw_flood_light();
	glPopMatrix();
	
	// Draw Bottom Left Flood Light
	glPushMatrix();
	glTranslatef(-30.0f, 100.0f, 40.0f);
	draw_flood_light();
	glPopMatrix();

	// Draw Bottom Right Flood Light
	glPushMatrix();
	glTranslatef(30.0f, 100.0f, 40.0f);
	draw_flood_light();
	glPopMatrix();
}

static void light(
	GLfloat x, GLfloat y, GLfloat z,
	GLenum light,
	bool toggle,
	bool spot)
{
	GLfloat no_light[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_ambient[] = { 0.25, 0.25, 0.25, 1.0 };
	GLfloat light_diffuse[] = { 0.75, 0.75, 0.75, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_position[] = { x, y, z, 1.0 };

	glEnable(light);

	if (toggle) {

		if (amb_light) glLightfv(light, GL_AMBIENT, light_ambient);
		else glLightfv(light, GL_AMBIENT, no_light);

		if (dif_light) glLightfv(light, GL_DIFFUSE, light_diffuse);
		else glLightfv(light, GL_DIFFUSE, no_light);

		if (spec_light) glLightfv(light, GL_SPECULAR, light_specular);
		else glLightfv(light, GL_SPECULAR, no_light);

		glLightfv(light, GL_POSITION, light_position);
	}
	else {
		glLightfv(light, GL_AMBIENT, no_light);
		glLightfv(light, GL_DIFFUSE, no_light);
		glLightfv(light, GL_SPECULAR, no_light);
		glLightfv(light, GL_POSITION, no_light);
	}

	if (spot) {
		GLfloat spot_direction[] = { 0.0, -1.0, 0.0 };
		glLightfv(light, GL_SPOT_DIRECTION, spot_direction);
		glLightf(light, GL_SPOT_CUTOFF, 100.0);
	}
}

void display()
{
	// Set the background color to mid gray
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Set the matrix mode to Projection
	glMatrixMode(GL_PROJECTION);
	// Load the identity matrix
	glLoadIdentity();
	// Load the frustum
	glFrustum(-10, 10, -10, 10, 5, 200);

	// Set the matrix mode to Model View
	glMatrixMode(GL_MODELVIEW);
	// Load the identity matrix
	glLoadIdentity();
	// Set the camera
	gluLookAt(
		eyeX, eyeY, eyeZ,
		lookX, lookY, lookZ,
		upX, upY, upZ
	);

	// Set the rotation
	glRotatef(alpha, axis_x, axis_y, axis_z);
	glRotatef(theta, axis_x, axis_y, axis_z);

	// Set the viewport 
	glViewport(0, 0, W_WIDTH, W_HEIGHT);

	// draw the scene
	draw_scene();

	// Lights
	light(0.0, 0.0, -50.0, GL_LIGHT0, light0, false);
	light(10.0, 0.0, 0.0, GL_LIGHT1, light1, false);
	light(0.0, 50.0, 0.0, GL_LIGHT2, light2, true);

	// Flush the buffer
	glFlush();
	// Swap buffers
	glutSwapBuffers();
}

static void pitch()
{
	// Constrain pitch
	if (pitch_angle > 179.0f) pitch_angle = 179.0f;
	if (pitch_angle < -179.0f) pitch_angle = -179.0f;

	lookX = eyeX * (cos(pitch_angle * static_cast<GLfloat>(std::numbers::pi) / 180.0f));
}

static void yaw()
{
	// Constrain yaw
	if (yaw_angle > 179.0f) yaw_angle = 179.0f;
	if (yaw_angle < 1.0f) yaw_angle = 1.0f;

	lookY = eyeY * (cos(yaw_angle * static_cast<GLfloat>(std::numbers::pi) / 180.0f));
}

void roll() // z axis
{
	// Constrain roll
	if (roll_angle > 269.0f) roll_angle = 269.0f;
	if (roll_angle < -89.0f) roll_angle = -89.0f;

	upX = eyeX * (cos(roll_angle * static_cast<GLfloat>(std::numbers::pi) / 180.0f));
	upY = eyeY * (sin(roll_angle * static_cast<GLfloat>(std::numbers::pi) / 180.0f));
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'Y':	// rotate about y-axis
		bRotate = !bRotate;
		axis_y = -1.0;
		break;
	case 'y':
		bRotate = !bRotate;
		axis_y = 1.0;
		break;
	case 'X':	// rotate about x-axis
		bRotate = !bRotate;
		axis_x = -1.0;
		break;
	case 'x':
		bRotate = !bRotate;
		axis_x = 1.0;
		break;
	case 'Z':	// rotate about z-axis
		bRotate = !bRotate;
		axis_z = -1.0;
		break;
	case 'z':
		bRotate = !bRotate;
		axis_z = 1.0;
		break;

		// case 'a' for Roll left
	case 'a':
		lookX--;
		break;
		// case 'd' for Roll right
	case 'd':
		lookX++;
		break;
		// case 'w' for Pitch up
	case 'w':
		lookY++;
		break;
		// case 's' for Pitch down
	case 's':
		lookY--;
		break;
		// case 'q' for Yaw left
	case 'q':
		lookZ--;
		break;
		// case 'e' for Yaw right
	case 'e':
		lookZ++;
		break;

		// Pitch
	case 'p':
		pitch_angle += 1.0f;
		pitch();
		glutPostRedisplay();
		break;
	case 'P':
		pitch_angle -= 1.0f;
		pitch();
		glutPostRedisplay();
		break;

		// Yaw
	case 'o':
		yaw_angle += 1.0f;
		yaw();
		glutPostRedisplay();
		break;
	case 'O':
		yaw_angle -= 1.0f;
		yaw();
		glutPostRedisplay();
		break;

		// Roll
	case 'r':
		roll_angle += 1.0f;
		roll();
		break;
	case 'R':
		roll_angle -= 1.0f;
		roll();
		break;

		// Fan rotation
	case 'f':
		fan_rotate = !fan_rotate;
		break;

		// Light Controls
	case '0':
		light0 = !light0;
		break;
	case '1':
		light1 = !light1;
		break;
	case '2':
		light2 = !light2;
		break;

	case 27:	// Escape key
		exit(EXIT_SUCCESS);

	default:
		break;
	}
}

void special(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		eyeY--;
		lookY--;
		break;
	case GLUT_KEY_DOWN:
		eyeY++;
		lookY++;
		break;
	case GLUT_KEY_LEFT:
		eyeX++;
		lookX++;
		break;
	case GLUT_KEY_RIGHT:
		eyeX--;
		lookX--;
		break;
	case GLUT_KEY_PAGE_UP:
		eyeZ--;
		lookZ--;
		break;
	case GLUT_KEY_PAGE_DOWN:
		eyeZ++;
		lookZ++;
		break;
	default:
		break;
	}
}

void mouse(int x, int y)
{
	// Normalize offsets

	// Update yaw and pitch

	// Constrain pitch

	// Update camera direction

	// Normalize front
}

void idle()
{
	if (bRotate)
	{
		theta += 0.05f;
		if (theta >= 360.0f) theta = 0.0f;
	}
	if (uRotate)
	{
		alpha += 0.05f;
		if (alpha >= 360.0f) alpha = 0.0f;
	}

	if (fan_rotate)
	{
		theta_fan_rotate += 1.0f;
		if (theta_fan_rotate > 360.0f) theta_fan_rotate = 0.0f;
	}

	glutPostRedisplay();
}