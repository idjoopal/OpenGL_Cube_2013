
#include "Angel.h"
#include <Time.h>

typedef Angel::vec4 point4;
typedef Angel::vec4 color4;

const int NumVertices = 36; //(6 faces)(2 triangles/face)(3 vertices/triangle)

point4 points[NumVertices];

vec3   normals[NumVertices]; //for lighting 

point4 vertices[8] = {
    point4( -0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5,  0.5,  0.5, 1.0 ),
    point4(  0.5, -0.5,  0.5, 1.0 ),
    point4( -0.5, -0.5, -0.5, 1.0 ),
    point4( -0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5,  0.5, -0.5, 1.0 ),
    point4(  0.5, -0.5, -0.5, 1.0 )
};


// Shader transformation matrices
mat4  model_view;
mat4  projection;
mat4  attached[100];

GLuint program;
GLuint ModelView, Projection;


GLfloat  thetaSpinx[100];
GLfloat  thetaSpiny[100];
GLfloat  thetaSpinz[100];

GLfloat  transx[100];
GLfloat  transy[100];
GLfloat  transz[100];

GLfloat rgb_red[100];
GLfloat rgb_green[100];
GLfloat rgb_blue[100];

GLfloat aspect;

// Menu option values

enum {
	Cube_TransX,
	Cube_TransY,
	Cube_TransZ,
	Cube_RotX,
	Cube_RotY,
	Cube_RotZ,
	Camera_RotX,
	Camera_RotY,
	Camera_Closeup,
	Cube_Color,
	FoV_Ctrl,
	Setting_Reset,
    Quit,
	Seperate,
	Newcube,
	Attachcube
};


//----------------------------------------------------------------------------

int Index = 0;
int count = 0;
//bool make[100];
int select=0;
float width[100];
float height[100];
float depth[100];

float cam_speed = 2.0f; // 1 unit per second
float cam_yaw_speed = 20.0f; // 20 degrees per second
float cam_pos[] = {0.0f, 0.0f, 5.0f}; 
float cam_yaw = 0.0f; // y-rotation in degrees
float targ_pos[] = {0.0f, 0.0f, 0.0f}; 
float up[] = {0.0f, 1.0f, 0.0f};
float deltax=0;
float deltay=0;
float k=10;

GLfloat fov = 45;

inline
mat4 Persp( const GLfloat fovy, const GLfloat aspects,
		  const GLfloat zNear, const GLfloat zFar)
{
    GLfloat top   = tan(fovy*DegreesToRadians/2) * zNear;
    GLfloat right = top * aspects;

    mat4 c;
    c[0][0] = zNear/right;
    c[1][1] = zNear/top;
    c[2][2] = -(zFar + zNear)/(zFar - zNear);
    c[2][3] = -2.0*zFar*zNear/(zFar - zNear);
    c[3][2] = -1.0;
    c[3][3] = 0.0;
    return c;
}

void quad( int a, int b, int c, int d )
{
	// Initialize temporary vectors along the quad's edge to
    //   compute its face normal 
    vec4 u = vertices[b] - vertices[a];
    vec4 v = vertices[c] - vertices[b];

    vec3 normal = normalize( cross(u, v) );

	normals[Index] = normal; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; points[Index] = vertices[d]; Index++;
}

void colorcube()
{
    quad( 1, 0, 3, 2 );
    quad( 2, 3, 7, 6 );
    quad( 3, 0, 4, 7 );
    quad( 6, 5, 1, 2 );
    quad( 4, 5, 6, 7 );
    quad( 5, 4, 0, 1 );
}

//----------------------------------------------------------------------------
void
coloring(color4 material_ambient, color4 material_diffuse, color4 material_specular){
	///////////////////////Lighting !!
	// Initialize shader lighting parameters
	point4 light_position( 0.0, 0.0, -1.0, 0.0 );
	color4 light_ambient( 0.2, 0.2, 0.2, 1.0 );
	color4 light_diffuse( 1.0, 1.0, 1.0, 1.0 );
	color4 light_specular( 1.0, 1.0, 1.0, 1.0 );

	
	float  material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv( glGetUniformLocation(program, "AmbientProduct"),
		1, ambient_product );
	glUniform4fv( glGetUniformLocation(program, "DiffuseProduct"),
		1, diffuse_product );
	glUniform4fv( glGetUniformLocation(program, "SpecularProduct"),
		1, specular_product );

	glUniform4fv( glGetUniformLocation(program, "LightPosition"),
		1, light_position );

	glUniform1f( glGetUniformLocation(program, "Shininess"),
		material_shininess );
	//////////////////////////////////////////////////////////////////

	ModelView = glGetUniformLocation( program, "ModelView" );
	Projection = glGetUniformLocation( program, "Projection" );
}
//----------------------------------------------------------------------------

