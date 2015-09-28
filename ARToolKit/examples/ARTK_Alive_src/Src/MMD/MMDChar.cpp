//******************************************
// PMD���f����VMD���[�V�����œ����L�����N�^
//******************************************

#include	<stdlib.h>
#include	<math.h>
#include	<time.h>
#include	"MMDChar.h"
#include	"../JankenHand.h"
#include	"../ErrorMes.h"

// ���[�V�����t�@�C����
const char	*cMMDChar::m_pszMotionFileNames[MOT_MAXNUM] = {
		"Data/Motion/�ʏ�_���[�v.vmd",

		"Data/Motion/��_�J�n_�I��.vmd",

		"Data/Motion/�K�b�c_�J�n.vmd",
		"Data/Motion/�K�b�c_���[�v.vmd",
		"Data/Motion/�K�b�c_�I��.vmd",

		"Data/Motion/������_�J�n.vmd",
		"Data/Motion/������_���[�v.vmd",
		"Data/Motion/������_�I��.vmd",

		"Data/Motion/�ǋy_�J�n.vmd",
		"Data/Motion/�ǋy_���[�v.vmd",
		"Data/Motion/�ǋy_�I��.vmd",

		"Data/Motion/�Y��_�J�n.vmd",
		"Data/Motion/�Y��_���[�v.vmd",
		"Data/Motion/�Y��_�I��.vmd",

		"Data/Motion/��������.vmd",

		"Data/Motion/�Ȃł��_�J�n.vmd",
		"Data/Motion/�Ȃł��_���[�v.vmd",
		"Data/Motion/�Ȃł��_�I��.vmd",

		"Data/Motion/��������.vmd",

		"Data/Motion/�߂����.vmd",

		"Data/Motion/����񂯂�_�J�n.vmd",
		"Data/Motion/����񂯂�_�O�[.vmd",
		"Data/Motion/����񂯂�_�`���L.vmd",
		"Data/Motion/����񂯂�_�p�[.vmd",
		"Data/Motion/����񂯂�_����.vmd",
		"Data/Motion/����񂯂�_����.vmd"
	};


//================
// �R���X�g���N�^
//================
cMMDChar::cMMDChar( void ) : m_bModelLoaded( false )
{
}

//==============
// �f�X�g���N�^
//==============
cMMDChar::~cMMDChar( void )
{
	release();
}

//========
// ������
//========
bool cMMDChar::initialize( void )
{
	if( !loadMotions() )
	{
		return false;
	}

	srand( (unsigned int)time( NULL ) );

	return true;
}

//--------------------
// ���[�V�����̓Ǎ���
//--------------------
bool cMMDChar::loadMotions( void )
{
	for( int i = 0 ; i < MOT_MAXNUM ; i++ )
	{
		if( !m_clVMDMotArray[i].load( m_pszMotionFileNames[i] ) )
		{
			return false;
		}
	}

	return true;
}

//================
// ���f���̓Ǎ���
//================
bool cMMDChar::loadModel( const char *szFilePath )
{
	if( !m_clPMDModel.load( szFilePath ) )
	{
		return false;
	}

	m_bModelLoaded = true;

	reset();

	return true;
}

//------------------
// ���낢�냊�Z�b�g
//------------------
void cMMDChar::reset( void )
{
	m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_GREETING], true );
	m_clPMDModel.setLookAtFlag( true );

	changeStateTo( ST_GREETING );

	m_pHeadBone = m_clPMDModel.getBoneByName( "��" );
	m_pBustBone = m_clPMDModel.getBoneByName( "�㔼�g" );
	m_pSkirtBone = m_clPMDModel.getBoneByName( "�����g" );

	m_vec3CurVec.x = m_vec3CurVec.y = m_vec3CurVec.z = 0.0f;
	m_vec3PrevCurPos.x = m_vec3PrevCurPos.y = m_vec3PrevCurPos.z = 0.0f;
	m_fCurVecLength = 0.0f;
	m_fStrokeCount = 0.0f;
	m_bStrokeNow = false;
	m_fTouchCount = 0.0f;
}

//====================
// ����񂯂�X�^�[�g
//====================
void cMMDChar::startJankenMode( void )
{
	if( m_iCharState != ST_JANKEN )
	{
		changeStateTo( ST_JANKEN );
	}
}

