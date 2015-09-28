//******************************************
// PMDモデルとVMDモーションで動くキャラクタ
//******************************************

#ifndef		_MMDCHAR_H_
#define		_MMDCHAR_H_

#include	"PMDModel.h"
#include	"VMDMotion.h"

class cMMDChar
{
	private :
		enum MMDCHAR_MOT
		{
			MOT_IDLING,

			MOT_GREETING,

			MOT_GUTS_S,
			MOT_GUTS_L,
			MOT_GUTS_E,

			MOT_SIDE_S,
			MOT_SIDE_L,
			MOT_SIDE_E,

			MOT_INVESTIGATE_S,
			MOT_INVESTIGATE_L,
			MOT_INVESTIGATE_E,

			MOT_WORRY_S,
			MOT_WORRY_L,
			MOT_WORRY_E,

			MOT_HEAD_BEAT,

			MOT_HEAD_STROKE_S,
			MOT_HEAD_STROKE_L,
			MOT_HEAD_STROKE_E,

			MOT_BUST_TOUCH,

			MOT_SKIRT_LIFT,

			MOT_JANKEN_START,
			MOT_JANKEN_GOO,
			MOT_JANKEN_CHOKI,
			MOT_JANKEN_PAR,
			MOT_JANKEN_WIN,
			MOT_JANKEN_LOSE,

			MOT_MAXNUM
		};

		static const char
						*m_pszMotionFileNames[MOT_MAXNUM];

		cPMDModel		m_clPMDModel;
		cVMDMotion		m_clVMDMotArray[MOT_MAXNUM];

		cPMDBone		*m_pHeadBone;
		cPMDBone		*m_pBustBone;
		cPMDBone		*m_pSkirtBone;

		bool			m_bModelLoaded;

		enum MMDCHAR_STATE
		{
			ST_IDLING,
			ST_GREETING,
			ST_GUTS,
			ST_SIDE,
			ST_INVESTIGATE,
			ST_WORRY,
			ST_HEAD_BEAT,
			ST_HEAD_STROKE,
			ST_BUST_TOUCH,
			ST_SKIRT_LIFT,
			ST_JANKEN,
			ST_JANKEN_WIN,
			ST_JANKEN_LOSE,

			ST_MAXNUM
		};

		int				m_iCharState;
		int				m_iCharSubState;

		void stIdling( void );
		void stGreeting( void );
		void stGuts( void );
		void stSide( void );
		void stInvestigate( void );
		void stWorry( void );
		void stHeadBeat( void );
		void stHeadStroke( void );
		void stBustTouch( void );
		void stSkirtLift( void );
		void stJanken( unsigned int uiJankenFlag );
		void stJankenWin( void );
		void stJankenLose( void );

		float			m_fFrameCounter;
		bool			m_bMotionEnd;
		unsigned int	m_uiMyJankenFlag;

		Vector3			m_vec3CurVec;
		Vector3			m_vec3PrevCurPos;
		float			m_fCurVecLength;

		float			m_fStrokeCount;
		bool			m_bStrokeNow;

		float			m_fTouchCount;

		bool loadMotions( void );
		void reset( void );
		void calcCursorVec( float fInvElapsedFrame, const Vector3 *pvec3CursorPos );

		inline void changeStateTo( enum MMDCHAR_STATE eCharState )
		{
			m_iCharState = eCharState;
			m_iCharSubState = 0;
			m_fFrameCounter = 0.0f;
			m_bMotionEnd = false;
		}

	public :
		cMMDChar( void );
		~cMMDChar( void );

		bool initialize( void );
		bool loadModel( const char *szFilePath );
		void startJankenMode( void );

		void update( float fElapsedFrame, const Vector3 *pvec3CameraPos, const Vector3 *pvec3CursorPos, unsigned int uiJankenFlag );
		void render( void );
		void renderForShadow( void );

		void release( void );

};

#endif	// _MMDCHAR_H_