// Please Define proper instance matrix for each of cubes


void cube1()
{
	for(int i=0; i<count+1; i++)
	{
		model_view = Translate(transx[i], transy[i], transz[i]) * RotateX(thetaSpinx[i])
			* RotateY(thetaSpiny[i]) * attached[i];

		coloring(color4(rgb_red[i], rgb_green[i], rgb_blue[i], 1.0),
			color4(rgb_red[i], rgb_green[i], rgb_blue[i], 1.0),
			color4(rgb_red[i], rgb_green[i], rgb_blue[i], 1.0));

		//coloring(color4(0,1,1,1),color4(0,1,1,1),color4(0,1,1,1));

		if(i!=count)
		{
			GLfloat distance = sqrt((transx[i]-transx[count])*(transx[i]-transx[count])
				+ (transy[i]-transy[count])*(transy[i]-transy[count])
				+ (transz[i]-transz[count])*(transz[i]-transz[count]));

			if(distance <= 1.7)
			{
				//coloring(color4(rand()%2, rand()%2, rand()%2, 1.0),
				//	color4(rand()%2, rand()%2, rand()%2, 1.0),
				//color4(rand()%2, rand()%2, rand()%2, 1.0));

				coloring(color4(1, 1, 1, 1.0),
					color4(rgb_red[i], rgb_green[i], rgb_blue[i], 1.0),
					color4(rgb_red[i], rgb_green[i],  rgb_blue[i], 1.0)
					);

				GLfloat wiggle = 0.03*(rand()%3-1);
				model_view = Translate(transx[i]+wiggle, transy[i]+wiggle, transz[i]+wiggle)
					* RotateX(thetaSpinx[i]) * RotateY(thetaSpiny[i]) * attached[i];
				
			}
		}
		

		if(cos(deltay) <= 0){
			up[1] = -1;
		}else{
			up[1] = 1;
		}

		mat4 view_mat = LookAt (vec3 (cam_pos[0], cam_pos[1], cam_pos[2]), 
			vec3 (targ_pos[0], targ_pos[1], targ_pos[2]), 
			vec3 (up[0], up[1], up[2]));

		mat4 instance = Scale(width[i], height[i], depth[i]);

		glUniformMatrix4fv( ModelView, 1, GL_TRUE, view_mat * model_view * instance );
		glDrawArrays( GL_TRIANGLES, 0, NumVertices );
	}
}


//----------------------------------------------------------------------------

void
init( void )
{
    colorcube();
    
    // Create a vertex array object
    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(points) + sizeof(normals),NULL, GL_DYNAMIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(points), points );
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(points), sizeof(normals), normals );

	// Load shaders and use the resulting shader program
	program = InitShader( "vshader80.glsl", "fshader80.glsl" );
	glUseProgram( program );
	GLuint vPosition = glGetAttribLocation( program, "vPosition" );
	glEnableVertexAttribArray( vPosition );
	glVertexAttribPointer( vPosition, 4, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(0) );

	GLuint vNormal = glGetAttribLocation( program, "vNormal" );
	glEnableVertexAttribArray( vNormal );
	glVertexAttribPointer( vNormal, 3, GL_FLOAT, GL_FALSE, 0,BUFFER_OFFSET(sizeof(points)) );

	ModelView = glGetUniformLocation( program, "ModelView" );
	Projection = glGetUniformLocation( program, "Projection" );

	glEnable( GL_DEPTH_TEST );
	glShadeModel(GL_SMOOTH);
	glPolygonMode( GL_FRONT_AND_BACK, GL_TRIANGLES);

	glClearColor( 0.5, 0.5, 0.5, 1.0 ); 
}

//----------------------------------------------------------------------------

void
mouse( int button, int state, int x, int y )
{
    if ( button == GLUT_LEFT_BUTTON && state == GLUT_DOWN ) {
		//count++;
    }

    if ( button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN ) {
		
	}
}
//----------------------------------------------------------------------------

void
reshape( int width, int height )
{
    glViewport( 0, 0, width, height );

	aspect = GLfloat(width)/height;
    projection = Persp( fov, aspect, 0.5, 30.0 );

    glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
}