//==========
// �X�V����
//==========
void cMMDChar::update( float fElapsedFrame, const Vector3 *pvec3CameraPos, const Vector3 *pvec3CursorPos, unsigned int uiJankenFlag )
{
	if( m_bModelLoaded && fElapsedFrame != 0.0f )
	{
		// ��Ԃ��Ƃɏ�����U�蕪����
		switch( m_iCharState )
		{
			case ST_IDLING :		stIdling();					break;
			case ST_GREETING :		stGreeting();				break;
			case ST_GUTS :			stGuts();					break;
			case ST_SIDE :			stSide();					break;
			case ST_INVESTIGATE :	stInvestigate();			break;
			case ST_WORRY :			stWorry();					break;
			case ST_HEAD_BEAT :		stHeadBeat();				break;
			case ST_HEAD_STROKE :	stHeadStroke();				break;
			case ST_BUST_TOUCH :	stBustTouch();				break;
			case ST_SKIRT_LIFT :	stSkirtLift();				break;
			case ST_JANKEN :		stJanken( uiJankenFlag );	break;
			case ST_JANKEN_WIN :	stJankenWin();				break;
			case ST_JANKEN_LOSE :	stJankenLose();				break;
		}

		if( m_fFrameCounter > 0.0f )	m_fFrameCounter -= fElapsedFrame;

		float	fInvElapsedFrame = 1.0f / fElapsedFrame;

		m_bStrokeNow = false;

		// ������蔻��
		if( pvec3CursorPos )
		{
			calcCursorVec( fInvElapsedFrame, pvec3CursorPos );

			Vector3		vec3BonePos;

			// �X�J�[�g�֘A����
			if( m_pSkirtBone )
			{
				m_pSkirtBone->getPos( &vec3BonePos );

				if( vec3BonePos.x - 5.0f < pvec3CursorPos->x && pvec3CursorPos->x < vec3BonePos.x + 5.0f &&
						vec3BonePos.y - 7.0f < pvec3CursorPos->y && pvec3CursorPos->y < vec3BonePos.y + 3.0f &&
							vec3BonePos.z - 5.0f < pvec3CursorPos->z && pvec3CursorPos->z < vec3BonePos.z + 5.0f &&
								0.4f < m_vec3CurVec.y )
				{
					// �X�J�[�g�߂���ꂽ�I
					if( m_iCharState != ST_SKIRT_LIFT )
					{
						changeStateTo( ST_SKIRT_LIFT );
					}
				}
			}

			// ���֘A����
			if( m_pBustBone )
			{
				m_pBustBone->getPos( &vec3BonePos );

				if( vec3BonePos.x - 5.0f < pvec3CursorPos->x && pvec3CursorPos->x < vec3BonePos.x + 5.0f &&
							vec3BonePos.y + 0.0f < pvec3CursorPos->y && pvec3CursorPos->y < vec3BonePos.y + 3.0f &&
								vec3BonePos.z - 5.0f < pvec3CursorPos->z && pvec3CursorPos->z < vec3BonePos.z + 5.0f &&
									0.001f < m_fCurVecLength )
				{
					// ���G��ꂽ�I
					if( m_iCharState != ST_BUST_TOUCH )
					{
						m_fTouchCount += fElapsedFrame;
						if( m_fTouchCount > 0.7f * 30.0f )	changeStateTo( ST_BUST_TOUCH );
					}
				}
				else
				{
					m_fTouchCount = 0.0f;
				}
			}

			// ���֘A����
			if( m_pHeadBone )
			{
				m_pHeadBone->getPos( &vec3BonePos );

				if( vec3BonePos.x - 5.0f < pvec3CursorPos->x && pvec3CursorPos->x < vec3BonePos.x + 5.0f &&
						vec3BonePos.y - 2.0f < pvec3CursorPos->y && pvec3CursorPos->y < vec3BonePos.y + 5.0f &&
							vec3BonePos.z - 5.0f < pvec3CursorPos->z && pvec3CursorPos->z < vec3BonePos.z + 5.0f &&
								m_vec3CurVec.y < -0.6f )
				{
					// ���������ꂽ�I
					changeStateTo( ST_HEAD_BEAT );
					m_fStrokeCount = 0.0f;
				}
				else if( vec3BonePos.x - 5.0f < pvec3CursorPos->x && pvec3CursorPos->x < vec3BonePos.x + 5.0f &&
							vec3BonePos.y + 0.0f < pvec3CursorPos->y && pvec3CursorPos->y < vec3BonePos.y + 4.0f &&
								vec3BonePos.z - 5.0f < pvec3CursorPos->z && pvec3CursorPos->z < vec3BonePos.z + 5.0f &&
									0.01f < m_fCurVecLength )
				{
					// ���Ȃł�ꂽ�I
					if( m_iCharState == ST_HEAD_STROKE )
					{
						// �Ȃł��p����
						m_bStrokeNow = true;
					}
					else if( m_iCharState != ST_HEAD_BEAT )
					{
						m_fStrokeCount += fElapsedFrame;
						if( m_fStrokeCount > 1.0f * 30.0f )	changeStateTo( ST_HEAD_STROKE );
					}
				}
				else
				{
					m_fStrokeCount = 0.0f;
				}
			}

			if( m_iCharState == ST_IDLING )		m_fFrameCounter = 30.0f;
		}
		else
		{
			m_vec3CurVec.x *= fInvElapsedFrame * 0.5f;
			m_vec3CurVec.y *= fInvElapsedFrame * 0.5f;
			m_vec3CurVec.z *= fInvElapsedFrame * 0.5f;
			m_fCurVecLength *= fInvElapsedFrame * 0.5f;
		}

		// �{�[���Ȃǂ̍X�V
		if( fElapsedFrame > 10.0f )	fElapsedFrame = 10.0f;
		m_bMotionEnd = m_clPMDModel.updateMotion( fElapsedFrame );

		if( pvec3CursorPos && (m_iCharState == ST_IDLING || m_iCharState == ST_SIDE || m_iCharState == ST_WORRY) )
			m_clPMDModel.updateNeckBone( pvec3CursorPos, -10.0f, 10.0f, 50.0f );
		else
			m_clPMDModel.updateNeckBone( pvec3CameraPos, -25.0f, 45.0f, 60.0f );

		m_clPMDModel.updateSkinning();
	}
}

