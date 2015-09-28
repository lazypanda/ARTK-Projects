/*
	ARTK_Alive
	Copyright (C) 2009  PY

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/*
	���̃v���O������AR��̌����Ă��炢�����č�������̂Ȃ̂�
	ARToolKit�����́A�Ȃ�ׂ��c���Ă����Ăق����Ǝv���܂��B
	MMD�Đ������͍D���Ɏg���Ă�����Č��\�ł��B
                                             PY
*/

#ifdef	_WIN32
#include	<windows.h>
#include	<locale.h>
#endif

#include	<gl/glut.h>
#include	<stdio.h>

#include	"ARTK/ARTK.h"
#include	"GLMetaseq.h"
#include	"JankenHand.h"

#include	"MMD/MMDChar.h"


extern cARTK		g_clARTK;

static cMMDChar		g_clMMDChar;
static MQO_MODEL	g_sNegiModel;
static cJankenHand	g_clJankenHand;

static bool			g_bDispFPS;
static Matrix		g_matPlanarProjection;
static float		g_fThresholdDispCount;

static void init( void );
static void cleanup( void );
static void display( void );
static void resize( int w, int h );
static void visibility( int visible );
static void idle( void );
static float getElapsedFrame( void );
static void keyboard( unsigned char key, int x, int y );
static void keyboardSP( int key, int x, int y );
static void menu( int value );
static void openNewModel( void );
static void calcFps( void );
static void drawString( const char *szStr, int iPosX, int iPosY );


//==========
// main�֐�
//==========
int main( int argc, char *argv[] )
{
	// GLUT�̏�����
	glutInit( &argc, argv );

	// ARToolKit�̏�����
	if( !g_clARTK.initialize() )
	{
		exit( -1 );
	}

	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_STENCIL );
	glutInitWindowSize( 640, 480 );
	glutCreateWindow( "ARTK Alive" );

	// argl�̏�����
	if( !g_clARTK.initArgl() )
	{
		exit( -1 );
	}

	glutDisplayFunc( display );
	glutReshapeFunc( resize );
	glutVisibilityFunc( visibility );
	glutKeyboardFunc( keyboard );
	glutSpecialFunc( keyboardSP );
	atexit( cleanup );

    glutCreateMenu( menu );
		glutAddMenuEntry( "Open Model(PMD)", 1 );
		glutAddMenuEntry( "Let's Play Janken!", 2 );
	glutAttachMenu( GLUT_RIGHT_BUTTON );

	init();

	glutMainLoop();

	return 0;
}

//--------
// ������
//--------
void init( void )
{
	// OpenGL�֘A
	glClearColor( 0.0f, 0.0f, 1.0f, 1.0f );
	glClearStencil( 0 );
	glEnable( GL_DEPTH_TEST );

	glEnable( GL_NORMALIZE );

	glEnable( GL_TEXTURE_2D );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	glEnable( GL_CULL_FACE );
	glCullFace( GL_FRONT );

	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	glEnable( GL_ALPHA_TEST );
	glAlphaFunc( GL_GEQUAL, 0.05f );

	const float	fLightPos[] = { 0.45f, 0.55f , 1.0f, 0.0f };
	const float	fLightDif[] = { 0.9f, 0.9f, 0.9f, 1.0f },
				fLightAmb[] = { 0.7f, 0.7f, 0.7f, 1.0f };

	glLightfv( GL_LIGHT0, GL_POSITION, fLightPos );
	glLightfv( GL_LIGHT0, GL_DIFFUSE, fLightDif );
	glLightfv( GL_LIGHT0, GL_AMBIENT, fLightAmb );
	glLightfv( GL_LIGHT0, GL_SPECULAR, fLightAmb );
	glEnable( GL_LIGHT0 );

	glEnable( GL_LIGHTING );

	// ���ʓ��e�s��̍쐬
	Vector4		vec4Plane = { 0.0f, 1.0f, 0.0f, 0.0f };		// { a, b, c, d } �� ax + by + cz + d = 0 (���e���������ʂ̕�����)
	Vector3		vec4LightPos = { 10.0f, 70.0f, -20.0f };	// ���C�g�̈ʒu

	MatrixPlanarProjection( g_matPlanarProjection, &vec4Plane, &vec4LightPos );

	g_bDispFPS = false;
	g_fThresholdDispCount = 0.0f;

	// MMD�֘A
	g_clMMDChar.initialize();

	// ���^�Z�R�֘A
	mqoInit();
	g_sNegiModel = mqoCreateModel( "data/negi.mqo", 1.0 );

	// ����񂯂�֘A
	g_clJankenHand.initialize();
}