//----------------------------------------------------------------------------

void display( void )
{
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	cube1();

	glutSwapBuffers();
	glutPostRedisplay();

}
//----------------------------------------------------------------------------
void
submenu( int entryID )
{
	switch(entryID)
	{
	case 1:
		rgb_red[count] = 1.0f;
		rgb_green[count] = 0.0f;
		rgb_blue[count] = 0.0f;
		break;
	case 2:
		rgb_red[count] = 1.0f;
		rgb_green[count] = 0.3f;
		rgb_blue[count] = 0.0f;
		break;
	case 3:
		rgb_red[count] = 1.0f;
		rgb_green[count] = 1.0f;
		rgb_blue[count] = 0.0f;
		break;
	case 4:
		rgb_red[count] = 0.0f;
		rgb_green[count] = 1.0f;
		rgb_blue[count] = 0.0f;
		break;
	case 5:
		rgb_red[count] = 0.0f;
		rgb_green[count] = 0.0f;
		rgb_blue[count] = 1.0f;
		break;
	case 6:
		rgb_red[count] = 0.0f;
		rgb_green[count] = 1.0f;
		rgb_blue[count] = 1.0f;
		break;
	case 7:
		rgb_red[count] = 1.0f;
		rgb_green[count] = 0.0f;
		rgb_blue[count] = 1.0f;
		break;
	case 8:
		rgb_red[count] = 1.0f;
		rgb_green[count] = 1.0f;
		rgb_blue[count] = 1.0f;
		break;
	case 9:
		rgb_red[count] = 0.0f;
		rgb_green[count] = 0.0f;
		rgb_blue[count] = 0.0f;
		break;
	}
}
//----------------------------------------------------------------------------
void
menu( int option )
{
    if ( option == Quit ) {
	exit( EXIT_SUCCESS );
    }
	else if( option == Cube_TransX)
	{
		select = 0;
	}
	else if( option == Cube_TransY)
	{
		select = 1;
	}
	else if( option == Cube_TransZ)
	{
		select = 2;
	}
	else if( option == Cube_RotX)
	{
		select = 3;
	}
	else if( option == Cube_RotY)
	{
		select = 4;
	}
	else if( option == Cube_RotZ)
	{
		select = 5;
	}
	else if( option == Camera_RotX)
	{
		select = 6;
	}
	else if( option == Camera_RotY)
	{
		select = 7;
	}
	else if( option == Camera_Closeup)
	{
		select = 8;
	}
	else if( option == Cube_Color)
	{
		select = 9;
	}
	else if( option == FoV_Ctrl)
	{
		select = 10;
	}
	else if( option == Setting_Reset)
	{
		deltax=0;
		deltay=0;
		k=10;
		fov = 45;

		//aspect = GLfloat(1025)/512;
		//projection = Persp( fov, aspect, 0.5, 30.0 );

		//glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );

		reshape(1025,512);
		cam_pos[0] = k*sin(deltax)*cos(deltay) ;
		cam_pos[1] = k*sin(deltay);
		cam_pos[2] = k*cos(deltax)*cos(deltay);


	}
	else if( option == Seperate)
	{
	}
	else if( option == Newcube)
	{
		count++;
	}
	else if( option == Attachcube)
	{
		for(int i=0; i<count+1; i++)
		{
			if(i!=count)
			{
				GLfloat distance = sqrt((transx[i]-transx[count])*(transx[i]-transx[count])
					+ (transy[i]-transy[count])*(transy[i]-transy[count])
					+ (transz[i]-transz[count])*(transz[i]-transz[count]));

				if(distance <= 1.7)	//큐브의 거리가 1.7일때
				{
					thetaSpinx[count] = thetaSpinx[i];
					thetaSpiny[count] = thetaSpiny[i];
					thetaSpinz[count] = thetaSpinz[i];
					if(abs(transx[i]-transx[count])>=1.0)
					{
						if(abs(transy[i]-transy[count])>=1.0)
						{
							if(abs(transz[i]-transz[count])>=1.0)//XYZ 1이상
							{
								//안붙음
							}
							else	//XY만 1이상
							{
								//더 긴쪽으로 붙음
								if(abs(transx[i]-transx[count])>=abs(transy[i]-transy[count]))
								{
									if(transx[i]-transx[count] >= 0)
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(-width[count],0.0f,0.0f);
									}
									else
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(width[count],0.0f,0.0f);
									}
								}
								else
								{
									if(transy[i]-transy[count] >= 0)
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(0.0f,-height[count],0.0f);
									}
									else
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(0.0f,height[count],0.0f);
									}
								}
							}
						}
						else
						{
							if(abs(transz[i]-transz[count])>=1.0)	//XZ만 1이상
							{
								//더 긴쪽으로 붙음
								if(abs(transx[i]-transx[count])>=abs(transz[i]-transz[count]))
								{
									if(transx[i]-transx[count] >= 0)
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(-width[count],0.0f,0.0f);
									}
									else
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(width[count],0.0f,0.0f);
									}
								}
								else
								{
									if(transz[i]-transz[count] >= 0)
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(0.0f,0.0f,-depth[count]);
									}
									else
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(0.0f,0.0f,depth[count]);
									}
								}
							}
							else	//X만 1이상
							{
								//X로 붙음
								if(transx[i]-transx[count] >= 0)
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(-width[count],0.0f,0.0f);
									}
									else
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(width[count],0.0f,0.0f);
									}
							}
						}
					}
					else
					{
						if(abs(transy[i]-transy[count])>=1.0)
						{
							if(abs(transz[i]-transz[count])>=1.0)//YZ만 1이상
							{
								//더 긴쪽으로 붙음
								if(abs(transy[i]-transy[count])>=abs(transz[i]-transz[count]))
								{
									if(transy[i]-transy[count] >= 0)
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(0.0f,-height[count],0.0f);
									}
									else
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(0.0f,height[count],0.0f);
									}
								}
								else
								{
									if(transz[i]-transz[count] >= 0)
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(0.0f,0.0f,-depth[count]);
									}
									else
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(0.0f,0.0f,depth[count]);
									}
								}
							}
							else	//Y만 1이상
							{
								//Y로 붙음
								if(transy[i]-transy[count] >= 0)
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(0.0f,-height[count],0.0f);
									}
									else
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(0.0f,height[count],0.0f);
									}
							}
						}
						else
						{
							if(abs(transz[i]-transz[count])>=1.0)//Z만 1이상
							{
								//Z로 붙음
								if(transz[i]-transz[count] >= 0)
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(0.0f,0.0f,-depth[count]);
									}
									else
									{
										transx[count] = transx[i];
										transy[count] = transy[i];
										transz[count] = transz[i];
										attached[count] = Translate(0.0f,0.0f,depth[count]);
									}
							}
							else	//1이상 없음
							{
								//안 붙음
							}
						}
					}
				}
			}
		}
	}
    
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
	switch( key ) {
	case 033: // Escape Key
	case 'z': //X,Y,Z축 -이동, 큐브X,Y,Z -회전, 카메라 X,Y -회전, 카메라 축소, 이전큐브칼라, Fov축소
		if(select == 0) //X축 이동
			transx[count] -= 0.1f;
		else if(select == 1) //Y축 -이동
			transy[count] -= 0.1f;
		else if(select == 2)//Z축 -이동
			transz[count] -= 0.1f;
		else if(select == 3)//X축 -회전
			thetaSpinx[count] -=2.0f;
		else if(select == 4)//Y축 -회전
			thetaSpiny[count] -=2.0f;
		else if(select == 5)//Z축 -회전
			thetaSpinz[count] -=2.0f;
		else if(select == 6)//카메라 X축-회전
			deltax -= cam_speed * 0.05f;
		else if(select == 7)//카메라 Y축-회전
			deltay -= cam_speed * 0.05f;
		else if(select == 8)//카메라 축소
			k = k + 0.1f;
		else if(select == 9)//이전 큐브칼라
		{
		}
		else if(select == 10)//FoV축소
		{
			if(fov>20)
			{
				fov--;
			}
		}
		break;
	case 'x': //X,Y,Z축 +이동, 큐브X,Y,Z +회전, 카메라 X,Y +회전, 카메라 확대, 다음큐브칼라, Fov확대
		if(select == 0) //X축 +이동
			transx[count] += 0.1f;
		else if(select == 1) //Y축 +이동
			transy[count] += 0.1f;
		else if(select == 2)//Z축 +이동
			transz[count] += 0.1f;
		else if(select == 3)//X축 +회전
			thetaSpinx[count] +=2.0f;
		else if(select == 4)//Y축 +회전
			thetaSpiny[count] +=2.0f;
		else if(select == 5)//Z축 +회전
			thetaSpinz[count] +=2.0f;
		else if(select == 6)//카메라 X축+회전
			deltax += cam_speed * 0.05f;
		else if(select == 7)//카메라 Y축+회전
			deltay += cam_speed * 0.05f;
		else if(select == 8)//카메라 확대
			k = k - 0.1f;
		else if(select == 9)//다음 큐브칼라
		{
		}
		else if(select == 10)//FoV확대
		{
			if(fov<150)
			{
				fov++;
			}
		}
		break;


	case 'q': case 'Q':
	    exit( EXIT_SUCCESS );
	    break;
	}

	//aspect = GLfloat(1025)/512;
	//projection = Persp( fov, aspect, 0.5, 30.0 );

   // glUniformMatrix4fv( Projection, 1, GL_TRUE, projection );
	reshape(1025,512);
	cam_pos[0] = k*sin(deltax)*cos(deltay) ;
	cam_pos[1] = k*sin(deltay);
	cam_pos[2] = k*cos(deltax)*cos(deltay);
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
	srand((unsigned)time(NULL));