//----------------------
// �J�[�\���̓������v�Z
//----------------------
void cMMDChar::calcCursorVec( float fInvElapsedFrame, const Vector3 *pvec3CursorPos )
{
	Vector3		vec3Diff;

	Vector3Sub( &vec3Diff, pvec3CursorPos, &m_vec3PrevCurPos );

	m_vec3CurVec.x = (m_vec3CurVec.x + vec3Diff.x * fInvElapsedFrame) * 0.5f;
	m_vec3CurVec.y = (m_vec3CurVec.y + vec3Diff.y * fInvElapsedFrame) * 0.5f;
	m_vec3CurVec.z = (m_vec3CurVec.z + vec3Diff.z * fInvElapsedFrame) * 0.5f;

	m_fCurVecLength = (m_fCurVecLength + sqrtf( Vector3DotProduct( &vec3Diff, &vec3Diff ) ) * fInvElapsedFrame) * 0.5f;

	m_vec3PrevCurPos = *pvec3CursorPos;
}

//----------
// �ҋ@���
//----------
void cMMDChar::stIdling( void )
{
	switch( m_iCharSubState )
	{
		case 0 :
			m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_IDLING], true, 10.0f );
			m_fFrameCounter = (rand() % 3 + 5) * 30.0f;
			m_iCharSubState++;
			break;

		case 1 :
			if( m_fFrameCounter <= 0.0f )
			{
				switch( rand() % 2 )
				{
					case 0 :	changeStateTo( ST_SIDE );			break;
					case 1 :	changeStateTo( ST_WORRY );			break;
				}
			}
			break;
	}
}

//----
// ��
//----
void cMMDChar::stGreeting( void )
{
	switch( m_iCharSubState )
	{
		case 0 :
			m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_GREETING], false, 0.0f );
			m_iCharSubState++;
			break;

		case 1 :
			if( m_bMotionEnd )
			{
				changeStateTo( ST_IDLING );
			}
			break;
	}
}