//------------------
// �v���O�����I����
//------------------
void cleanup( void )
{
	mqoDeleteModel( g_sNegiModel );
	mqoCleanup();
}

//----------
// �\���֐�
//----------
void display( void )
{
	glClear( GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	glDisable( GL_CULL_FACE );
	glDisable( GL_ALPHA_TEST );
	glDisable( GL_BLEND );

	g_clARTK.display();

	glEnable( GL_CULL_FACE );
	glEnable( GL_ALPHA_TEST );
	glEnable( GL_BLEND );

	if( g_clARTK.isMarkersFound() )
	{
		double	dMat[16];

		glMatrixMode( GL_PROJECTION );
		g_clARTK.getProjectionMat( dMat );
		glLoadMatrixd( dMat );

		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

		// ���f���̕`��
		if( g_clARTK.isMarkerVisible( ARMK_BASE ) )
		{
			glPushMatrix();

				// �{�̃��f���`��
				g_clARTK.getModelViewMat( dMat, ARMK_BASE );
				glLoadMatrixd( dMat );

				glCullFace( GL_FRONT );

				glScalef( 1.0f, 1.0f, -1.0f );	// ����n �� �E��n
				g_clMMDChar.render();

				// �X�e���V���o�b�t�@�ɉe�̌`��`��
				glDisable( GL_CULL_FACE );
				glDisable( GL_TEXTURE_2D );
				glDisable( GL_LIGHTING );

				glEnable( GL_STENCIL_TEST );
				glStencilFunc( GL_ALWAYS, 1, ~0 );
				glStencilOp( GL_REPLACE, GL_KEEP, GL_REPLACE );

				glColorMask( 0, 0, 0, 0 );
				glDepthMask( 0 );

					glMultMatrixf( (const float *)g_matPlanarProjection );
					g_clMMDChar.renderForShadow();		// �e�p�̕`��

				glColorMask( 1, 1, 1, 1 );

				// �X�e���V���o�b�t�@�̉e�̌`��h��Ԃ�
				float	fWndW = glutGet( GLUT_WINDOW_WIDTH ),
						fWndH = glutGet( GLUT_WINDOW_HEIGHT );

				glStencilFunc( GL_EQUAL, 1, ~0);
				glStencilOp( GL_KEEP, GL_KEEP ,GL_KEEP );

				glDisable( GL_DEPTH_TEST );

				glMatrixMode( GL_PROJECTION );
				glPushMatrix();
				glLoadIdentity();
				gluOrtho2D( 0.0f, fWndW, 0.0f, fWndH );

				glMatrixMode( GL_MODELVIEW );
				glPushMatrix();
				glLoadIdentity();

					glColor4f( 0.2f, 0.2f, 0.2f, 0.3f );

					glBegin( GL_TRIANGLE_FAN );
						glVertex2f(  0.0f, fWndH );
						glVertex2f( fWndW, fWndH );
						glVertex2f( fWndW,  0.0f );
						glVertex2f(  0.0f,  0.0f );
					glEnd();

				glMatrixMode( GL_PROJECTION );
				glPopMatrix();
				glMatrixMode( GL_MODELVIEW );
				glPopMatrix();

				glDepthMask( 1 );
				glEnable( GL_LIGHTING );
				glEnable( GL_DEPTH_TEST );
				glDisable( GL_STENCIL_TEST );

			glPopMatrix();
		}

		// �J�[�\���̕`��
		if( g_clARTK.isMarkerVisible( ARMK_CURSOR ) )
		{
			glPushMatrix();

				g_clARTK.getModelViewMat( dMat, ARMK_CURSOR );
				glLoadMatrixd( dMat );

				glDisable( GL_CULL_FACE );
				glDisable( GL_TEXTURE_2D );

				mqoCallModel( g_sNegiModel );

			glPopMatrix();
		}

		// �O�[�̕`��
		if( g_clARTK.isMarkerVisible( ARMK_GOO ) )
		{
			glPushMatrix();

				g_clARTK.getModelViewMat( dMat, ARMK_GOO );
				glLoadMatrixd( dMat );

				g_clJankenHand.render( JANKEN_GOO );

			glPopMatrix();
		}

		// �`���L�̕`��
		if( g_clARTK.isMarkerVisible( ARMK_CHOKI ) )
		{
			glPushMatrix();

				g_clARTK.getModelViewMat( dMat, ARMK_CHOKI );
				glLoadMatrixd( dMat );

				g_clJankenHand.render( JANKEN_CHOKI );

			glPopMatrix();
		}

		// �p�[�̕`��
		if( g_clARTK.isMarkerVisible( ARMK_PAR ) )
		{
			glPushMatrix();

				g_clARTK.getModelViewMat( dMat, ARMK_PAR );
				glLoadMatrixd( dMat );

				g_clJankenHand.render( JANKEN_PAR );

			glPopMatrix();
		}

	}

	// 臒l�\��
	if( g_fThresholdDispCount > 0.0f )
	{
		char	szStr[16];
		sprintf( szStr, "Threshold:%3d", g_clARTK.getThreshold() );
		drawString( szStr, 8, 16 );
	}

	// �t���[�����[�g�\��
	calcFps();

	glutSwapBuffers();
}

//--------------------------------
// �E�C���h�E�����T�C�Y���ꂽ�Ƃ�
//--------------------------------
void resize( int w, int h )
{
	int		iResizeW = (float)h * 4.0f / 3.0f;
	int		iResizeH = (float)w * 3.0f / 4.0f;

	if( iResizeW < w )
		iResizeH = h;
	else
		iResizeW = w;

	glutReshapeWindow( iResizeW, iResizeH );

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
	glViewport( 0, 0, iResizeW, iResizeH );
	
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
}

//----------------------------------------
// �E�C���h�E�̃r�W�r���e�B���ω������Ƃ�
//----------------------------------------
void visibility( int visible )
{
	if( visible == GLUT_VISIBLE )	glutIdleFunc( idle );
	else							glutIdleFunc( NULL );
}

//------------
// �A�C�h����
//------------
void idle( void )
{
	if( g_clARTK.update() )
	{
		if( g_clARTK.isMarkerVisible( ARMK_BASE ) )
		{
			Vector3		vecCamPos;
			g_clARTK.getCameraPos( &vecCamPos );

			unsigned int	uiJankenFlag = 0;

			if( g_clARTK.isMarkerVisible( ARMK_GOO )   )	uiJankenFlag |= JANKEN_GOO;
			if( g_clARTK.isMarkerVisible( ARMK_CHOKI ) )	uiJankenFlag |= JANKEN_CHOKI;
			if( g_clARTK.isMarkerVisible( ARMK_PAR )   )	uiJankenFlag |= JANKEN_PAR;

			float		fElapsedFrame = getElapsedFrame();

			if( g_clARTK.isMarkerVisible( ARMK_CURSOR ) )
			{
				Vector3		vecCurPos;
				g_clARTK.getMarkerPos( &vecCurPos, ARMK_BASE, ARMK_CURSOR );
				g_clMMDChar.update( fElapsedFrame, &vecCamPos, &vecCurPos, uiJankenFlag );
			}
			else
			{
				g_clMMDChar.update( fElapsedFrame, &vecCamPos, NULL, uiJankenFlag );
			}

			if( g_fThresholdDispCount > 0.0f )
				g_fThresholdDispCount -= fElapsedFrame;
		}

		glutPostRedisplay();
	}
}

//-----------------------------------------------
// �o�߃t���[������Ԃ�(1.0 = 1/30s = 33.333ms)
//-----------------------------------------------
float getElapsedFrame( void )
{
	static int	s_iPrevTime = 0;
	int			iTime = glutGet( GLUT_ELAPSED_TIME );
	float		fDiffTime;

	if( s_iPrevTime == 0 )	s_iPrevTime = iTime;
	fDiffTime = (float)(iTime - s_iPrevTime) * (1.0f / 30.0f);

	s_iPrevTime = iTime;

	return fDiffTime;
}

//----------------------------
// �L�[�{�[�h���͂��������Ƃ�
//----------------------------
void keyboard( unsigned char key, int x, int y )
{
	switch( key )
	{
		case 'o' :	// ���f���Ǎ���
		case 'O' :
			openNewModel();
			break;

		case 'f' :	// �t���[�����[�g�\��On/Off
		case 'F' :
			g_bDispFPS = !g_bDispFPS;
			break;

		case 'j' :	// ����񂯂�X�^�[�g
		case 'J' :
			g_clMMDChar.startJankenMode();
			break;

		case '+' :	// ���f���g��
			g_clARTK.addViewScaleFactor( -0.01 );
			break;

		case '-' :	// ���f���k��
			g_clARTK.addViewScaleFactor(  0.01 );
			break;
	}
}

void keyboardSP( int key, int x, int y )
{
	switch( key )
	{
		case GLUT_KEY_UP :		// 臒l���グ��
			g_clARTK.addThreshold(  5 );
			g_fThresholdDispCount = 3.0f * 30.0f;
			break;

		case GLUT_KEY_DOWN :	// 臒l��������
			g_clARTK.addThreshold( -5 );
			g_fThresholdDispCount = 3.0f * 30.0f;
			break;
	}
}

//--------------------------------
// ���j���[�̍��ڂ��I�����ꂽ�Ƃ�
//--------------------------------
void menu( int value )
{
	switch( value )
	{
		case 1 :	// ���f���Ǎ���
			openNewModel();
			break;

		case 2 :	// ����񂯂�X�^�[�g
			g_clMMDChar.startJankenMode();
			break;
	}
}

//----------------------
// ���f���t�@�C�����J��
//----------------------
void openNewModel( void )
{
#ifdef	_WIN32
	OPENFILENAME	ofn;
	wchar_t			wszFileName[256],
					wszFile[64];
	char			szFileName[256];

	ZeroMemory( &wszFileName, 256 );
	ZeroMemory( &wszFile,      64 );
	ZeroMemory( &ofn, sizeof(OPENFILENAME) );
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = L"PMD File(*.pmd)\0*.pmd\0\0";
	ofn.lpstrFile = wszFileName;
	ofn.lpstrFileTitle = wszFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = sizeof(wszFile);
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = L"pmd";
	ofn.lpstrTitle = L"Open";

	if( GetOpenFileName( &ofn ) )
	{
		wchar_t		wszDirName[256];

		wcscpy( wszDirName, wszFileName );
		for( int i = wcslen( wszDirName ) ; i > 0 ; i-- )
		{
			if( wszDirName[i - 1] == '\\' )
			{
				wszDirName[i] = '\0';
				SetCurrentDirectory( wszDirName );
				break;
			}
		}

		setlocale( LC_ALL, "Japanese_Japan.932" );
	    wcstombs( szFileName, wszFileName, 255 );

		g_clMMDChar.loadModel( szFileName );
	}
#endif
}

//-------------------
// FPS�̌v��(���x��)
//-------------------
void calcFps( void )
{
	static int		iT1 = 0,
					iT2 = 0,
					iCnt = 0;
	static float	fFps = 0.0f;

	iT1 = glutGet( GLUT_ELAPSED_TIME );
	if( iT1 - iT2 >= 1000 )
	{
		fFps = (1000.0f * iCnt) / (float)(iT1 - iT2);
		iT2 = iT1;
		iCnt = 0;
	}
	iCnt++; 

	if( g_bDispFPS )
	{
		char	szStr[16];
		sprintf( szStr, "%6.2f", fFps );
		drawString( szStr, 8, 8 );
	}
}

//------------
// ������`��
//------------
void drawString( const char *szStr, int iPosX, int iPosY )
{
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_LIGHTING );
	glDisable( GL_TEXTURE_2D );

	glMatrixMode( GL_PROJECTION );
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D( 0.0f, (float)glutGet( GLUT_WINDOW_WIDTH ), 0.0f, (float)glutGet( GLUT_WINDOW_HEIGHT ) );

	glMatrixMode( GL_MODELVIEW );
	glPushMatrix();
	glLoadIdentity();

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	glRasterPos2i( iPosX, glutGet( GLUT_WINDOW_HEIGHT ) - (iPosY + 15) );

	while( *szStr )
	{
		glutBitmapCharacter( GLUT_BITMAP_9_BY_15, *szStr );
		szStr++;
 	}

	glMatrixMode( GL_PROJECTION );
	glPopMatrix();
	glMatrixMode( GL_MODELVIEW );
	glPopMatrix();

	glEnable( GL_DEPTH_TEST );
	glEnable( GL_LIGHTING );
}