//-----------------------------------------------------------------------  
	for(int i=0; i<100; i++)
	{
		attached[i]=Translate(0.0f, 0.0f, 0.0f);
		//make[i]=false;

		width[i]=1.0;
		height[i]=1.0;
		depth[i]=1.0;

		thetaSpinx[i]=0.0f;
		thetaSpiny[i]=0.0f;
		thetaSpinz[i]=0.0f;

		transx[i]=0.0f;
		transy[i]=0.0f;
		transz[i]=0.0f;

		rgb_red[i] = rand()%2;
		rgb_green[i] = rand()%2;
		rgb_blue[i] = rand()%2;
	}

	cam_pos[0] = k*cos(deltax)*cos(deltay);
	cam_pos[1] = k*sin(deltay);
	cam_pos[2] = k*sin(deltax)*cos(deltay) ;
//-----------------------------------------------------------------------
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH );
    glutInitWindowSize( 1024, 512 );
    glutInitContextVersion( 3, 2 );
    glutInitContextProfile( GLUT_CORE_PROFILE );
    glutCreateWindow( "Local Coordinate" );

	glewExperimental = true;

    glewInit();
    
    init();

	GLint MySubMenuID = glutCreateMenu(submenu);
	glutAddMenuEntry("Red", 1);
	glutAddMenuEntry("Orange", 2);
	glutAddMenuEntry("Yellow", 3);
	glutAddMenuEntry("Green", 4);
	glutAddMenuEntry("Blue", 5);
	glutAddMenuEntry("Cyan", 6);
	glutAddMenuEntry("Purple", 7);
	glutAddMenuEntry("White", 8);
	glutAddMenuEntry("Black", 9);

	GLint MyMainMenuID = glutCreateMenu( menu );
	glutAddMenuEntry( "Create new cube", Newcube );
	glutAddMenuEntry( "Attach new cube", Attachcube );
	glutAddMenuEntry( "---------------",  Seperate );
	glutAddMenuEntry( "cube Move X", Cube_TransX );
	glutAddMenuEntry( "cube Move Y", Cube_TransY );
	glutAddMenuEntry( "cube Move Z", Cube_TransZ );
	glutAddMenuEntry( "---------------",  Seperate );
	glutAddMenuEntry( "cube Rotate X", Cube_RotX );
	glutAddMenuEntry( "cube Rotate Y", Cube_RotY );
	glutAddMenuEntry( "---------------",  Seperate );
	glutAddMenuEntry( "cam Rotate X",  Camera_RotX );
	glutAddMenuEntry( "cam Rotate Y",  Camera_RotY );
	glutAddMenuEntry( "cube Close-up", Camera_Closeup );
	glutAddMenuEntry( "---------------",  Seperate );
	glutAddSubMenu( "change cube color", MySubMenuID );
	glutAddMenuEntry( "---------------",  Seperate );
	glutAddMenuEntry( "fov control", FoV_Ctrl );
	glutAddMenuEntry( "---------------",  Seperate );
	glutAddMenuEntry( "setting reset", Setting_Reset );
	glutAddMenuEntry( "---------------",  Seperate );
	glutAddMenuEntry( "quit", Quit );
	glutAttachMenu( GLUT_MIDDLE_BUTTON );

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );
    glutReshapeFunc( reshape );
    glutMouseFunc( mouse );

	

    glutMainLoop();

    return 0;
}