//--------
// �K�b�c
//--------
void cMMDChar::stGuts( void )
{
	switch( m_iCharSubState )
	{
		case 0 :
			m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_GUTS_S], false, 5.0f );
			m_iCharSubState++;
			break;

		case 1 :
			if( m_bMotionEnd )
			{
				m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_GUTS_L], true, 5.0f );
				m_fFrameCounter = 30.0f;
				m_iCharSubState++;
			}
			break;

		case 2 :
			if( m_fFrameCounter <= 0.0f )
			{
				m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_GUTS_E], false, 5.0f );
				m_iCharSubState++;
			}
			break;

		case 3 :
			if( m_bMotionEnd )
			{
				changeStateTo( ST_IDLING );
			}
			break;
	}
}

//--------
// ������
//--------
void cMMDChar::stSide( void )
{
	switch( m_iCharSubState )
	{
		case 0 :
			m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_SIDE_S], false, 5.0f );
			m_iCharSubState++;
			break;

		case 1 :
			if( m_bMotionEnd )
			{
				m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_SIDE_L], true, 5.0f );
				m_fFrameCounter = (rand() % 3 + 2) * 30.0f;
				m_iCharSubState++;
			}
			break;

		case 2 :
			if( m_fFrameCounter <= 0.0f )
			{
				m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_SIDE_E], false, 5.0f );
				m_iCharSubState++;
			}
			break;

		case 3 :
			if( m_bMotionEnd )
			{
				changeStateTo( ST_IDLING );
			}
			break;
	}
}

//------
// �ǋy
//------
void cMMDChar::stInvestigate( void )
{
	switch( m_iCharSubState )
	{
		case 0 :
			m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_INVESTIGATE_S], false, 5.0f );
			m_iCharSubState++;
			break;

		case 1 :
			if( m_bMotionEnd )
			{
				m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_INVESTIGATE_L], true, 5.0f );
				m_fFrameCounter = (rand() % 4 + 3) * 30.0f;
				m_iCharSubState++;
			}
			break;

		case 2 :
			if( m_fFrameCounter <= 0.0f )
			{
				m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_INVESTIGATE_E], false, 5.0f );
				m_iCharSubState++;
			}
			break;

		case 3 :
			if( m_bMotionEnd )
			{
				changeStateTo( ST_IDLING );
			}
			break;
	}
}

//------
// �Y��
//------
void cMMDChar::stWorry( void )
{
	switch( m_iCharSubState )
	{
		case 0 :
			m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_WORRY_S], false, 5.0f );
			m_iCharSubState++;
			break;

		case 1 :
			if( m_bMotionEnd )
			{
				m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_WORRY_L], true, 5.0f );
				m_fFrameCounter = (rand() % 3 + 3) * 30.0f;
				m_iCharSubState++;
			}
			break;

		case 2 :
			if( m_fFrameCounter <= 0.0f )
			{
				m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_WORRY_E], false, 5.0f );
				m_iCharSubState++;
			}
			break;

		case 3 :
			if( m_bMotionEnd )
			{
				changeStateTo( ST_IDLING );
			}
			break;
	}
}

//------------
// ����������
//------------
void cMMDChar::stHeadBeat( void )
{
	switch( m_iCharSubState )
	{
		case 0 :
			m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_HEAD_BEAT], false, 0.0f );
			m_iCharSubState++;
			break;

		case 1 :
			if( m_bMotionEnd )
			{
				changeStateTo( ST_IDLING );
			}
			break;
	}
}

//------------
// ���Ȃł��
//------------
void cMMDChar::stHeadStroke( void )
{
	switch( m_iCharSubState )
	{
		case 0 :
			m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_HEAD_STROKE_S], false, 5.0f );
			m_iCharSubState++;
			break;

		case 1 :
			if( m_bMotionEnd )
			{
				m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_HEAD_STROKE_L], true, 5.0f );
				m_fFrameCounter = 2 * 30.0f;
				m_iCharSubState++;
			}
			break;

		case 2 :
			if( m_fFrameCounter < 0.0f && !m_bStrokeNow )
			{
				m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_HEAD_STROKE_E], false, 5.0f );
				m_iCharSubState++;
			}
			break;

		case 3 :
			if( m_bMotionEnd )
			{
				changeStateTo( ST_IDLING );
			}
			break;
	}
}

