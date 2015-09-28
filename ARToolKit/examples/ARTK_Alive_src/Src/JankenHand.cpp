//****************
// じゃんけんの手
//****************

#include	<gl/glut.h>
#include	"MMD/TextureList.h"
#include	"JankenHand.h"

extern cTextureList		g_clsTextureList;


//================
// コンストラクタ
//================
cJankenHand::cJankenHand( void ) : m_uiGooTexID( 0xFFFFFFFF ), m_uiChokiTexID( 0xFFFFFFFF ), m_uiParTexID( 0xFFFFFFFF )
{
}

//==============
// デストラクタ
//==============
cJankenHand::~cJankenHand( void )
{
	release();
}

//========
// 初期化
//========
bool cJankenHand::initialize( void )
{
	m_uiGooTexID   = g_clsTextureList.getTexture( "Data/goo.tga" );
	m_uiChokiTexID = g_clsTextureList.getTexture( "Data/choki.tga" );
	m_uiParTexID   = g_clsTextureList.getTexture( "Data/par.tga" );

	return true;
}

//======
// 描画
//======
void cJankenHand::render( unsigned int uiJankenType )
{
	glDisable( GL_CULL_FACE );
	glDisable( GL_LIGHTING );

	glEnable( GL_TEXTURE_2D );

	switch( uiJankenType )
	{
		case JANKEN_GOO :	glBindTexture( GL_TEXTURE_2D, m_uiGooTexID   );	break;
		case JANKEN_CHOKI :	glBindTexture( GL_TEXTURE_2D, m_uiChokiTexID );	break;
		case JANKEN_PAR :	glBindTexture( GL_TEXTURE_2D, m_uiParTexID   );	break;
	}

	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );

	glBegin( GL_TRIANGLE_FAN );
		glTexCoord2f( 1.0f, 0.0f );	glVertex3f( -2.0f, 0.0f, -2.0f );
		glTexCoord2f( 1.0f, 1.0f );	glVertex3f(  2.0f, 0.0f, -2.0f );
		glTexCoord2f( 0.0f, 1.0f );	glVertex3f(  2.0f, 0.0f,  2.0f );
		glTexCoord2f( 0.0f, 0.0f );	glVertex3f( -2.0f, 0.0f,  2.0f );
	glEnd();

	glEnable( GL_LIGHTING );
}

//======
// 解放
//======
void cJankenHand::release( void )
{
	if( m_uiGooTexID != 0xFFFFFFFF )
	{
		g_clsTextureList.releaseTexture( m_uiGooTexID );
		m_uiGooTexID = 0xFFFFFFFF;
	}

	if( m_uiChokiTexID != 0xFFFFFFFF )
	{
		g_clsTextureList.releaseTexture( m_uiChokiTexID );
		m_uiChokiTexID = 0xFFFFFFFF;
	}

	if( m_uiParTexID != 0xFFFFFFFF )
	{
		g_clsTextureList.releaseTexture( m_uiParTexID );
		m_uiParTexID = 0xFFFFFFFF;
	}
}