//----------
// ���G���
//----------
void cMMDChar::stBustTouch( void )
{
	switch( m_iCharSubState )
	{
		case 0 :
			m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_BUST_TOUCH], false, 0.0f );
			m_iCharSubState++;
			break;

		case 1 :
			if( m_bMotionEnd )
			{
				changeStateTo( ST_INVESTIGATE );
			}
			break;
	}
}

//------------------
// �X�J�[�g�߂����
//------------------
void cMMDChar::stSkirtLift( void )
{
	switch( m_iCharSubState )
	{
		case 0 :
			m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_SKIRT_LIFT], false, 0.0f );
			m_iCharSubState++;
			break;

		case 1 :
			if( m_bMotionEnd )
			{
				changeStateTo( ST_INVESTIGATE );
			}
			break;
	}
}

//------------
// ����񂯂�
//------------
void cMMDChar::stJanken( unsigned int uiJankenFlag )
{
	switch( m_iCharSubState )
	{
		case 0 :
			m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_JANKEN_START], false, 3.0f );
			m_iCharSubState++;
			break;

		case 1 :
			if( m_bMotionEnd )
			{
				m_uiMyJankenFlag = 1 << (rand() % 3);
				switch( m_uiMyJankenFlag )
				{
					case JANKEN_GOO :	m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_JANKEN_GOO],   false, 0.0f );	break;
					case JANKEN_CHOKI :	m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_JANKEN_CHOKI], false, 0.0f );	break;
					case JANKEN_PAR :	m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_JANKEN_PAR],   false, 0.0f );	break;
				}
				m_iCharSubState++;
			}
			break;

		case 2 :
			if( m_bMotionEnd )
			{
				if( uiJankenFlag == m_uiMyJankenFlag )
				{
					// ������
					m_iCharSubState = 0;
				}
				else if( uiJankenFlag == JANKEN_GOO )
				{
					if( m_uiMyJankenFlag == JANKEN_CHOKI )		changeStateTo( ST_JANKEN_LOSE );
					else if( m_uiMyJankenFlag == JANKEN_PAR )	changeStateTo( ST_JANKEN_WIN );
				}
				else if( uiJankenFlag == JANKEN_CHOKI )
				{
					if( m_uiMyJankenFlag == JANKEN_GOO )		changeStateTo( ST_JANKEN_WIN );
					else if( m_uiMyJankenFlag == JANKEN_PAR )	changeStateTo( ST_JANKEN_LOSE );
				}
				else if( uiJankenFlag == JANKEN_PAR )
				{
					if( m_uiMyJankenFlag == JANKEN_GOO )		changeStateTo( ST_JANKEN_LOSE );
					else if( m_uiMyJankenFlag == JANKEN_CHOKI )	changeStateTo( ST_JANKEN_WIN );
				}
				else
				{
					// �s��
					changeStateTo( ST_INVESTIGATE );
				}
			}
			break;
	}
}

//----------------
// ����񂯂񏟂�
//----------------
void cMMDChar::stJankenWin( void )
{
	switch( m_iCharSubState )
	{
		case 0 :
			m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_JANKEN_WIN], false, 3.0f );
			m_iCharSubState++;
			break;

		case 1 :
			if( m_bMotionEnd )
			{
				changeStateTo( ST_GUTS );
			}
			break;
	}
}

//----------------
// ����񂯂񕉂�
//----------------
void cMMDChar::stJankenLose( void )
{
	switch( m_iCharSubState )
	{
		case 0 :
			m_clPMDModel.setMotion( &m_clVMDMotArray[MOT_JANKEN_LOSE], false, 3.0f );
			m_iCharSubState++;
			break;

		case 1 :
			if( m_bMotionEnd )
			{
				changeStateTo( ST_IDLING );
			}
			break;
	}
}

//======
// �`��
//======
void cMMDChar::render( void )
{
	m_clPMDModel.render();
}

void cMMDChar::renderForShadow( void )
{
	m_clPMDModel.renderForShadow();
}

//======
// ���
//======
void cMMDChar::release( void )
{
	m_pHeadBone = NULL;
	m_pBustBone = NULL;
	m_pSkirtBone = NULL;

	m_clPMDModel.release();

	for( int i = 0 ; i < MOT_MAXNUM ; i++ )
	{
		m_clVMDMotArray[i].release();
	}
}
